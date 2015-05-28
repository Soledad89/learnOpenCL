
#include <CL/cl.h>

#include "OpenCL.h"

#include <gdal_priv.h>
#include <gdal_alg.h>
#include <ogr_api.h>
#include <ogrsf_frmts.h>
#include "TerrainAnalysisAlg.h"

bool GridToContour(const char* pszRasFile,
				   const char* pszContourFile,
				   const char* pszFormat,
				   double  dfContourInterval,  
				   double  dfContourBase ,
				   double nFixedLevelCount,
				   double *padfFixedLevels,
				   GDALProgressFunc pfnProgress, 
				   void *pProgressArg)
{
	GDALAllRegister();
	OGRRegisterAll();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8","NO");
	GDALRasterBand *poBand = NULL;

	GDALDataset* poSrcDS = NULL;
	poSrcDS = (GDALDataset*)GDALOpen( pszRasFile, GA_ReadOnly );
	if( poSrcDS == NULL )
	{
		return false;
	}

	int nBandCount = poSrcDS->GetRasterCount();
	if (nBandCount > 1)
	{
		return false;
	}
	

	poBand = poSrcDS->GetRasterBand(1);
	if( poBand == NULL )
	{
		return false;
	}

	//获取无效值
	int bNoDataSet = -1;
	double dfNoData = poBand->GetNoDataValue(&bNoDataSet);

	/* -------------------------------------------------------------------- */
	/*      获得坐标系统                                                    */
	/* -------------------------------------------------------------------- */
	OGRSpatialReferenceH hSRS = NULL;

	const char *pszWKT = GDALGetProjectionRef((GDALDataset*)poSrcDS);

	if( pszWKT != NULL && strlen(pszWKT) != 0 )
	{
		hSRS = OSRNewSpatialReference( pszWKT );
	}

	/* -------------------------------------------------------------------- */
	/*     创建输出文件  .                                                  */
	/* -------------------------------------------------------------------- */
	OGRDataSourceH hDS;
	OGRSFDriverH hDriver = OGRGetDriverByName( pszFormat );
	OGRFieldDefnH hFld;
	OGRLayerH hLayer;

	if( hDriver == NULL )
	{
		return false;
	}

	hDS = OGR_Dr_CreateDataSource( hDriver, pszContourFile, NULL );
	if( hDS == NULL )
		return false;

	hLayer = OGR_DS_CreateLayer( hDS, pszContourFile, hSRS, wkbLineString,NULL );
	if( hLayer == NULL )
		return false;

	hFld = OGR_Fld_Create( "ID", OFTInteger );
	OGR_Fld_SetWidth( hFld, 8 );
	OGR_L_CreateField( hLayer, hFld, FALSE );
	OGR_Fld_Destroy( hFld );

	if( 1 )
	{
		hFld = OGR_Fld_Create( "高程", OFTReal );
		OGR_Fld_SetWidth( hFld, 12 );
		OGR_Fld_SetPrecision( hFld, 3 );
		OGR_L_CreateField( hLayer, hFld, FALSE );
		OGR_Fld_Destroy( hFld );
	}

	/* -------------------------------------------------------------------- */
	/*      调用  .                                                         */
	/* -------------------------------------------------------------------- */
	CPLErr eErr;

	eErr = GDALContourGenerate( poBand, dfContourInterval, dfContourBase, 
		nFixedLevelCount, padfFixedLevels,
		bNoDataSet, dfNoData, hLayer, 
		OGR_FD_GetFieldIndex( OGR_L_GetLayerDefn( hLayer ), "ID" ), 
		OGR_FD_GetFieldIndex( OGR_L_GetLayerDefn( hLayer ), "高程" ), 
		pfnProgress, pProgressArg );

	OGR_DS_Destroy( hDS );
	GDALClose( poSrcDS );

	if (hSRS != NULL)
		OSRDestroySpatialReference( hSRS );

	GDALDestroyDriverManager();
	OGRCleanupAll();

	return true;
}

extern float SlopeCal (float* afRectData, float fDstNoDataValue,void* pData);

extern void SlopeCal_OpenCL(float* poDataIn,float *poDataOut,int nWidth,int nHeight,const SlopeOption* pSlopeType);


