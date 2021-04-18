#include "StdAfx.h"
#include "RegionGrowingAlgorithm.h"
//#include "ArcheImageData.h"
//#include "vcmi.h"
#include "imageIO/MyImage.h"


RegionGrowingAlgorithm::RegionGrowingAlgorithm(void)
{
}

RegionGrowingAlgorithm::~RegionGrowingAlgorithm(void)
{
}

BYTE* RegionGrowingAlgorithm::GetSource()
{
	return _src;
}

BYTE* RegionGrowingAlgorithm::GetDestination()
{
	return _dst;
}

void RegionGrowingAlgorithm::SetSource( BYTE* src, int w, int h )
{
	_src = src;
	_width = w;
	_height = h;
}

void RegionGrowingAlgorithm::SetDestination( BYTE* dst, int w, int h )
{
	_dst = dst;
	_width = w;
	_height = h;
}

void RegionGrowingAlgorithm::SetImageData(CByteImage& imageData)
{
	_imageData = imageData;
}

bool RegionGrowingAlgorithm::SowSeed(int index, bool connectivity)
{
	if (_condition->IsGrowable(_src[index], _dst[index]))
	{
		if (!connectivity)
		{
			_Q.push(index);
			return true;
		}
	}
	return false;
}

// connectivity 변수의 필요성을 잘 모르겠어서 (seed point의 절약?) 
// connectivity 변수 생략하고 실험해보려고 override 했음: 잘됨
void RegionGrowingAlgorithm::SowSeed(int index)
{
	if (_condition->IsGrowable(_src[index], _dst[index]))
	{
		_Q.push(index);
		
	}
}

bool RegionGrowingAlgorithm::SowSeed3D(int index, bool connectivity)
{
	int w = _imageData.GetWidth(), h = _imageData.GetHeight();
	int sliceSize = w * h;

	int zPos = index / sliceSize,
		yPos = (index % sliceSize) / w,
		xPos = (index % sliceSize) % w;
	int xyIndex = yPos * w + xPos;

	/*if (_condition->IsGrowable(_imageData.GetBitmapData(zPos)[xyIndex], _imageData.GetMaskData(zPos)[xyIndex]))
	{
		if (!connectivity)
		{
			_Q.push(index);
			return true;
		}
	}*/
	return false;
}

// connectivity 변수의 필요성을 잘 모르겠어서 (seed point의 절약?) 
// connectivity 변수 생략하고 실험해보려고 override 했음: 잘됨
void RegionGrowingAlgorithm::SowSeed3D(int index)
{
	int w = _imageData.GetWidth(), h = _imageData.GetHeight();
	int sliceSize = w * h;

	int zPos = index / sliceSize,
		yPos = (index % sliceSize) / w,
		xPos = (index % sliceSize) % w;
	int xyIndex = yPos * w + xPos;

	if (_condition->IsGrowable(_imageData.GetBitmapData(zPos)[xyIndex], _imageData.GetMaskData(zPos)[xyIndex]))
	{
		_Q.push(index);
		
	}

}

