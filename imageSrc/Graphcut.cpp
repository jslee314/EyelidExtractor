
#include "stdafx.h"
#include "Graphcut.h"

#include "graph.h"
#include "math.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _MSC_VER
#pragma warning(disable: 4661)

#endif




void Graphcut(const CByteImage& imageIn, CByteImage& imageOut, CByteImage& GC_obj_seed, CByteImage& GC_bck_seed, Roi roi) {

	int nWidth = imageIn.GetWidth();
	int nHeight = imageIn.GetHeight();
	//int slicesize = nWidth*nHeight;
	int VOI_slicesize = nWidth*nHeight;

	BYTE* pIn = imageIn.GetPtr();
	BYTE* pOut = imageOut.GetPtr();

	BYTE* pObjs = GC_obj_seed.GetPtr();
	BYTE* pBcks = GC_bck_seed.GetPtr();

	int i,j;
	int VOI_width = (roi.endX - roi.stX);
	int VOI_heigh = (roi.endY - roi.stY);
	//int VOI_slicesize = VOI_width*VOI_heigh;


	// (1) Regional weight 계산을위한변수
	int bckseed_size=0;	// bckseed_size : VOI의 한슬라이스크기
	int objseed_size=0;

	// seeds 구조체선언
	struct seed {
		int row;		// seed 좌표에서 x좌표
		int colum;		// seed 좌표에서 y좌표
		int intensity;	// 해당 좌표에서seed intensity
	};

	// obj와 bck seeds 배열선언
	struct seed *m_objseed = (seed*)calloc(VOI_slicesize, sizeof(seed));
	struct seed *m_bckseed = (seed*)calloc(VOI_slicesize, sizeof(seed));


	// (2) Boundary weight 계산을위한변수
	double sigma = 50.0;
	int ave_objseed = 0, ave_bckseed = 0;	// intensity value 평균값저장변수
	int o_sum = 0, b_sum = 0;

	// ############################
	//	weight(constrain) 설정
	// ############################

	// Intensity constrain (seed 저장)

	for (int r = roi.stY; r < roi.endY; r++) {
		for (int c = roi.stX; c < roi.endX; c++) {
			int index = r*nWidth + c;
			if (pObjs[index] == OBJECT) {
				m_objseed[objseed_size].row = r;
				m_objseed[objseed_size].colum = c;
				m_objseed[objseed_size].intensity = (int)pIn[index];
				objseed_size++;
			}

			if (pBcks[index] == OBJECT) {
				m_bckseed[bckseed_size].row = r;
				m_bckseed[bckseed_size].colum = c;
				m_bckseed[bckseed_size].intensity = (int)pIn[index];
				bckseed_size++;
			}
		}

	}

	//##############################
	//	슬라이스별로 graph cut 실행
	//#############################

	// [1] Graph 생성
	typedef Graph<int, int, int> GraphType;
	GraphType *g = new GraphType(VOI_slicesize, VOI_slicesize);

	// [2] node 추가
	for (i = 0; i < VOI_slicesize; i++) {
		g->add_node();
	}
	// [3] object,background seeds의 intensity value의 average 계산
	// ave_objseed
	for (int r = 0; r < objseed_size; r++) {
		o_sum += m_objseed[r].intensity;
	}
	if (objseed_size == 0) {	// objseed_size[z-start.z]가 0이면 에러!
		ave_objseed = 0;
	}
	else {
		ave_objseed = o_sum / objseed_size;
	}

	// ave_bckseed
	for (int r = 0; r < bckseed_size; r++) {
		b_sum += m_bckseed[r].intensity;
	}
	if (bckseed_size == 0) {	// bckseed_size[z-start.z]가 0이면 에러!
		ave_bckseed = 0;
	}
	else {
		ave_bckseed = b_sum / bckseed_size;
	}

	printf("seed: %d, %d", ave_objseed, ave_bckseed);
	b_sum = 0;
	o_sum = 0;

	// [4] weight 계산 후 edge 추가
	for (int i = roi.stY; i < roi.endY; i++) {
		for (int j = roi.stX; j < roi.endX; j++) {
			int index =  i*nWidth + j;	// index : p (특정 pixel)

			double omin = 10000.0, bmin = 10000.0;

			double w = 0.5;	// regional과 boundary weight 추가	// source(object)

			//(1) Regional weight 계산 후 edge 추가 
			int cap_source = w *(abs(pIn[index] - ave_bckseed));	// back와 얼마나 다른가 (object)
			int cap_sink = w *(abs(pIn[index] - ave_objseed));
			//int cap_source = w *(abs(pIn[i*total.x + j] - ave_bckseed) + 10 * postPr_kidney[(z - start.z)*VOI.x*VOI.y + (i - start.y)*VOI.x + (j - start.x)]);	// back와 얼마나 다른가 (object)
			//int cap_sink = w *(abs(pIn[i*total.x + j] - ave_objseed) + 10 * postPr_background[(z - start.z)*VOI.x*VOI.y + (i - start.y)*VOI.x + (j - start.x)]

			if (cap_sink < 0) {
				cap_sink = 0;
				cap_source = cap_source - cap_sink;
			}
			g->add_tweights(index, cap_source, cap_sink);

			//(2) Boundary weight 계산 후 edge 추가
			if (i > roi.stY && j > roi.stX && i < roi.endY - 1 && j < roi.endX - 1) {
				for (int r = 0; r < 3; r++) {
					for (int c = 0; c < 3; c++) {

						int node_i = i*nWidth + j;
						int node_j = (i + r - 1)*nWidth + (j + c - 1);
						int nei_index = (i - roi.stY + r - 1)*nWidth + (j - roi.stX + c - 1);
						int nei = -1 * (pIn[node_i] - pIn[node_j]) * (pIn[node_i] - pIn[node_j]);

						int edge = (1 - w)*  (int)(exp((double)nei / (2 * (sigma*sigma))));

						if (r == 0 && c == 0) {
							g->add_edge(index, index - nWidth - 1, edge, 0);
						}
						else if (r == 0 && c == 1) {
							g->add_edge(index, index - nWidth, edge, 0);
						}
						else if (r == 0 && c == 2) {
							g->add_edge(index, index - nWidth + 1, edge, 0);
						}
						else if (r == 1 && c == 0) {
							g->add_edge(index, index - 1, edge, 0);
						}
						else if (r == 1 && c == 1) {
							break;
						}
						else if (r == 1 && c == 2) {
							g->add_edge(index, index + 1, edge, 0);
						}
						else if (r == 2 && c == 0) {
							g->add_edge(index, index +nWidth - 1, edge, 0);
						}
						else if (r == 2 && c == 1) {
							g->add_edge(index, index + nWidth, edge, 0);
						}
						else if (r == 2 && c == 2) {
							g->add_edge(index, index + nWidth + 1, edge, 0);
						}

					}
				}
			}
		}
	}

	// seeds 부분weight 다시계산 : seed의 neighbor 인지
	for (int r = 0; r < bckseed_size; r++) {
		int index = (m_bckseed[r].row - roi.stY)*nWidth + (m_bckseed[r].colum - roi.stX);
		int cap_source = abs(m_bckseed[r].intensity - ave_bckseed);
		int cap_sink = abs(m_bckseed[r].intensity - ave_objseed);
		if (cap_sink < 0) {
			cap_sink = 0;
			cap_source = cap_source - cap_sink;
		}
		g->nodes[index].tr_cap = 0;		// 초기tr_cap=0 으로재설정
		g->add_tweights(index, 0, 10000);	// background seed면 무한대
	}

	for (int r = 0; r < objseed_size; r++) {
		int index = (m_objseed[r].row - roi.stY)*nWidth + (m_objseed[r].colum - roi.stX);
		int cap_source = abs(m_objseed[r].intensity - ave_bckseed);
		int cap_sink = abs(m_objseed[r].intensity - ave_objseed);
		if (cap_sink < 0) {
			cap_sink = 0;
			cap_source = cap_source - cap_sink;
		}
		g->nodes[index].tr_cap = 0;		// 초기tr_cap=0 으로재설정
		g->add_tweights(index, 10000, 0); // object seed면 무한대
	}



	// [5] Segmentation
	int flow = g->maxflow();

	for (int r = roi.stY; r < roi.endY; r++) {
		for (int c = roi.stX; c < roi.endX; c++) {
			int index = r*nWidth + c;	
			if (g->what_segment(index) == GraphType::SOURCE) {
				pOut[index] = OBJECT;

			}
			else {
				pOut[index] = BACKGROUND;
			}
		}
	}
	delete g;


}