bool ExtractSlope(const char* pszDEMfile,const char* pszOutSlpoeFile,const char* pszFormat, SLOPE_TYPE eSlopeType
				  ,double dbScale)
{
	GDALAllRegister();
	OGRRegisterAll();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8","NO");
	GDALRasterBand *poBand = NULL;

	GDALDataset* poSrcDS = NULL;
	poSrcDS = (GDALDataset*)GDALOpen( pszDEMfile, GA_ReadOnly );
	if( poSrcDS == NULL )
	{
		return false;
	}

	int nBandCount = poSrcDS->GetRasterCount();
	if (nBandCount > 1)	//多波段数据不做
	{
		return false;
	}


	poBand = poSrcDS->GetRasterBand(1);
	if( poBand == NULL )
	{
		return false;
	}

	//投影
	const char* pszWkt = poSrcDS->GetProjectionRef();

	double dbNres = 0;
	double dbEres = 0;

	double dfGeotrans[6];
	if (poSrcDS->GetGeoTransform(dfGeotrans) != CE_None)
	{
		dbNres = fabs(dfGeotrans[5]);
		dbEres = fabs(dfGeotrans[1]);
	}

	else
	{
		OGRSpatialReference* pSrs = (OGRSpatialReference*)OSRNewSpatialReference(pszWkt);
		if (pSrs != NULL)
		{
			if (pSrs->IsGeographic())
			{
				dbNres = fabs(dfGeotrans[5]);
				dbNres *= 110000;
				dbEres = fabs(dfGeotrans[1]);
				dbEres *= 110000;
			}

			else if (pSrs->IsGeocentric())
			{
				dbNres = fabs(dfGeotrans[5]);
				dbEres = fabs(dfGeotrans[1]);
			}

			else if (pSrs->IsProjected())
			{
				dbNres = fabs(dfGeotrans[5]);
				dbEres = fabs(dfGeotrans[1]);
			}
			
		}

		OSRDestroySpatialReference((OGRSpatialReferenceH)pSrs);
	}

	SlopeOption slopeOption;
	slopeOption.dbEwres = dbEres;
	slopeOption.dbNsres = dbNres;
	slopeOption.slopeType = eSlopeType;
	

	int nXsize = poBand->GetXSize();
	int nYsize = poBand->GetYSize();

	//创建新文件输出数据
	GDALDriver *poDriver = NULL;
	GDALDataset* poDstDS = NULL;
	poDriver = GetGDALDriverManager()->GetDriverByName(pszFormat);
	if (NULL != poDriver)
	{
		poDstDS = poDriver->Create(pszOutSlpoeFile,nXsize,nYsize,1,GDT_Float32,NULL);
		poDstDS->SetProjection(pszWkt);
		poDstDS->SetGeoTransform(dfGeotrans);
	}

	clock_t t1 = clock();

	//分块处理

	int nSubHeight = 2000;
	int nYBlockCount = (nYsize+nSubHeight-1)/nSubHeight;	//计算分块的数量
	for (int i = 0; i < nYBlockCount; i ++)
	{
		//实际的块的大小
		int nRealWidth = nXsize;
		int nRealHeight = nSubHeight;
		//读取数据块的大小
		int nReadWidth = nXsize;
		int nReadHeight = nSubHeight;
		int nYOffset = i*nSubHeight;
		if (1 == nYBlockCount)
		{
			nRealHeight = nYsize;
			nReadHeight = nRealHeight;
		}
		else
		{
			if (i == 0)
			{
				nReadHeight += 1;
			}
			else if (i > 0 && i < nYBlockCount-1)
			{
				nReadHeight += 2;
				nYOffset -= 1;
			}
			else if(i == nYBlockCount-1)
			{
				nRealHeight = nYsize-nSubHeight*(nYBlockCount-1);
				nReadHeight = nRealHeight + 1;
				nYOffset -= 1;
			}
		}

		//读取数据
		float* poData = new float[nReadWidth*nReadHeight];
		float* poOutData = new float[nReadWidth*nReadHeight];
		poBand->RasterIO(GF_Read,0,nYOffset,nReadWidth,nReadHeight,poData,nReadWidth,nReadHeight,GDT_Float32,0,0);

#ifdef OPENCL_MODE
		SlopeCal_OpenCL(poData,poOutData,nReadWidth,nReadHeight,&slopeOption);
#else
		int nLeftTep = 0;
		int nRightTmp = 0;
		int nTopTmp = 0;
		int nBottomTmp = 0;
		for (int i = 0; i < nReadHeight; i ++)
		{
			for (int j = 0; j < nXsize; j ++)
			{
				nTopTmp = i-1;
				nBottomTmp = i+1;
				nLeftTep = j-1;
				nRightTmp = j+1;

				//边界处理情况
				if (0 == i)
				{
					nTopTmp = i;
				}
				if (0 == j)
				{
					nLeftTep = j;
				}
				if (i == nReadHeight-1)
				{
					nBottomTmp = i;
				}
				if (j == nXsize-1)
				{
					nRightTmp = j;
				}
				float dbRectData[9];
				dbRectData[0] = poData[nTopTmp*nXsize+nLeftTep];
				dbRectData[1] = poData[nTopTmp*nXsize+j];
				dbRectData[2] = poData[nTopTmp*nXsize+nRightTmp];

				dbRectData[3] = poData[i*nXsize+nLeftTep];
				dbRectData[4] = poData[i*nXsize+j];
				dbRectData[5] = poData[i*nXsize+nRightTmp];

				dbRectData[6] = poData[nBottomTmp*nXsize+nLeftTep];
				dbRectData[7] = poData[nBottomTmp*nXsize+j];
				dbRectData[8] = poData[nBottomTmp*nXsize+nRightTmp];

				//计算坡度
				poOutData[i*nXsize+j] = SlopeCal(dbRectData,999999.0,&slopeOption);
			}
		}
#endif

		//写入数据
		int pBandList[] = {1};
		if (1 == nYBlockCount)
		{
			poDstDS->RasterIO(GF_Write,0,nYOffset,nRealWidth,nRealHeight,poOutData,nRealWidth,nRealHeight,
				GDT_Float32,1,pBandList,0,0,0);
		}

		else
		{
			if (i == 0)
			{
				poDstDS->RasterIO(GF_Write,0,nYOffset,nRealWidth,nRealHeight,poOutData,nRealWidth,nRealHeight,
					GDT_Float32,1,pBandList,0,0,0);
			}
			else if (i > 0 && i < nYBlockCount-1)
			{
				poDstDS->RasterIO(GF_Write,0,nYOffset+1,nRealWidth,nRealHeight,poOutData+nRealWidth,nRealWidth,nRealHeight,
					GDT_Float32,1,pBandList,0,0,0);
			}
			else if(i == nYBlockCount-1)
			{
				poDstDS->RasterIO(GF_Write,0,nYOffset+1,nRealWidth,nRealHeight,poOutData+nRealWidth,nRealWidth,nRealHeight,
					GDT_Float32,1,pBandList,0,0,0);
			}
		}
	
		if (poData != NULL)
		{
			delete []poData;
			poData = NULL;
		}

		if (poOutData != NULL)
		{
			delete []poOutData;
			poOutData = NULL;
		}
		
	}


	GDALClose(poSrcDS);
	GDALClose(poDstDS);

	clock_t t2 = clock();
	clock_t t = t2-t1;

	printf("总共计时：%d毫秒\n",t);
	

	
	return false;
}