void RegionGrowingAlgorithm::DoRegionGrowing(RegionGrowingCondition* condition, int seedX, int seedY )
{
	int w = _width, h = _height;
	int index = w * seedY + seedX;
	_condition = condition;

	if(seedX < 0 || seedX >= w || seedY < 0 || seedY >= h)
		return;

	if (!condition->IsGrowable(_src[index], _dst[index]))
	{
		return;
	}
	_Q.push(index);			// push(): queue 맨 뒤에 원소를 새로 추가

	do{
		index = _Q.front();	// front(): queue 맨 앞에 위치한 원소를 반환(삭제하지 않는다)
		_Q.pop();			// pop(): queue 맨 앞에 위치한 원소를 삭제(반환하지 않는다)

		int xPos = index % w,
			yPos = index / w;
		int	yIndex = w * yPos;

		bool topConnectivity = false;
		bool bottomConnectivity = false;
		
		// top line 인지 bottom line 인지 체크하는 변수
		bool topCheck = true;
		bool bottomCheck = true;

		// if top line or bottom line of slice 
		if(yPos == 0) topCheck = false;
		if(yPos == h-1) bottomCheck = false;

		bool isLineGrowable = true;

		// to left side from seed
		for(int x = xPos; x >= 0 && isLineGrowable; x--)
		{
			int newIndex = yIndex + x;
			if (!_condition->IsGrowable(_src[newIndex], _dst[newIndex]))
			{
				isLineGrowable = false;
			} 
			else 
			{
				_dst[x + yIndex] = _condition->GetGrowingValue();

				// top line 이 아닌 경우 위 픽셀 검사
				if(topCheck)
				{
					// above pixel check
					int upperIndex = newIndex - w;
					topConnectivity = SowSeed(upperIndex, topConnectivity);
				}

				// bottom line 이 아닌 경우 아래 픽셀 검사
				if(bottomCheck)
				{
					// below pixel check
					int lowerIndex = newIndex + w;
					bottomConnectivity = SowSeed(lowerIndex, bottomConnectivity);
				}
			} // else
		} // for(right)

		isLineGrowable = true;

		// to right side from seed
		for (int x = xPos + 1; x < w && isLineGrowable; x++)
		{
			int newIndex = yIndex + x;
			if (!_condition->IsGrowable(_src[newIndex], _dst[newIndex]))
			{
				isLineGrowable = false;
			} 
			else 
			{
				_dst[x + yIndex] = _condition->GetGrowingValue();

				if(topCheck)
				{
					// above pixel check
					int upperIndex = newIndex - w;
					topConnectivity = SowSeed(upperIndex, topConnectivity);
				}

				if(bottomCheck)
				{
					// below pixel check
					int lowerIndex = newIndex + w;
					bottomConnectivity = SowSeed(lowerIndex, bottomConnectivity);
				}
			} // else
		} // for(right)
	} while (!_Q.empty());
}

// 2D region growing: 해당 픽셀 주변 8방향을 탐색 범위로 수정
void RegionGrowingAlgorithm::DoRegionGrowingFor8Pixels(RegionGrowingCondition* condition, int seedX, int seedY )
{
	int w = _width, h = _height;
	int index = w * seedY + seedX;
	_condition = condition;

	if(seedX < 0 || seedX >= w || seedY < 0 || seedY >= h)
		return;

	if (!condition->IsGrowable(_src[index], _dst[index]))
	{
		return;
	}
	_Q.push(index);			// push(): queue 맨 뒤에 원소를 새로 추가

	do{
		index = _Q.front();	// front(): queue 맨 앞에 위치한 원소를 반환(삭제하지 않는다)
		_Q.pop();			// pop(): queue 맨 앞에 위치한 원소를 삭제(반환하지 않는다)

		int xPos = index % w,
			yPos = index / w;
		int	yIndex = w * yPos;

		// top line 인지 bottom line 인지 체크하는 변수
		bool topCheck = true;
		bool bottomCheck = true;

		// if top line or bottom line of slice 
		if(yPos == 0) topCheck = false;
		if(yPos == h-1) bottomCheck = false;

		bool isLineGrowable = true;

		// to left side from seed
		for(int x = xPos; x >= 0 && isLineGrowable; x--)
		{
			int newIndex = yIndex + x;

			if (!_condition->IsGrowable(_src[newIndex], _dst[newIndex]))
			{
				isLineGrowable = false;
			} 
			else 
			{
				// 실제 region growing 수행: 마스크값 세팅
				_dst[x + yIndex] = _condition->GetGrowingValue();

				int index;

				// 현재 픽셀 윗줄 검사
				if(topCheck)
				{
					for(int i = -1; i <= 1; i++)
					{
						index = newIndex - w + i;
						SowSeed(index);
					}
				}

				// 현재 픽셀 아래줄 검사
				if(bottomCheck)
				{
					for(int i = -1; i <= 1; i++)
					{
						index = newIndex + w + i;
						SowSeed(index);
					}
				}
			} // else
		} // for(right)

		isLineGrowable = true;

		// to right side from seed
		for (int x = xPos + 1; x < w && isLineGrowable; x++)
		{
			int newIndex = yIndex + x;
			if (!_condition->IsGrowable(_src[newIndex], _dst[newIndex]))
			{
				isLineGrowable = false;
			} 
			else 
			{
				// 실제 region growing 수행: 마스크값 세팅
				_dst[x + yIndex] = _condition->GetGrowingValue();

				int index;

				// 현재 픽셀 윗줄 검사
				if(topCheck)
				{
					for(int i = -1; i <= 1; i++)
					{
						index = newIndex - w + i;
						SowSeed(index);
					}
				}

				// 현재 픽셀 아래줄 검사
				if(bottomCheck)
				{
					for(int i = -1; i <= 1; i++)
					{
						index = newIndex + w + i;
						SowSeed(index);
					}
				}
			} // else
		} // for(right)
	} while (!_Q.empty());
}

