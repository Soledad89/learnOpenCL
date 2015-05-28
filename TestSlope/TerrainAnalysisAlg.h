/*******************************************************************************
* ��Ȩ����(C)  2013
* �ļ�����	: TerrainAnalysisAlg.h
* ��ǰ�汾	: 1.0.0.1
* ��    ��	: ����� (zhouxuguang236@126.com)
* �������	: 2013��12��24��
* ����ժҪ	: DEM���η����㷨
* �޸ļ�¼	: 
* ��    ��		��    ��		�޸���		�޸�ժҪ

********************************************************************************/
#ifndef __TERRAINANALYSISALG_H_BB9A14F5_356B_4B59_8BED_03D993EA72FC__
#define __TERRAINANALYSISALG_H_BB9A14F5_356B_4B59_8BED_03D993EA72FC__

#include "gdal_alg.h"

/**********************************  ͷ�ļ� ************************************/

//�¶ȵı�﷽ʽ
typedef enum 
{
	DEGREE_SLOPE,		//������ʽ
	PERCENT_SLOPE		//�ٷֱȷ�ʽ
}SLOPE_TYPE;

//�¶��㷨�ṹ��
struct SlopeOption
{
	double dbNsres;				//�ϱ�����ֱ��� 
	double dbEwres;				//��������ֱ��� 
	SLOPE_TYPE slopeType;		//�¶ȷ�ʽ 
} ;

#ifdef  __cplusplus
extern "C" {
#endif

	
	/*******************************************************************************
	@ ��������	: GridToContour
	@ ��������	: ��DEM���ɵȸ���
	@ �Ρ�����	: const char* pszRasFile	����DEM�ļ�
	@ �Ρ�����	: const char* pszContourFile	����ļ�
	@ �Ρ�����	: const char* pszFormat		�����ʽ
	@ �Ρ�����	: double  dfContourInterval	�ȸ��߼��
	@ �Ρ�����	: double  dfContourBase		��ʼ�ȸ��߸߶�
	@ �Ρ�����	: double nFixedLevelCount	�ȸ��߹̶�������
	@ �Ρ�����	: double *padfFixedLevels	ÿ���ȸ��ߵĸ߶�����
	@ �� �� ֵ	: bool �ɹ�����ture������false
	@ ��������	: guang
	@ �������	: 2013��12��24��
	@ �޸�����		   �޸���		  �޸�����
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
	@ ��������	: CreateGeoGrid
	@ ��������	: ��ɢ����������
	@ �Ρ�����	: GDALGridAlgorithm  eAlgorithm
	@ �Ρ�����	: GUInt32  nPoints
	@ �Ρ�����	: const double *  padfX
	@ �Ρ�����	: const double *  padfY
	@ �Ρ�����	: const double *  padfZ
	@ �Ρ�����	: double  dfXMin
	@ �Ρ�����	: double  dfXMax
	@ �Ρ�����	: double  dfYMin
	@ �Ρ�����	: double  dfYMax
	@ �Ρ�����	: GUInt32  nXSize
	@ �Ρ�����	: GUInt32  nYSize
	@ �Ρ�����	: GDALDataType  eType
	@ �Ρ�����	: const char* pszOutFile
	@ �� �� ֵ	: bool
	@ ��������	: guang
	@ �������	: 2014��1��1��
	@ �޸�����		   �޸���		  �޸�����
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

