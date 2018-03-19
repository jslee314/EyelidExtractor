#pragma once

//#include "queue"
//#include "ArcheImageData.h"
#include "MyImage.h"
#include "MyImageFunc.h"
#include <queue>

class RegionGrowingCondition
{
public:
	virtual bool IsGrowable(BYTE srcByte, BYTE dstByte)
	{

		return (srcByte == BITMAPIMAGE && dstByte != MASKSEGMENTED && dstByte != MASKFENCE && dstByte != MASKBONE);
	};

	virtual BYTE GetGrowingValue()
	{
		return MASKSEGMENTED;
	}
};

class ReverseRegionGrowingCondition : public RegionGrowingCondition
{
public:
	virtual bool IsGrowable(BYTE srcByte, BYTE dstByte)
	{
		return (srcByte == MASKSEGMENTED && dstByte != MASKFENCE && dstByte != MASKNONE);
	};

	virtual BYTE GetGrowingValue()
	{
		return MASKNONE;
	}
};

class FenceCondition : public RegionGrowingCondition
{
public:
	virtual bool IsGrowable(BYTE srcByte, BYTE dstByte)
	{
		return (srcByte == BITMAPIMAGE && dstByte != MASKSEGMENTED && dstByte != MASKFENCE && dstByte != MASKBONE);
	};

	virtual BYTE GetGrowingValue()
	{
		return MASKSEGMENTED;
	}
};

class FillholeRegionGrowingCondition : public RegionGrowingCondition
{
public:
	virtual bool IsGrowable(BYTE srcByte, BYTE dstByte)
	{
		return (srcByte != MASKSEGMENTED && dstByte != MASKSEGMENTED);
	};

	virtual BYTE GetGrowingValue()
	{
		return MASKSEGMENTED;
	}
};

class RegionGrowingAlgorithm
{
private:
	BYTE* _src;
	BYTE* _dst;

	int _width;
	int _height;

	RegionGrowingCondition* _condition;
	//ArcheImageData* _imageData;
	CByteImage _imageData;
	
//	ArcheImageWrapper* _imageWarpper;//sue

	queue<int> _Q;

public:
	RegionGrowingAlgorithm(void);
	~RegionGrowingAlgorithm(void);

	BYTE* GetSource();
	BYTE* GetDestination();

	void SetSource(BYTE*, int w, int h);
	void SetDestination(BYTE*, int w, int h);
	void SetImageData(CByteImage& imageData);

	// seeded region growing
	void DoRegionGrowing(RegionGrowingCondition* condition, int x, int y);
	void DoRegionGrowing3D(RegionGrowingCondition* condition, int x, int y, int z,int startz,int endz,int startx,int endx,int starty,int endy);

	bool SowSeed(int index, bool connectivity);
	bool SowSeed3D(int index, bool connectivity);

	void FillHoleContour(BYTE*, RegionGrowingCondition* condition);

	// jhyu
	void DoRegionGrowingFor8Pixels(RegionGrowingCondition* condition, int seedX, int seedY );
	void DoRegionGrowing3DFor26Voxels(RegionGrowingCondition* condition, int x, int y, int z,int startz,int endz,int startx,int endx,int starty,int endy);
	void DoRegionGrowing3DFor6Voxels(RegionGrowingCondition* condition, int seedX, int seedY, int seedZ, int startz, int endz,
		int startx,int endx, int starty,int endy);
	
	void SowSeed(int index);
	void SowSeed3D(int index);

	void GroupLabeling(BYTE* labeling, int x, int y, int z, int labelnum);
	void FindLabelingRange(BYTE* labeling);

};