void RegionGrowingAlgorithm::DoRegionGrowing3D(RegionGrowingCondition* condition, int seedX, int seedY, int seedZ,int startz,int endz,int startx,int endx,int starty,int endy)
{
	int w = _imageData.GetWidth(), h = _imageData.GetHeight();
	int sliceSize = w * h;
	int z = seedZ;


	/*
	// lastSlice id
	int lastSlice = _imageData->GetNumberOfSlices();

	_condition = condition;

	if(seedX < 0 || seedX >= w || seedY < 0 || seedY >= h || seedZ < 0 || seedZ >= lastSlice)
	{
		return;
	}*/

	int index = z * sliceSize + w * seedY + seedX;

	_Q.push(index);				// push(): queue 맨 뒤에 원소를 새로 추가

	do{
		index = _Q.front();		// front(): queue 맨 앞에 위치한 원소를 반환(삭제하지 않는다)
		_Q.pop();				// pop(): queue 맨 앞에 위치한 원소를 삭제(반환하지 않는다)

		int zPos = index / sliceSize,
			yPos = (index % sliceSize) / w,
			xPos = (index % sliceSize) % w;
		int	yIndex = w * yPos;
		
		// 현재 슬라이스에서 바로 위 픽셀과 아래 픽셀에 대한 연결성을 체크하기 위한 변수
		// 현재 픽셀에서 위 픽셀이나 아래 픽셀이 growing 되어야 할 픽셀인지 아닌지를 체크하기 위한 변수
		bool topConnectivity = false;
		bool bottomConnectivity = false;

		// 현재 슬라이스에서 z 축으로 바로 위 픽셀 또는 아래 픽셀이 growing 되어야 할 픽셀인지 아닌지를 체크하기 위한 변수
		bool upConnectivity = false;
		bool downConnectivity = false;

		// 맨 위 라인이나 맨 아래 라인이나 맨 위 슬라이스나 맨 아래 슬라이스인지 체크하는 변수
		// 그럼 위, 아래를 체크할 필요가 없으니까
		bool topCheck = true;
		bool bottomCheck = true;
		bool upCheck = true;
		bool downCheck = true;

		// if top line or bottom line of slice 
//		if(yPos == 0) topCheck = false;
//		if(yPos == h-1) bottomCheck = false;
		if(yPos == starty) topCheck = false;
		if(yPos == endy) bottomCheck = false;

		// if first or last slice
//		if(z == 0) upCheck = false;
//		if(z == lastSlice-1) downCheck = false;
		if(zPos == startz) upCheck = false;
		if(zPos == endz) downCheck = false;

		bool isLineGrowable = true;

		// to left side from seed
		for(int x = xPos; x >= startx && isLineGrowable; x--)
		{
			int xyIndex = yIndex + x;
			int newIndex = zPos * sliceSize + xyIndex;

			if (!_condition->IsGrowable(_imageData.GetBitmapData(zPos)[xyIndex], _imageData.GetMaskData(zPos)[xyIndex])) 
			{
				isLineGrowable = false;
			} 
			else 
			{
				// 실제 region growing 수행: 마스크값 세팅
				_imageData.GetMaskData(zPos)[xyIndex] = _condition->GetGrowingValue();

				if(topCheck) 
				{
					int upperIndex = newIndex - w;
					topConnectivity = SowSeed3D(upperIndex, topConnectivity);
				}

				if(bottomCheck)	
				{
					int lowerIndex = newIndex + w;
					bottomConnectivity = SowSeed3D(lowerIndex, bottomConnectivity);
				}

				if(upCheck)	
				{
					int upperSliceIndex = newIndex - sliceSize ;
					upConnectivity = SowSeed3D(upperSliceIndex, upConnectivity);
				}

				if(downCheck) 
				{
					int lowerSliceIndex = newIndex + sliceSize ;
					downConnectivity = SowSeed3D(lowerSliceIndex, downConnectivity);
				}
			} // else
		} // for(left)

		isLineGrowable = true;

		// to right side from seed
		for (int x = xPos + 1; x < endx && isLineGrowable; x++)
		{
			int xyIndex = yIndex + x;
			int newIndex = zPos * sliceSize + xyIndex;

			if (!_condition->IsGrowable(_imageData.GetBitmapData(zPos)[xyIndex], _imageData.GetMaskData(zPos)[xyIndex])) 
			{
				isLineGrowable = false;
			} 
			else 
			{
				// 실제 region growing 수행: 마스크값 세팅
				_imageData.GetMaskData(zPos)[xyIndex] = _condition->GetGrowingValue();

				if(topCheck) 
				{
					int upperIndex = newIndex - w;
					topConnectivity = SowSeed3D(upperIndex, topConnectivity);
				}

				if(bottomCheck) 
				{
					int lowerIndex = newIndex + w;
					bottomConnectivity = SowSeed3D(lowerIndex, bottomConnectivity);
				}

				if(upCheck)	
				{
					int upperSliceIndex = newIndex - sliceSize ;
					upConnectivity = SowSeed3D(upperSliceIndex, upConnectivity);
				}

				if(downCheck) 
				{
					int lowerSliceIndex = newIndex + sliceSize ;
					downConnectivity = SowSeed3D(lowerSliceIndex, downConnectivity);
				}
			} // else
		} // for(right)

	} while (!_Q.empty());
}