float SlopeCal (float* afRectData, float fDstNoDataValue,void* pData)
{
	const double radiansToDegrees = 180.0 / M_PI;
	SlopeOption *psData = (SlopeOption*)pData;

	double dx =((afRectData[0] + afRectData[3]*2 + afRectData[6]) -
		(afRectData[2]+ afRectData[5]*2 + afRectData[8])) / (psData->dbEwres*8);

	double dy =((afRectData[6] + afRectData[7]*2 + afRectData[8]) -
		(afRectData[0]+ afRectData[1]*2 + afRectData[2])) / (psData->dbNsres*8);

	double key = (dx *dx + dy * dy);

	if(psData->slopeType == DEGREE_SLOPE)
	{
		return (float)(atan(sqrt(key) ) * radiansToDegrees);
		//return key;
	}
	else if (psData->slopeType == PERCENT_SLOPE)
		return (float)(100*(sqrt(key) ));

	return 0;
		
}

void SlopeCal_OpenCL(float* poDataIn,float *poDataOut,int nWidth,int nHeight,const SlopeOption* pSlopeType)
{
	//opencl平台搭建
	cl_int status = 0;				//状态号码
	static cl_context cxGPUContext = NULL;        // OpenCL context
	static cl_command_queue cqCommandQueue = NULL;// OpenCL command que
	static cl_platform_id cpPlatform = NULL;      // OpenCL platform
	static cl_device_id cdDevice = NULL;          // OpenCL device
	static cl_program cpProgram = NULL;           // OpenCL program
	static cl_kernel ckKernel = NULL;             // OpenCL kernel

	static bool bInit = 0;		//是否初始化了
	if (!bInit)
	{
		OpenCLInit(&cpPlatform,&cdDevice,&cxGPUContext);
		BuildKernel(cpPlatform,cdDevice,cxGPUContext,&cpProgram,&cqCommandQueue);
		ckKernel = clCreateKernel(cpProgram,"slope_kernel",&status);
		bInit = 1;
	}

	cl_int errNum;
	cl_mem bufIn = clCreateBuffer(cxGPUContext,CL_MEM_READ_WRITE|CL_MEM_COPY_HOST_PTR,
		sizeof(float)*nWidth*nHeight,poDataIn,&errNum);
	cl_mem bufOut = clCreateBuffer(cxGPUContext,CL_MEM_WRITE_ONLY,
		sizeof(float)*nWidth*nHeight,NULL,&errNum);


	//设置参数
	status = clSetKernelArg(ckKernel,0,sizeof(cl_mem),&bufIn);
	status = clSetKernelArg(ckKernel,1,sizeof(cl_mem),&bufOut);
	status = clSetKernelArg(ckKernel,2,sizeof(cl_int),&nWidth);
	status = clSetKernelArg(ckKernel,3,sizeof(cl_int),&nHeight);
	SlopeOption slopeOpt;
	memcpy(&slopeOpt,pSlopeType,sizeof(SlopeOption));
	status = clSetKernelArg(ckKernel,4,sizeof(SlopeOption),&slopeOpt);

	//执行核函数
	size_t globalThreads[] = {nWidth,nHeight};
	status = clEnqueueNDRangeKernel(cqCommandQueue,ckKernel,2,
		NULL,globalThreads,NULL,0,NULL,NULL);
	status = clFinish(cqCommandQueue);

	status = clEnqueueReadBuffer(cqCommandQueue,bufOut,CL_TRUE,0,sizeof(float)*nWidth*nHeight,poDataOut,0,NULL,NULL);
	/*poDataOut = (float*)clEnqueueMapBuffer(cqCommandQueue,bufOut,CL_TRUE,CL_MAP_READ,
		0,sizeof(float)*nWidth*nHeight,0,NULL,NULL,&errNum);*/

	//清除资源
	if(ckKernel != NULL)
	{
		//status = clReleaseKernel(ckKernel);
	}
	if (cpProgram != NULL)
	{
		//status = clReleaseProgram(cpProgram);
	}
	if (bufIn != NULL)
	{
		status = clReleaseMemObject(bufIn);
	}
	if (bufOut != NULL)
	{
		status = clReleaseMemObject(bufOut);
	}
	if (cqCommandQueue != NULL)
	{
		//status = clReleaseCommandQueue(cqCommandQueue);
	}
	if (cxGPUContext != NULL)
	{
		//status = clReleaseContext(cxGPUContext);
	}
	
}

