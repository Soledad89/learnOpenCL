

#pragma OPENCL EXTENSION cl_khr_fp64: enable


//坡度的表达方式
typedef enum 
{
	DEGREE_SLOPE,		//度数方式
	PERCENT_SLOPE		//百分比方式
}SLOPE_TYPE;

//坡度算法结构体
struct SlopeOption
{
	double dbNsres;				//南北方向分辨率 
	double dbEwres;				//东西方向分辨率 
	SLOPE_TYPE slopeType;		//坡度方式
} ;

__kernel void slope_kernel( __global const float *pSrcData,
						 __global float *pDestData,const int nWidth,const int nHeight
						 , struct SlopeOption slopeType)
{
	int j = (int)get_global_id(0);
	int i = (int)get_global_id(1);

	if (j >= nWidth || i >= nHeight)
		return;


	int nTopTmp = i-1;
	int nBottomTmp = i+1;
	int nLeftTep = j-1;
	int nRightTmp = j+1;

	//处理边界情况
	if (0 == i)
	{
		nTopTmp = i;
	}
	if (0 == j)
	{
		nLeftTep = j;
	}
	if (i == nHeight-1)
	{
		nBottomTmp = i;
	}
	if (j == nWidth-1)
	{
		nRightTmp = j;
	}
	float dbRectData[9];
	dbRectData[0] = pSrcData[nTopTmp*nWidth+nLeftTep];
	dbRectData[1] = pSrcData[nTopTmp*nWidth+j];
	dbRectData[2] = pSrcData[nTopTmp*nWidth+nRightTmp];

	dbRectData[3] = pSrcData[i*nWidth+nLeftTep];
	dbRectData[4] = pSrcData[i*nWidth+j];
	dbRectData[5] = pSrcData[i*nWidth+nRightTmp];

	dbRectData[6] = pSrcData[nBottomTmp*nWidth+nLeftTep];
	dbRectData[7] = pSrcData[nBottomTmp*nWidth+j];
	dbRectData[8] = pSrcData[nBottomTmp*nWidth+nRightTmp];

	double dx = ((dbRectData[0] + dbRectData[3]*2 + dbRectData[6]) -
		(dbRectData[2]+ dbRectData[5]*2 + dbRectData[8])) / (slopeType.dbEwres*8);

	double dy =((dbRectData[6] + dbRectData[7]*2 + dbRectData[8]) -
		(dbRectData[0]+ dbRectData[1]*2 + dbRectData[2])) / (slopeType.dbNsres*8);

	double fTmp = (dx *dx + dy * dy);

	//计算坡度
	double radiansToDegrees = 180.0/M_PI;
	double fValue = 0;
	if(slopeType.slopeType == DEGREE_SLOPE)
	{
		fValue = atan(sqrt(fTmp) ) * radiansToDegrees;
	}
	else if (slopeType.slopeType == PERCENT_SLOPE)
		fValue = 100*sqrt(fTmp);

	pDestData[i*nWidth+j] = fValue; 

}