// 3D region growing: 해당 픽셀 주변 26방향을 탐색 범위로 수정
void RegionGrowingAlgorithm::DoRegionGrowing3DFor26Voxels(RegionGrowingCondition* condition, int seedX, int seedY, int seedZ,int startz,int endz,int startx,int endx,int starty,int endy)
{
	int w = _imageData.GetWidth(), h = _imageData.GetHeight();
	int sliceSize = w * h;
	int z = seedZ;

	// lastSlice id
	int lastSlice = endz;

	_condition = condition;

	if(seedX < 0 || seedX >= w || seedY < 0 || seedY >= h || seedZ < 0 || seedZ > lastSlice)
	{
		return;
	}

	int index = z * sliceSize + w * seedY + seedX;

	_Q.push(index);				// push(): queue 맨 뒤에 원소를 새로 추가

	do{
		index = _Q.front();		// front(): queue 맨 앞에 위치한 원소를 반환(삭제하지 않는다)
		_Q.pop();				// pop(): queue 맨 앞에 위치한 원소를 삭제(반환하지 않는다)

		int zPos = index / sliceSize,
			yPos = (index % sliceSize) / w,
			xPos = (index % sliceSize) % w;
		int	yIndex = w * yPos;


		// 맨 위 라인이나 맨 아래 라인이나 맨 위 슬라이스나 맨 아래 슬라이스인지 체크하는 변수
		// 그럼 위, 아래를 체크할 필요가 없으니까
		bool topCheck = true;
		bool bottomCheck = true;
		bool upCheck = true;
		bool downCheck = true;

		// if top line or bottom line of slice 
		if(yPos == starty) topCheck = false;
		if(yPos == endy) bottomCheck = false;

		// if first or last slice
		if(zPos == startz) upCheck = false;
		if(zPos == endz) downCheck = false;

		bool isLineGrowable = true;

		// to left side from seed
		for(int x = xPos; x >= startx && isLineGrowable; x--)
		{
			int xyIndex = yIndex + x;
			int newIndex = zPos * sliceSize + xyIndex;

			if (!_condition->IsGrowable(_imageData.GetBitmapData(zPos)[xyIndex], _imageData.GetMaskData(zPos)[xyIndex])) 
			{
				isLineGrowable = false;
			} 
			else 
			{
				// 실제 region growing 수행: 마스크값 세팅
				_imageData.GetMaskData(zPos)[xyIndex] = _condition->GetGrowingValue();

				int index;

				// 현재 픽셀 윗줄
				if(topCheck) 
				{
					for(int i = -1; i <= 1; i++)
					{
						index = newIndex - w + i;
						SowSeed3D(index);
					}
				}
	
				// 현재 픽셀 아랫줄
				if(bottomCheck)	
				{
					for(int i = -1; i <= 1; i++)
					{
						index = newIndex + w + i;
						SowSeed3D(index);
					}
				}
	
				// 현재 픽셀 이전 슬라이스
				if(upCheck)	
				{
					// 이전 슬라이스의 오른쪽 픽셀
					index = newIndex - sliceSize + 1;
					SowSeed3D(index);
					// 이전 슬라이스의 왼쪽 픽셀
					index = newIndex - sliceSize - 1;
					SowSeed3D(index);
					
					// 이전 슬라이스의 윗줄 3 픽셀
					if(topCheck) 
					{
						for(int i = -1; i <= 1; i++)
						{
							index = newIndex - sliceSize - w + i;
							SowSeed3D(index);
						}
					}

					// 이전 슬라이스의 아랫줄 3 픽셀
					if(bottomCheck)	
					{
						for(int i = -1; i <= 1; i++)
						{
							index = newIndex - sliceSize + w + i;
							SowSeed3D(index);
						}
					}
				}
				
				// 현재 픽셀 다음 슬라이스
				if(downCheck) 
				{
					// 이전 슬라이스의 오른쪽 픽셀
					index = newIndex + sliceSize + 1;
					SowSeed3D(index);
					// 이전 슬라이스의 왼쪽 픽셀
					index = newIndex + sliceSize - 1;
					SowSeed3D(index);

					// 이전 슬라이스의 윗줄 3 픽셀
					if(topCheck) 
					{
						for(int i = -1; i <= 1; i++)
						{
							index = newIndex + sliceSize - w + i;
							SowSeed3D(index);
						}
					}

					// 이전 슬라이스의 아랫줄 3 픽셀
					if(bottomCheck)	
					{
						for(int i = -1; i <= 1; i++)
						{
							index = newIndex + sliceSize + w + i;
							SowSeed3D(index);
						}
					}
				}

			} // else
		} // for(left)

		isLineGrowable = true;

		// to right side from seed
		for (int x = xPos + 1; x < endx && isLineGrowable; x++)
		{
			int xyIndex = yIndex + x;
			int newIndex = zPos * sliceSize + xyIndex;

			if (!_condition->IsGrowable(_imageData.GetBitmapData(zPos)[xyIndex], _imageData.GetMaskData(zPos)[xyIndex])) 
			{
				isLineGrowable = false;
			} 
			else 
			{
				// 실제 region growing 수행: 마스크값 세팅
				_imageData.GetMaskData(zPos)[xyIndex] = _condition->GetGrowingValue();

				int index;

				// 현재 픽셀 윗줄
				if(topCheck) 
				{
					for(int i = -1; i <= 1; i++)
					{
						index = newIndex - w + i;
						SowSeed3D(index);
					}
				}

				// 현재 픽셀 아랫줄
				if(bottomCheck)	
				{
					for(int i = -1; i <= 1; i++)
					{
						index = newIndex + w + i;
						SowSeed3D(index);
					}
				}

				// 현재 픽셀 이전 슬라이스
				if(upCheck)	
				{
					// 이전 슬라이스의 오른쪽 픽셀
					index = newIndex - sliceSize + 1;
					SowSeed3D(index);
					// 이전 슬라이스의 왼쪽 픽셀
					index = newIndex - sliceSize - 1;
					SowSeed3D(index);

					// 이전 슬라이스의 윗줄 3 픽셀
					if(topCheck) 
					{
						for(int i = -1; i <= 1; i++)
						{
							index = newIndex - sliceSize - w + i;
							SowSeed3D(index);
						}
					}

					// 이전 슬라이스의 아랫줄 3 픽셀
					if(bottomCheck)	
					{
						for(int i = -1; i <= 1; i++)
						{
							index = newIndex - sliceSize + w + i;
							SowSeed3D(index);
						}
					}
				}

				// 현재 픽셀 다음 슬라이스
				if(downCheck) 
				{
					// 이전 슬라이스의 오른쪽 픽셀
					index = newIndex + sliceSize + 1;
					SowSeed3D(index);
					// 이전 슬라이스의 왼쪽 픽셀
					index = newIndex + sliceSize - 1;
					SowSeed3D(index);

					// 이전 슬라이스의 윗줄 3 픽셀
					if(topCheck) 
					{
						for(int i = -1; i <= 1; i++)
						{
							index = newIndex + sliceSize - w + i;
							SowSeed3D(index);
						}
					}

					// 이전 슬라이스의 아랫줄 3 픽셀
					if(bottomCheck)	
					{
						for(int i = -1; i <= 1; i++)
						{
							index = newIndex + sliceSize + w + i;
							SowSeed3D(index);
						}
					}
				}

			} // else
		} // for(right)

	} while (!_Q.empty());
}

