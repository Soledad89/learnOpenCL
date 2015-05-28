#include <stdio.h>
#include "TerrainAnalysisAlg.h"

#include <Windows.h>

void main()
{
	MEMORYSTATUS mem;//定义一个内存状态变量 

	DWORD MemTotal,MemFree,VMemTotal,VMemFree;//存储内存状态信息 

	GlobalMemoryStatus(&mem); //存储内存状态信息 
	MemTotal=mem.dwTotalPhys;//得到总内存大小（单位b） 
	MemFree=mem.dwAvailPhys;//得到剩余内存大小（单位b） 
	VMemTotal=mem.dwTotalVirtual;//得到总虚拟内存大小（单位b） 
	VMemFree=mem.dwAvailVirtual;//得到剩余虚拟内存内存大小（单位b） 

	const char* pszInFile = "E:\\my-space\\数据\\DEM\\srtm-90\\srtm_55_03.tif";

	//const char* pszInFile = "E:\\my-space\\数据\\DEM\\srtm-90\\拼接-DEM\\拼接3-4.tif";
	

#ifdef OPENCL_MODE
	const char* pszOutFile = "E:\\my-space\\数据\\DEM\\srtm-90\\拼接-DEM\\拼接3-4_slope_opencl.tif";
#else
	const char* pszOutFile = "E:\\my-space\\数据\\DEM\\srtm-90\\拼接-DEM\\拼接3-4_slope.tif";
#endif
	//GridToContour(pszInFile,"srtm_55_07_等高线.shp","ESRI Shapefile",100,800,0,NULL,BufferAnalysisProgress);
	ExtractSlope(pszInFile,pszOutFile,"GTiff",DEGREE_SLOPE,1.0);
}