bool CreateGeoGrid(GDALGridAlgorithm eAlgorithm, 
				   GUInt32 nPoints, 
				   const double * padfX, 
				   const double * padfY, 
				   const double * padfZ, 
				   double dfXMin, 
				   double dfXMax, 
				   double dfYMin, 
				   double dfYMax, 
				   GUInt32 nXSize, 
				   GUInt32 nYSize, 
				   GDALDataType eType, 
				   const char* pszOutFile )
{
	//创建输出缓冲区
	void* poData = CPLMalloc(nXSize*nYSize*GDALGetDataTypeSize(eType));

	CPLErr err;
	//网格化
	err = GDALGridCreate(eAlgorithm,NULL,nPoints,padfX,padfY,padfZ,
		dfXMin,dfXMax,dfYMin,dfYMax,nXSize,nYSize,eType,poData,NULL,NULL);

	if (err != CE_None)
	{
		return false;
	}

	GDALAllRegister();
	OGRRegisterAll();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8","NO");

	//创建输出文件驱动
	GDALDriver *poDriver = NULL;
	poDriver = GetGDALDriverManager()->GetDriverByName("GTiff");

	GDALRasterBand *poDestBand = NULL;		//目标波段，就一个波段
	GDALDataset* poDestDS = NULL;			//目标数据源
	poDestDS = poDriver->Create(pszOutFile,nXSize,nYSize,1,eType,NULL);
	if (poDestDS == NULL)
	{
		return false;
	}

	poDestBand = poDestDS->GetRasterBand(1);

	//写入数据
	err = poDestBand->RasterIO(GF_Write,0,0,nXSize,nYSize,poData,nXSize,nYSize,eType,0,0);
	if (err != CE_None)
	{
		return false;
	}

	//写入仿射变换系数
	double dbGeotrans[6];
	dbGeotrans[0] = dfXMin;
	dbGeotrans[3] = dfYMax;
	dbGeotrans[1] = (dfXMax-dfXMin)/nXSize;
	dbGeotrans[5] = -(dfYMax-dfYMin)/nYSize;
	dbGeotrans[2] = 0;
	dbGeotrans[4] = 0;

	poDestDS->SetGeoTransform(dbGeotrans);

	GDALClose(poDestDS);

	return true;
	
}