// 3D region growing: 해당 픽셀 주변 6방향을 탐색하면서 확장, 기존의 라인 by 라인 growing x
void RegionGrowingAlgorithm::DoRegionGrowing3DFor6Voxels(RegionGrowingCondition* condition, int seedX, int seedY, int seedZ, int startz, int endz, int startx,int endx, int starty,int endy)
{
	int w = _imageData.GetWidth(), h = _imageData.GetHeight();
	int sliceSize = w * h;
	//int lastSlice = _imageData.GetNumberOfSlices();
	int lastSlice = 0;

	// seed point 범위가 볼륨 범위를 벗어나면 return
	if(seedX < 0 || seedX >= w || seedY < 0 || seedY >= h || seedZ < 0 || seedZ > lastSlice)
	{
		return;
	}

	_condition = condition;

	queue<int> allQ;									// allQ: 전체 seed point를 저장
	queue<int> localQ;									// localQ: 단계별 seed point를 저장
	queue<int> tempQ;									// tempQ: 단계별 seed point를 저장, CCL 체크 목적으로 사용

	int index = seedZ * sliceSize + w * seedY + seedX;

	localQ.push(index);									// 첫번째 seed point 저장 (push(): queue 맨 뒤에 원소를 새로 추가)	
	allQ.push(index);
	tempQ.push(index);

	// 전체 region growing에 대한 반복
	while(!allQ.empty())								
	{
		// 단계별 region growing에 대한 반복
		while(!localQ.empty())							
		{
			index = localQ.front();						// seed point 가져옴 (front(): queue 맨 앞에 위치한 원소를 반환(삭제x))
			localQ.pop();							// seed point 삭제 (pop(): queue 맨 앞에 위치한 원소를 삭제(반환x))

			// index 계산
			int zPos = index / sliceSize,
				yPos = (index % sliceSize) / w,
				xPos = (index % sliceSize) % w;

			int	yIndex = w * yPos;
			int xyIndex = yIndex + xPos;
			int newIndex = zPos * sliceSize + xyIndex;

			// 볼륨의 경계 체크하는 변수: x,y,z
			bool leftCheck = xPos == startx ? false : true;
			bool rightCheck = xPos == endx ? false : true;
			bool topCheck = yPos == starty ? false : true;
			bool bottomCheck = yPos == endy ? false : true;
			bool upCheck = zPos == startz ? false : true;
			bool downCheck = zPos == endz ? false : true;


			// 현재 픽셀을 영역으로 체크
			_imageData.GetMaskData(zPos)[xyIndex] = _condition->GetGrowingValue();

			// 현재 픽셀 주변 6방향에 대해 조건에 맞으면 영역으로 확장 및 seed point 큐에 저장
			if(leftCheck) 
			{
				int	yIndex = w * yPos;
				int xyIndex = yIndex + (xPos - 1);					// x축 왼쪽
				int newIndex = zPos * sliceSize + xyIndex;

				if (_condition->IsGrowable(_imageData.GetBitmapData(zPos)[xyIndex], _imageData.GetMaskData(zPos)[xyIndex]))
				{
					_imageData.GetMaskData(zPos)[xyIndex] = _condition->GetGrowingValue();
					allQ.push(newIndex);
				}
			}

			if(rightCheck)	
			{
				int	yIndex = w * yPos;
				int xyIndex = yIndex + (xPos + 1);					// x축 오른쪽
				int newIndex = zPos * sliceSize + xyIndex;

				if (_condition->IsGrowable(_imageData.GetBitmapData(zPos)[xyIndex], _imageData.GetMaskData(zPos)[xyIndex]))
				{
					_imageData.GetMaskData(zPos)[xyIndex] = _condition->GetGrowingValue();
					allQ.push(newIndex);
				}
			}

			if(topCheck) 
			{
				int	yIndex = w * (yPos - 1);						// y축 위쪽
				int xyIndex = yIndex + xPos;
				int newIndex = zPos * sliceSize + xyIndex;

				if (_condition->IsGrowable(_imageData.GetBitmapData(zPos)[xyIndex], _imageData.GetMaskData(zPos)[xyIndex]))
				{
					_imageData.GetMaskData(zPos)[xyIndex] = _condition->GetGrowingValue();
					allQ.push(newIndex);
				}
			}

			if(bottomCheck)	
			{
				int	yIndex = w * (yPos + 1);						// y축 아래쪽
				int xyIndex = yIndex + xPos;
				int newIndex = zPos * sliceSize + xyIndex;

				if (_condition->IsGrowable(_imageData.GetBitmapData(zPos)[xyIndex], _imageData.GetMaskData(zPos)[xyIndex]))
				{
					_imageData.GetMaskData(zPos)[xyIndex] = _condition->GetGrowingValue();
					allQ.push(newIndex);
				}
			}

			if(upCheck)	
			{
				int	yIndex = w * yPos;
				int xyIndex = yIndex + xPos;
				int newIndex = (zPos - 1) * sliceSize + xyIndex;	// z축 위 슬라이스

				if (_condition->IsGrowable(_imageData.GetBitmapData(zPos - 1)[xyIndex], _imageData.GetMaskData(zPos - 1)[xyIndex]))
				{
					_imageData.GetMaskData(zPos - 1)[xyIndex] = _condition->GetGrowingValue();
					allQ.push(newIndex);
				}
			}

			if(downCheck) 
			{
				int	yIndex = w * yPos;
				int xyIndex = yIndex + xPos;
				int newIndex = (zPos + 1) * sliceSize + xyIndex;	// z축 아래 슬라이스

				if (_condition->IsGrowable(_imageData.GetBitmapData(zPos + 1)[xyIndex], _imageData.GetMaskData(zPos + 1)[xyIndex]))
				{
					_imageData.GetMaskData(zPos + 1)[xyIndex] = _condition->GetGrowingValue();
					allQ.push(newIndex);
				}
			}

		}// while(!localQ.empty())

		// connected component labeling 수행할 메모리 할당
		BYTE* labeling = new BYTE[sliceSize * lastSlice];				 
		memset(labeling, 0, sizeof(BYTE) * sliceSize * lastSlice);
		int NumberOfLabel = 0;	// 레이블 값(숫자) 1부터-오브젝트 개수까지		

		// connected component labeling 수행하여 component 개수 계산
		while(!tempQ.empty())
		{
			int index = tempQ.front();
			tempQ.pop();
			allQ.pop();

			// index 계산
			int zPos = index / sliceSize, yPos = (index % sliceSize) / w, xPos = (index % sliceSize) % w;
		
			int	yIndex = w * yPos;
			int xyIndex = yIndex + xPos;
			int newIndex = zPos * sliceSize + xyIndex;

			// 현재 단계의 region growing 된 영역이면서 레이블링 되지 않은 영역이면 레이블링 수행
			if(_imageData.GetMaskData(zPos)[xyIndex] == 1 && labeling[newIndex] == 0)
			{
				NumberOfLabel++;
				GroupLabeling(labeling, xPos, yPos, zPos, NumberOfLabel);
			}
		}

		// connected component labeling 수행할 메모리 제거
		delete[] labeling;
		
		printf("CCL 개수: %d\n", NumberOfLabel);
		if(NumberOfLabel >= 2)
		{
			FindLabelingRange(labeling);
			break;
		}

		// 현재 단계에서의 CCL이 끝났으면 allQ 개수만큼 localQ로 이동하고 다음 반복 수행
		for(int j = 0; j < allQ.size(); j++)
		{
			int allIndex = allQ.front();

			localQ.push(allIndex);	
			tempQ.push(allIndex);	
			allQ.pop();
			allQ.push(allIndex);
		}
		
		
	}// while(!allQ.empty())
}

