#include <stdio.h>
#include "TerrainAnalysisAlg.h"

#include <Windows.h>

void main()
{
	MEMORYSTATUS mem;//����һ���ڴ�״̬���� 

	DWORD MemTotal,MemFree,VMemTotal,VMemFree;//�洢�ڴ�״̬��Ϣ 

	GlobalMemoryStatus(&mem); //�洢�ڴ�״̬��Ϣ 
	MemTotal=mem.dwTotalPhys;//�õ����ڴ��С����λb�� 
	MemFree=mem.dwAvailPhys;//�õ�ʣ���ڴ��С����λb�� 
	VMemTotal=mem.dwTotalVirtual;//�õ��������ڴ��С����λb�� 
	VMemFree=mem.dwAvailVirtual;//�õ�ʣ�������ڴ��ڴ��С����λb�� 

	const char* pszInFile = "E:\\my-space\\����\\DEM\\srtm-90\\srtm_55_03.tif";

	//const char* pszInFile = "E:\\my-space\\����\\DEM\\srtm-90\\ƴ��-DEM\\ƴ��3-4.tif";
	

#ifdef OPENCL_MODE
	const char* pszOutFile = "E:\\my-space\\����\\DEM\\srtm-90\\ƴ��-DEM\\ƴ��3-4_slope_opencl.tif";
#else
	const char* pszOutFile = "E:\\my-space\\����\\DEM\\srtm-90\\ƴ��-DEM\\ƴ��3-4_slope.tif";
#endif
	//GridToContour(pszInFile,"srtm_55_07_�ȸ���.shp","ESRI Shapefile",100,800,0,NULL,BufferAnalysisProgress);
	ExtractSlope(pszInFile,pszOutFile,"GTiff",DEGREE_SLOPE,1.0);
}