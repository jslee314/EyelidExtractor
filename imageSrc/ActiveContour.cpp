#include "stdafx.h"
#include "ActiveContour.h"


// ---------------------------------------------------------------------
//                            생성자 및 소멸자 
// ---------------------------------------------------------------------

Snake::Snake(void) 
{
}
Snake::Snake(int width, int height, int* gradient, int* flow, VECTOR_P circle) {
	width = width;			// size of the image and of the 2 following arrays
	height = height;
	chanel_gradient = gradient;	// gradient (modulus)
	chanel_flow = flow;			// gradient flow (modulus)
	snake = circle;			// inital points of the snake
}
Snake::~Snake(void) 
{
}

// ---------------------------------------------------------------------
//                    멤버함수: START SNAKE SEGMENTATION
// ---------------------------------------------------------------------

// 1. Snake 반복 수행  /* main loop() */
int Snake::loop() {

	int loop = 0;

	while (step()  &&  loop < MAXITERATION) {	//MAXITERATION: 10

		// auto adapt the number of points in the snake
		if ((loop%AUTOADAPT_LOOP) == 0) {	//AUTOADAPT_LOOP: 10 즉 loop가 10, 20, 30,..마다 포인트 제거 삽입 진행한다.
			printf("auto loop   : %d\n", loop);
			printf("snake length: %d\n\n", snake.size());
				
			removeOverlappingPoints(AUTOADAPT_MINLEN);
			addMissingPoints(AUTOADAPT_MAXLEN);
			//printf("%d:remv snake size: %d\n",loop, snake.size());
		
			//printf("%d:add snake size: %d\n\n", loop, snake.size());
		}
		
		loop++;
	}
	
	// rebuild using spline interpolation
	rebuild(AUTOADAPT_MAXLEN);	// AUTOADAPT_MAXLEN: 점 사이 거리
	

	return loop;	// return: the number of iterations performed
}

// 2. update the position of each point of the snake
bool Snake::step() {


	bool changed = false;
	PointS p;	p.x = 0;	p.y = 0;

	// compute length of original snake (used by method: f_uniformity)
	snakelength = getsnakelength();

	// compute the new snake
	//VECTOR_P newsnake(snake.size());	// <- List<PointS> newsnake = new ArrayList<PointS>(snake.size());
	VECTOR_P newsnake;

	// for each point of the previous snake
	for (int i = 0; i<snake.size(); i++) {

		// 1. snake중에서 prev, cur, next의 값 참조
		int nPrev = (int)((i + snake.size() - 1) % snake.size());
		int nNext = (int)((i + 1) % snake.size());
	
		PointS prev = snake[nPrev];
		PointS cur  = snake[i];
		PointS next = snake[nNext];
		
		// 2. 3x3 이웃범위에 있는 점 각각, 4개의 에너지 함수를 계산 및 정규화
		for (int dy = -1; dy <= 1; dy++) {
			for (int dx = -1; dx <= 1; dx++) {
				p.x = cur.x + dx;		//???????????p.setLocation(cur.x + dx, cur.y + dy);
				p.y = cur.y + dy;
				
				e_uniformity[1 + dx][1 + dy] = f_uniformity(prev, next, p);
				e_curvature[1 + dx][1 + dy]  = f_curvature(prev, p, next);
				e_flow[1 + dx][1 + dy]       = f_gflow(cur, p);
				e_inertia[1 + dx][1 + dy]    = f_inertia(cur, p);
			}
		}
		normalize(e_uniformity);// normalize energies
		normalize(e_curvature);
		normalize(e_flow);
		normalize(e_inertia);

		// 3. 3x3 이웃중에서 에너지합이 가장 작은 점 x와 y를 찾아라
		double emin = 99999999;
		double e = 0;
		int x = 0;
		int y = 0;
		
		for (int dy = -1; dy <= 1; dy++) {
			for (int dx = -1; dx <= 1; dx++) {
				e = 0;
				e += alpha * e_uniformity[1 + dx][1 + dy]; // internal energy
				e += beta  * e_curvature[1 + dx][1 + dy];  // internal energy
				e += gamma * e_flow[1 + dx][1 + dy];       // external energy
				e += delta * e_inertia[1 + dx][1 + dy];    // external energy
				
				if (e<emin) { 
					emin = e; 
					x = cur.x + dx; 
					y = cur.y + dy; 
				}
			}
		}

		// 4. 그때의 x와 y가 이미지 내부에 있는지 체크!
		if (x < 1) {x = 1;}
		if (x >= (width - 1)){x = width - 2;}
		if (y < 1) {y = 1;}
		if (y >= (height - 1)) {y = height - 2;}


		// 5. 그렇게 찾은 점이 중앙의 원래 내 위치가 아닌 이웃중에 하나면(점의 위치가 바꼈으면) change true로!
		if (x != cur.x || y != cur.y) {
			changed = true;
		}

		// create the PointS in the new snake
	
		PointS pushP; 	
		pushP.x = x;		pushP.y = y;
		newsnake.push_back(pushP);
	}

	// new snake becomes current
	snake = newsnake;
	

	return changed;	//return true if the snake has changed, otherwise false.
}