void RegionGrowingAlgorithm::FindLabelingRange(BYTE* labeling)
{
	int w = _imageData.GetWidth(), h = _imageData.GetHeight();
	int sliceSize = w * h;
	int z = 1;
	//int lastSlice = _imageData.GetNumberOfSlices();
	int lastSlice = 0;

	int zMin = 100000;
	int zMax = 0;

	for(int zPos = 0; zPos < z; zPos++)
	{			
		for(int yPos = 0; yPos < h; yPos++)
		{
			for(int xPos = 0; xPos < w; xPos++)
			{
				int index = zPos * sliceSize + w * yPos + xPos;

				if(labeling[index] >= 2)
				{
					zMin = MIN(zMin, zPos);
					zMax = MAX(zMax, zPos);
				}
			}
		}
	}

	printf("Slice min - max : %d - %d\n", zMin, zMax);
}

void RegionGrowingAlgorithm::GroupLabeling(BYTE* labeling, int x, int y, int z, int labelnum)
{
	int w = _imageData.GetWidth(), h = _imageData.GetHeight();
	int sliceSize = w * h;
	//int lastSlice = _imageData.GetNumberOfSlices();
	int lastSlice = 0;

	int index;

	// 26-connected 조사
	for(int zPos = z - 1; zPos <= z + 1; zPos++)
	{			
		for(int yPos = y - 1; yPos <= y + 1; yPos++)
		{
			for(int xPos = x - 1; xPos <= x + 1; xPos++)
			{
				//영상의 경계를 벗어나면 라벨링 하지 않음
				if (zPos < 0 || zPos >= lastSlice || yPos < 0 || yPos >= h || xPos < 0 || xPos >= w)
				{
					continue;
				}

				int	yIndex = w * yPos;
				int xyIndex = yIndex + xPos;
				int index = zPos * sliceSize + xyIndex;

				//미방문 픽셀이고, 값이 물체영역(255) 이라면 라벨링함
				if(labeling[index] == 0 && _imageData.GetMaskData(zPos)[xyIndex] == 1)
				{
					labeling[index] = labelnum;
					GroupLabeling(labeling, xPos, yPos, zPos, labelnum);
				}
			}
		}
	}	
}


void RegionGrowingAlgorithm::FillHoleContour(BYTE* pSrc, RegionGrowingCondition* condition)
{
	BYTE* dst = GetDestination();

	int w = _width;
	int h = _height;

	int size = w*h;

	int nIdx;
	int seed = 0;

	for (int i = 0; i < w; i++){
		//top
		nIdx = i;
		DoRegionGrowing(condition, i, 0);

		//bottom
		nIdx =size - i -1;
		DoRegionGrowing(condition, w - i - 1, h - 1);
	}

	for (int j = 0; j < h; j++){
		//left
		nIdx = j * w;
		DoRegionGrowing(condition, 0, j);

		//right
		nIdx = nIdx + w - 1;
		DoRegionGrowing(condition, w - 1, j);

	}

   	for (int k = 0; k <w * h; k++){
   		if (dst[k] != MASKSEGMENTED ){ 
   			pSrc[k] = MASKSEGMENTED;
   		}
   	}

	delete[] dst;
}