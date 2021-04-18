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

// connectivity ������ �ʿ伺�� �� �𸣰ھ (seed point�� ����?) 
// connectivity ���� �����ϰ� �����غ����� override ����: �ߵ�
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

// connectivity ������ �ʿ伺�� �� �𸣰ھ (seed point�� ����?) 
// connectivity ���� �����ϰ� �����غ����� override ����: �ߵ�
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
	_Q.push(index);			// push(): queue �� �ڿ� ���Ҹ� ���� �߰�

	do{
		index = _Q.front();	// front(): queue �� �տ� ��ġ�� ���Ҹ� ��ȯ(�������� �ʴ´�)
		_Q.pop();			// pop(): queue �� �տ� ��ġ�� ���Ҹ� ����(��ȯ���� �ʴ´�)

		int xPos = index % w,
			yPos = index / w;
		int	yIndex = w * yPos;

		bool topConnectivity = false;
		bool bottomConnectivity = false;
		
		// top line ���� bottom line ���� üũ�ϴ� ����
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

				// top line �� �ƴ� ��� �� �ȼ� �˻�
				if(topCheck)
				{
					// above pixel check
					int upperIndex = newIndex - w;
					topConnectivity = SowSeed(upperIndex, topConnectivity);
				}

				// bottom line �� �ƴ� ��� �Ʒ� �ȼ� �˻�
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

// 2D region growing: �ش� �ȼ� �ֺ� 8������ Ž�� ������ ����
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
	_Q.push(index);			// push(): queue �� �ڿ� ���Ҹ� ���� �߰�

	do{
		index = _Q.front();	// front(): queue �� �տ� ��ġ�� ���Ҹ� ��ȯ(�������� �ʴ´�)
		_Q.pop();			// pop(): queue �� �տ� ��ġ�� ���Ҹ� ����(��ȯ���� �ʴ´�)

		int xPos = index % w,
			yPos = index / w;
		int	yIndex = w * yPos;

		// top line ���� bottom line ���� üũ�ϴ� ����
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
				// ���� region growing ����: ����ũ�� ����
				_dst[x + yIndex] = _condition->GetGrowingValue();

				int index;

				// ���� �ȼ� ���� �˻�
				if(topCheck)
				{
					for(int i = -1; i <= 1; i++)
					{
						index = newIndex - w + i;
						SowSeed(index);
					}
				}

				// ���� �ȼ� �Ʒ��� �˻�
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
				// ���� region growing ����: ����ũ�� ����
				_dst[x + yIndex] = _condition->GetGrowingValue();

				int index;

				// ���� �ȼ� ���� �˻�
				if(topCheck)
				{
					for(int i = -1; i <= 1; i++)
					{
						index = newIndex - w + i;
						SowSeed(index);
					}
				}

				// ���� �ȼ� �Ʒ��� �˻�
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

	_Q.push(index);				// push(): queue �� �ڿ� ���Ҹ� ���� �߰�

	do{
		index = _Q.front();		// front(): queue �� �տ� ��ġ�� ���Ҹ� ��ȯ(�������� �ʴ´�)
		_Q.pop();				// pop(): queue �� �տ� ��ġ�� ���Ҹ� ����(��ȯ���� �ʴ´�)

		int zPos = index / sliceSize,
			yPos = (index % sliceSize) / w,
			xPos = (index % sliceSize) % w;
		int	yIndex = w * yPos;
		
		// ���� �����̽����� �ٷ� �� �ȼ��� �Ʒ� �ȼ��� ���� ���Ἲ�� üũ�ϱ� ���� ����
		// ���� �ȼ����� �� �ȼ��̳� �Ʒ� �ȼ��� growing �Ǿ�� �� �ȼ����� �ƴ����� üũ�ϱ� ���� ����
		bool topConnectivity = false;
		bool bottomConnectivity = false;

		// ���� �����̽����� z ������ �ٷ� �� �ȼ� �Ǵ� �Ʒ� �ȼ��� growing �Ǿ�� �� �ȼ����� �ƴ����� üũ�ϱ� ���� ����
		bool upConnectivity = false;
		bool downConnectivity = false;

		// �� �� �����̳� �� �Ʒ� �����̳� �� �� �����̽��� �� �Ʒ� �����̽����� üũ�ϴ� ����
		// �׷� ��, �Ʒ��� üũ�� �ʿ䰡 �����ϱ�
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
				// ���� region growing ����: ����ũ�� ����
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
				// ���� region growing ����: ����ũ�� ����
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

// 3D region growing: �ش� �ȼ� �ֺ� 26������ Ž�� ������ ����
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

	_Q.push(index);				// push(): queue �� �ڿ� ���Ҹ� ���� �߰�

	do{
		index = _Q.front();		// front(): queue �� �տ� ��ġ�� ���Ҹ� ��ȯ(�������� �ʴ´�)
		_Q.pop();				// pop(): queue �� �տ� ��ġ�� ���Ҹ� ����(��ȯ���� �ʴ´�)

		int zPos = index / sliceSize,
			yPos = (index % sliceSize) / w,
			xPos = (index % sliceSize) % w;
		int	yIndex = w * yPos;


		// �� �� �����̳� �� �Ʒ� �����̳� �� �� �����̽��� �� �Ʒ� �����̽����� üũ�ϴ� ����
		// �׷� ��, �Ʒ��� üũ�� �ʿ䰡 �����ϱ�
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
				// ���� region growing ����: ����ũ�� ����
				_imageData.GetMaskData(zPos)[xyIndex] = _condition->GetGrowingValue();

				int index;

				// ���� �ȼ� ����
				if(topCheck) 
				{
					for(int i = -1; i <= 1; i++)
					{
						index = newIndex - w + i;
						SowSeed3D(index);
					}
				}
	
				// ���� �ȼ� �Ʒ���
				if(bottomCheck)	
				{
					for(int i = -1; i <= 1; i++)
					{
						index = newIndex + w + i;
						SowSeed3D(index);
					}
				}
	
				// ���� �ȼ� ���� �����̽�
				if(upCheck)	
				{
					// ���� �����̽��� ������ �ȼ�
					index = newIndex - sliceSize + 1;
					SowSeed3D(index);
					// ���� �����̽��� ���� �ȼ�
					index = newIndex - sliceSize - 1;
					SowSeed3D(index);
					
					// ���� �����̽��� ���� 3 �ȼ�
					if(topCheck) 
					{
						for(int i = -1; i <= 1; i++)
						{
							index = newIndex - sliceSize - w + i;
							SowSeed3D(index);
						}
					}

					// ���� �����̽��� �Ʒ��� 3 �ȼ�
					if(bottomCheck)	
					{
						for(int i = -1; i <= 1; i++)
						{
							index = newIndex - sliceSize + w + i;
							SowSeed3D(index);
						}
					}
				}
				
				// ���� �ȼ� ���� �����̽�
				if(downCheck) 
				{
					// ���� �����̽��� ������ �ȼ�
					index = newIndex + sliceSize + 1;
					SowSeed3D(index);
					// ���� �����̽��� ���� �ȼ�
					index = newIndex + sliceSize - 1;
					SowSeed3D(index);

					// ���� �����̽��� ���� 3 �ȼ�
					if(topCheck) 
					{
						for(int i = -1; i <= 1; i++)
						{
							index = newIndex + sliceSize - w + i;
							SowSeed3D(index);
						}
					}

					// ���� �����̽��� �Ʒ��� 3 �ȼ�
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
				// ���� region growing ����: ����ũ�� ����
				_imageData.GetMaskData(zPos)[xyIndex] = _condition->GetGrowingValue();

				int index;

				// ���� �ȼ� ����
				if(topCheck) 
				{
					for(int i = -1; i <= 1; i++)
					{
						index = newIndex - w + i;
						SowSeed3D(index);
					}
				}

				// ���� �ȼ� �Ʒ���
				if(bottomCheck)	
				{
					for(int i = -1; i <= 1; i++)
					{
						index = newIndex + w + i;
						SowSeed3D(index);
					}
				}

				// ���� �ȼ� ���� �����̽�
				if(upCheck)	
				{
					// ���� �����̽��� ������ �ȼ�
					index = newIndex - sliceSize + 1;
					SowSeed3D(index);
					// ���� �����̽��� ���� �ȼ�
					index = newIndex - sliceSize - 1;
					SowSeed3D(index);

					// ���� �����̽��� ���� 3 �ȼ�
					if(topCheck) 
					{
						for(int i = -1; i <= 1; i++)
						{
							index = newIndex - sliceSize - w + i;
							SowSeed3D(index);
						}
					}

					// ���� �����̽��� �Ʒ��� 3 �ȼ�
					if(bottomCheck)	
					{
						for(int i = -1; i <= 1; i++)
						{
							index = newIndex - sliceSize + w + i;
							SowSeed3D(index);
						}
					}
				}

				// ���� �ȼ� ���� �����̽�
				if(downCheck) 
				{
					// ���� �����̽��� ������ �ȼ�
					index = newIndex + sliceSize + 1;
					SowSeed3D(index);
					// ���� �����̽��� ���� �ȼ�
					index = newIndex + sliceSize - 1;
					SowSeed3D(index);

					// ���� �����̽��� ���� 3 �ȼ�
					if(topCheck) 
					{
						for(int i = -1; i <= 1; i++)
						{
							index = newIndex + sliceSize - w + i;
							SowSeed3D(index);
						}
					}

					// ���� �����̽��� �Ʒ��� 3 �ȼ�
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

// 3D region growing: �ش� �ȼ� �ֺ� 6������ Ž���ϸ鼭 Ȯ��, ������ ���� by ���� growing x
void RegionGrowingAlgorithm::DoRegionGrowing3DFor6Voxels(RegionGrowingCondition* condition, int seedX, int seedY, int seedZ, int startz, int endz, int startx,int endx, int starty,int endy)
{
	int w = _imageData.GetWidth(), h = _imageData.GetHeight();
	int sliceSize = w * h;
	//int lastSlice = _imageData.GetNumberOfSlices();
	int lastSlice = 0;

	// seed point ������ ���� ������ ����� return
	if(seedX < 0 || seedX >= w || seedY < 0 || seedY >= h || seedZ < 0 || seedZ > lastSlice)
	{
		return;
	}

	_condition = condition;

	queue<int> allQ;									// allQ: ��ü seed point�� ����
	queue<int> localQ;									// localQ: �ܰ躰 seed point�� ����
	queue<int> tempQ;									// tempQ: �ܰ躰 seed point�� ����, CCL üũ �������� ���

	int index = seedZ * sliceSize + w * seedY + seedX;

	localQ.push(index);									// ù��° seed point ���� (push(): queue �� �ڿ� ���Ҹ� ���� �߰�)	
	allQ.push(index);
	tempQ.push(index);

	// ��ü region growing�� ���� �ݺ�
	while(!allQ.empty())								
	{
		// �ܰ躰 region growing�� ���� �ݺ�
		while(!localQ.empty())							
		{
			index = localQ.front();						// seed point ������ (front(): queue �� �տ� ��ġ�� ���Ҹ� ��ȯ(����x))
			localQ.pop();							// seed point ���� (pop(): queue �� �տ� ��ġ�� ���Ҹ� ����(��ȯx))

			// index ���
			int zPos = index / sliceSize,
				yPos = (index % sliceSize) / w,
				xPos = (index % sliceSize) % w;

			int	yIndex = w * yPos;
			int xyIndex = yIndex + xPos;
			int newIndex = zPos * sliceSize + xyIndex;

			// ������ ��� üũ�ϴ� ����: x,y,z
			bool leftCheck = xPos == startx ? false : true;
			bool rightCheck = xPos == endx ? false : true;
			bool topCheck = yPos == starty ? false : true;
			bool bottomCheck = yPos == endy ? false : true;
			bool upCheck = zPos == startz ? false : true;
			bool downCheck = zPos == endz ? false : true;


			// ���� �ȼ��� �������� üũ
			_imageData.GetMaskData(zPos)[xyIndex] = _condition->GetGrowingValue();

			// ���� �ȼ� �ֺ� 6���⿡ ���� ���ǿ� ������ �������� Ȯ�� �� seed point ť�� ����
			if(leftCheck) 
			{
				int	yIndex = w * yPos;
				int xyIndex = yIndex + (xPos - 1);					// x�� ����
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
				int xyIndex = yIndex + (xPos + 1);					// x�� ������
				int newIndex = zPos * sliceSize + xyIndex;

				if (_condition->IsGrowable(_imageData.GetBitmapData(zPos)[xyIndex], _imageData.GetMaskData(zPos)[xyIndex]))
				{
					_imageData.GetMaskData(zPos)[xyIndex] = _condition->GetGrowingValue();
					allQ.push(newIndex);
				}
			}

			if(topCheck) 
			{
				int	yIndex = w * (yPos - 1);						// y�� ����
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
				int	yIndex = w * (yPos + 1);						// y�� �Ʒ���
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
				int newIndex = (zPos - 1) * sliceSize + xyIndex;	// z�� �� �����̽�

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
				int newIndex = (zPos + 1) * sliceSize + xyIndex;	// z�� �Ʒ� �����̽�

				if (_condition->IsGrowable(_imageData.GetBitmapData(zPos + 1)[xyIndex], _imageData.GetMaskData(zPos + 1)[xyIndex]))
				{
					_imageData.GetMaskData(zPos + 1)[xyIndex] = _condition->GetGrowingValue();
					allQ.push(newIndex);
				}
			}

		}// while(!localQ.empty())

		// connected component labeling ������ �޸� �Ҵ�
		BYTE* labeling = new BYTE[sliceSize * lastSlice];				 
		memset(labeling, 0, sizeof(BYTE) * sliceSize * lastSlice);
		int NumberOfLabel = 0;	// ���̺� ��(����) 1����-������Ʈ ��������		

		// connected component labeling �����Ͽ� component ���� ���
		while(!tempQ.empty())
		{
			int index = tempQ.front();
			tempQ.pop();
			allQ.pop();

			// index ���
			int zPos = index / sliceSize, yPos = (index % sliceSize) / w, xPos = (index % sliceSize) % w;
		
			int	yIndex = w * yPos;
			int xyIndex = yIndex + xPos;
			int newIndex = zPos * sliceSize + xyIndex;

			// ���� �ܰ��� region growing �� �����̸鼭 ���̺� ���� ���� �����̸� ���̺� ����
			if(_imageData.GetMaskData(zPos)[xyIndex] == 1 && labeling[newIndex] == 0)
			{
				NumberOfLabel++;
				GroupLabeling(labeling, xPos, yPos, zPos, NumberOfLabel);
			}
		}

		// connected component labeling ������ �޸� ����
		delete[] labeling;
		
		printf("CCL ����: %d\n", NumberOfLabel);
		if(NumberOfLabel >= 2)
		{
			FindLabelingRange(labeling);
			break;
		}

		// ���� �ܰ迡���� CCL�� �������� allQ ������ŭ localQ�� �̵��ϰ� ���� �ݺ� ����
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

	// 26-connected ����
	for(int zPos = z - 1; zPos <= z + 1; zPos++)
	{			
		for(int yPos = y - 1; yPos <= y + 1; yPos++)
		{
			for(int xPos = x - 1; xPos <= x + 1; xPos++)
			{
				//������ ��踦 ����� �󺧸� ���� ����
				if (zPos < 0 || zPos >= lastSlice || yPos < 0 || yPos >= h || xPos < 0 || xPos >= w)
				{
					continue;
				}

				int	yIndex = w * yPos;
				int xyIndex = yIndex + xPos;
				int index = zPos * sliceSize + xyIndex;

				//�̹湮 �ȼ��̰�, ���� ��ü����(255) �̶�� �󺧸���
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