// 3. normalize energy matrix
void Snake::normalize(double array3x3[3][3]) {
	double sum = 0;
	for (int i = 0; i<3; i++)
		for (int j = 0; j<3; j++)
			sum += abs(array3x3[i][j]);

	if (sum == 0) return;

	for (int i = 0; i<3; i++)
		for (int j = 0; j<3; j++)
			array3x3[i][j] /= sum;
}
double Snake::getsnakelength() {
	// total length of snake
	double length = 0;
	for (int i = 0; i<snake.size(); i++) {
		int nNext = (int)((i + 1) % snake.size());
		PointS cur = snake[i];
		PointS next = snake[nNext];
		length += distance2D(cur, next);
	}
	return length;
}
double Snake::distance2D(PointS A, PointS B) {
	int ux = A.x - B.x;
	int uy = A.y - B.y;
	double un = ux * ux + uy * uy;
	return sqrt(un);
}

// ---------------------------------------------------------------------
//                    멤버함수:  ENERGY FUNCTIONS 
// ---------------------------------------------------------------------

double Snake::f_uniformity(PointS prev, PointS next, PointS p) {
	// length of previous segment
	double un = distance2D(prev, p);

	// mesure of uniformity
	double avg = snakelength / snake.size();
	double dun = abs(un - avg);

	// elasticity energy
	return (dun * dun);
}

double Snake::f_curvature(PointS prev, PointS p, PointS next) {
	int ux = p.x - prev.x;
	int uy = p.y - prev.y;
	double un = sqrt(ux*ux + uy * uy);

	int vx = p.x - next.x;
	int vy = p.y - next.y;
	double vn = sqrt(vx*vx + vy * vy);

	if (un == 0 || vn == 0) return 0;

	double cx = (vx + ux) / (un*vn);
	double cy = (vy + uy) / (un*vn);

	// curvature energy
	double cn = cx * cx + cy * cy;
	return cn;
}

double Snake::f_gflow(PointS cur, PointS p) {
	// gradient flow
	
	int dcur = chanel_flow[cur.x + cur.y*width];
	int dp = chanel_flow[p.x+ p.y*width];
	double d = dp - dcur;
	return d;
}

double Snake::f_inertia(PointS cur, PointS p) {
	double d = distance2D(cur, p);
	double g = chanel_gradient[cur.x+cur.y*width];
	double e = g * d;
	return e;
}
// ---------------------------------------------------------------------
//                    멤버함수:  AUTOADAPT
// ---------------------------------------------------------------------

