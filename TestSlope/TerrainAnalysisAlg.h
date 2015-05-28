/*******************************************************************************
* 版权所有(C)  2013
* 文件名称	: TerrainAnalysisAlg.h
* 当前版本	: 1.0.0.1
* 作    者	: 周旭光 (zhouxuguang236@126.com)
* 设计日期	: 2013年12月24日
* 内容摘要	: DEM地形分析算法
* 修改记录	: 
* 日    期		版    本		修改人		修改摘要

********************************************************************************/
#ifndef __TERRAINANALYSISALG_H_BB9A14F5_356B_4B59_8BED_03D993EA72FC__
#define __TERRAINANALYSISALG_H_BB9A14F5_356B_4B59_8BED_03D993EA72FC__

#include "gdal_alg.h"

/**********************************  头文件 ************************************/

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

#ifdef  __cplusplus
extern "C" {
#endif

	
	/*******************************************************************************
	@ 函数名称	: GridToContour
	@ 功能描述	: 由DEM生成等高线
	@ 参　　数	: const char* pszRasFile	输入DEM文件
	@ 参　　数	: const char* pszContourFile	输出文件
	@ 参　　数	: const char* pszFormat		输出格式
	@ 参　　数	: double  dfContourInterval	等高线间隔
	@ 参　　数	: double  dfContourBase		起始等高线高度
	@ 参　　数	: double nFixedLevelCount	等高线固定的条数
	@ 参　　数	: double *padfFixedLevels	每条等高线的高度数组
	@ 返 回 值	: bool 成功返回ture，否则false
	@ 作　　者	: guang
	@ 设计日期	: 2013年12月24日
	@ 修改日期		   修改人		  修改内容
	*******************************************************************************/
	bool GridToContour(
		const char* pszRasFile,
		const char* pszContourFile,
		const char* pszFormat,
		double  dfContourInterval,  
		double  dfContourBase,
		double nFixedLevelCount,
		double *padfFixedLevels,
		GDALProgressFunc pfnProgress = NULL, 
		void *pProgressArg = NULL);

	bool ExtractSlope(const char* pszDEMfile,const char* pszOutSlpoeFile,const char* pszFormat,
		SLOPE_TYPE eSlopeType,double dbScale);

	/*******************************************************************************
	@ 函数名称	: CreateGeoGrid
	@ 功能描述	: 离散点生成网格
	@ 参　　数	: GDALGridAlgorithm  eAlgorithm
	@ 参　　数	: GUInt32  nPoints
	@ 参　　数	: const double *  padfX
	@ 参　　数	: const double *  padfY
	@ 参　　数	: const double *  padfZ
	@ 参　　数	: double  dfXMin
	@ 参　　数	: double  dfXMax
	@ 参　　数	: double  dfYMin
	@ 参　　数	: double  dfYMax
	@ 参　　数	: GUInt32  nXSize
	@ 参　　数	: GUInt32  nYSize
	@ 参　　数	: GDALDataType  eType
	@ 参　　数	: const char* pszOutFile
	@ 返 回 值	: bool
	@ 作　　者	: guang
	@ 设计日期	: 2014年1月1日
	@ 修改日期		   修改人		  修改内容
	*******************************************************************************/
	bool CreateGeoGrid(GDALGridAlgorithm  eAlgorithm,    
		GUInt32  nPoints,  
		const double *  padfX,  
		const double *  padfY,  
		const double *  padfZ,  
		double  dfXMin,  
		double  dfXMax,  
		double  dfYMin,  
		double  dfYMax,  
		GUInt32  nXSize,  
		GUInt32  nYSize,  
		GDALDataType  eType,  
		const char* pszOutFile   );


#ifdef  __cplusplus
}
#endif

#endif // end of __TERRAINANALYSISALG_H_BB9A14F5_356B_4B59_8BED_03D993EA72FC__