// 1. rebuild the snake using cubic spline interpolation
void Snake::rebuild(int space) {

	// precompute length(i) = length of the snake from start to PointS #i
	double* clength = new double[snake.size() + 1];
	clength[0] = 0;
	for (int i = 0; i<snake.size(); i++) {
		PointS cur = snake[i];
		PointS next = snake[(i + 1) % snake.size()];
		clength[i + 1] = clength[i] + distance2D(cur, next);
	}

	// compute number of PointSs in the new snake
	double total = clength[snake.size()];
	int nmb = (int)(0.5 + total / space);

	// build a new snake
	//VECTOR_P newsnake(snake.size());
	VECTOR_P newsnake;

	for (int i = 0, j = 0; j<nmb; j++) {
		// current length in the new snake
		double dist = (j*total) / nmb;

		// find corresponding interval of PointSs in the original snake
		while (!(clength[i] <= dist && dist<clength[i + 1])) i++;

		// get PointSs (P-1,P,P+1,P+2) in the original snake
		PointS prev = snake[(i + snake.size() - 1) % snake.size()];
		PointS cur = snake[i];
		PointS next = snake[(i + 1) % snake.size()];
		PointS next2 = snake[(i + 2) % snake.size()];

		// do cubic spline interpolation
		double t = (dist - clength[i]) / (clength[i + 1] - clength[i]);
		double t2 = t * t, t3 = t2 * t;
		double c0 = 1 * t3;
		double c1 = -3 * t3 + 3 * t2 + 3 * t + 1;
		double c2 = 3 * t3 - 6 * t2 + 4;
		double c3 = -1 * t3 + 3 * t2 - 3 * t + 1;
		double x = prev.x*c3 + cur.x*c2 + next.x* c1 + next2.x*c0;
		double y = prev.y*c3 + cur.y*c2 + next.y* c1 + next2.y*c0;
	
		// add computed point to the new snake
		PointS newpoint;
		newpoint.x = (int)(0.5 + x / 6);
		newpoint.y = (int)(0.5 + y / 6);
		newsnake.push_back(newpoint);
	}
	snake = newsnake;
	delete clength;
}
// 2. 
void Snake::removeOverlappingPoints(int minlen) {

	// for each point of the snake
	for (int i = 0; i<snake.size(); i++) {
		
		PointS cur = snake[i];

		// check the other points (right half)
		for (int di = 1 + snake.size()/2; di>0; di--){
			PointS end = snake[((i + di) % snake.size())];
			double dist = distance2D(cur, end);
			
			//printf("%d: dist: %f\n", di, dist);
			int e = ((i + 1) % snake.size());
			// if the two points are to close..->>.for문을 건너뛴다!
			if (dist > minlen){
				continue; 
			}

			//printf("minlen 보다 dist 가 작은점 있음\n");
			// ... cut the "loop" part og the snake
			for (int k = 0; k < di; k++) {
				int e = ((i + 1) % snake.size());
				snake.erase(snake.begin() + e -1);
			}
			
			break;	// ->>.for문을 빠져나간다.!
		}
		


	}
}
// 3. 
void Snake::addMissingPoints(int maxlen) {
	// for each point of the snake
	for (int i = 0; i<snake.size(); i++) {
		PointS prev = snake[(i + snake.size() - 1) % snake.size()];
		PointS cur = snake[i];
		PointS next = snake[(i + 1) % snake.size()];
		PointS next2 = snake[(i + 2) % snake.size()];

		// if the next point is to far then add a new point
		if (distance2D(cur, next)>maxlen) {

			// precomputed Uniform cubic B-spline for t=0.5
			double c0 = 0.125 / 6.0, c1 = 2.875 / 6.0, c2 = 2.875 / 6.0, c3 = 0.125 / 6.0;
			double x = prev.x*c3 + cur.x*c2 + next.x* c1 + next2.x*c0;
			double y = prev.y*c3 + cur.y*c2 + next.y* c1 + next2.y*c0;
			
			// add computed point to the new snake
			PointS newpoint;
			newpoint.x = (int)(0.5 + x);
			newpoint.y = (int)(0.5 + y);
			snake.push_back(newpoint);
	
			i--; 
		}
	}
}

// ---------------------------------------------------------------------
//            SNAKE 수행을 위한 일반 함수 (Snake class 함수 아님)
// ---------------------------------------------------------------------


void display(CByteImage& m_imageOut, VECTOR_P snake) {

	VECTOR_P snakepoints = snake;

	for (int i = 0; i < snakepoints.size(); i++) {
		int j = (i + 1) % snakepoints.size();
		PointS p1 = snakepoints[i];
		PointS p2 = snakepoints[j];
		DrawLine(m_imageOut, p1.x, p1.y, p2.x, p2.y, OBJECT);
	}
}


VECTOR_P initialPoint(int in_MAXLEN) {

	int width = 640; int height = 480;

	int MAXLEN = in_MAXLEN;	//  max segment length 
						// initial points
	double radius = (width / 2 + height / 2) / 2;
	double perimeter = 6.28 * radius;
	int nmb = (int)(perimeter / MAXLEN);
	VECTOR_P circle(nmb);
	//VECTOR_P circle;

	for (int i = 0; i < nmb; i++) {
//		double x = (width / 2 + 0) + (width / 3 - 2) * cos((6.28 * i) / circle.size());
//		double y = (height / 2 + 0) + (height / 6 - 2) * sin((6.28 * i) / circle.size());
		double x = (width / 2 + 0) + (width / 2 - 2) * cos((6.28 * i) / circle.size());
		double y = (height / 2 + 0) + (height / 2 - 2) * sin((6.28 * i) / circle.size());

		
		x = (int)x;
		y = (int)y;
		
		if (x < 1) { x = 1; }
		if (x >= (width - 1)) { x = width - 2; }
		if (y < 1) { y = 1; }
		if (y >= (height - 1)) { y = height - 2; }

	//	PointS pushP;
	//	pushP.x = x;		pushP.y = y;
	//	circle.push_back(pushP);

		circle[i] = { (int)x, (int)y };
	}

	return circle;

}


void computegflow(const CByteImage& imageIn, int* chanel_gradient, int* chanel_flow, int THRESHOLD) {
	//int W = 640;	int H = 480;

	int W = imageIn.GetWidth();
	int H = imageIn.GetHeight();



	//int THRESHOLD = 50;///////////////////////////////???????????

	BYTE* pIn = imageIn.GetPtr();

	// GrayLevelScale (Luminance)
	int *clum = new int[W*H];
	memset(clum, 0, sizeof(int)*W*H);
	for (int y = 0; y < H; y++) {
		for (int x = 0; x < W; x++) {
			clum[x + W * y] = (int)pIn[y*W + x];
		}
	}

	// Gradient (sobel)----------------------------------------------------------------------------
	int maxgradient = 0;
	for (int y = 1; y < H - 1; y++) {
		for (int x = 1; x < W - 1; x++) {

			int p00 = clum[(x - 1) + W * (y - 1)]; int p10 = clum[(x + 0) + W * (y - 1)]; int p20 = clum[(x + 1) + W * (y - 1)];
			int p01 = clum[(x - 1) + W * (y + 0)]; /* ------------------------------ */ int p21 = clum[(x + 1) + W * (y + 0)];
			int p02 = clum[(x - 1) + W * (y + 1)]; int p12 = clum[(x + 0) + W * (y + 1)]; int p22 = clum[(x + 1) + W * (y + 1)];

			int sx = (p20 + 2 * p21 + p22) - (p00 + 2 * p01 + p02);
			int sy = (p02 + 2 * p12 + p22) - (p00 + 2 * p10 + p20);

			BYTE snorm = (BYTE)sqrt(sx*sx + sy * sy);
			chanel_gradient[x + y * W] = snorm;

			maxgradient = max(maxgradient, snorm);
		}
	}
	//Printf("maxgradient: %d\n", maxgradient);



	// thresholding--------------------------------------------------------------------------------
	bool *binarygradient = new bool[W*H];
	memset(binarygradient, false, sizeof(bool)*W*H);

	for (int y = 0; y < H; y++) {
		for (int x = 0; x < W; x++) {
			if (chanel_gradient[y*W + x] > THRESHOLD*maxgradient / 100) {
				binarygradient[y*W + x] = true;
				//chanel_gradient[y*W + x] = 255;
			}
			else {
				chanel_gradient[y*W + x] = 0;
			}
		}
	}



	//********************** chamfer Distance map  ****************************
	// distance map to binarized gradient
	double * cdist = new double[W*H];
	memset(cdist, 0, sizeof(double)*W*H);


	int *chamfer5 = new int[9];
// 	chamfer5[0] = 1;	chamfer5[1] = 0;	chamfer5[2] = 5;
// 	chamfer5[3] = 1;	chamfer5[4] = 1;	chamfer5[5] = 11;
// 	chamfer5[6] = 5;	chamfer5[7] = 1;	chamfer5[8] = 11;

	chamfer5[0] = 1;	chamfer5[1] = 0;	chamfer5[2] = 5;
	chamfer5[3] = 1;	chamfer5[4] = 1;	chamfer5[5] = 7;
	chamfer5[6] = 2;	chamfer5[7] = 1;	chamfer5[8] = 11;



	ChamferDistance _chamfer = ChamferDistance(chamfer5);
	_chamfer.compute(cdist, binarygradient, W, H);
	
	for (int y = 0; y < H; y++) {
		for (int x = 0; x < W; x++) {
			chanel_flow[y*W + x] = (int)(5 * cdist[y*W + x]);
		}
	}








	delete[] clum;
	delete[] cdist;
	delete[] binarygradient;



}