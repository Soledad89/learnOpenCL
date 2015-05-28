

#include "OpenCL.h"

const char* cSourceFile = "E:\\my-space\\PIEGeoImage3.1\\src\\Test\\TestSlope\\kernel.cl";	//�˺������ڵ��ļ�

char* LoadProgSource(const char* cFilename, const char* cPreamble, size_t* szFinalLength)
{
	FILE* pFileStream = NULL;
	size_t szSourceLength;

	// open the OpenCL source code file
	pFileStream = fopen(cFilename, "rb");
	if(pFileStream == 0) 
	{       
		return NULL;
	}

	size_t szPreambleLength = strlen(cPreamble);

	// get the length of the source code
	fseek(pFileStream, 0, SEEK_END); 
	szSourceLength = ftell(pFileStream);
	fseek(pFileStream, 0, SEEK_SET); 

	// allocate a buffer for the source code string and read it in
	char* cSourceString = (char *)malloc(szSourceLength + szPreambleLength + 1); 
	memcpy(cSourceString, cPreamble, szPreambleLength);
	if (fread((cSourceString) + szPreambleLength, szSourceLength, 1, pFileStream) != 1)
	{
		fclose(pFileStream);
		free(cSourceString);
		return 0;
	}

	// close the file and return the total length of the combined (preamble + source) string
	fclose(pFileStream);
	if(szFinalLength != 0)
	{
		*szFinalLength = szSourceLength + szPreambleLength;
	}
	cSourceString[szSourceLength + szPreambleLength] = '\0';

	return cSourceString;
}

void OpenCLInit(cl_platform_id *clPlatform ,cl_device_id *clDevice,cl_context *clContext)
{
	cl_uint numPlatforms = 0;   //GPU����ƽ̨����
	cl_platform_id platform = NULL;
	clGetPlatformIDs(0,NULL,&numPlatforms);

	//���ƽ̨�б�
	cl_platform_id * platforms = (cl_platform_id*)malloc(numPlatforms * sizeof(cl_platform_id));
	clGetPlatformIDs (numPlatforms, platforms, NULL);

	//��ѯ����opencl�豸
	for (cl_uint i = 0; i < numPlatforms; i ++)
	{
		char pBuf[100];
		clGetPlatformInfo(platforms[i],CL_PLATFORM_VERSION,sizeof(pBuf),pBuf,NULL);
		printf("%s\n",pBuf);
		platform = platforms[i];
		//break;
	}

	*clPlatform	= platform;

	free(platforms);

	cl_int status = 0;

	//���GPU�豸
	cl_device_id device;
	status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
	*clDevice = device;

	//����������
	cl_context context = clCreateContext(0, 1, &device, NULL, NULL, &status);
	*clContext = context;


} 

void BuildKernel(cl_platform_id platform ,
				 cl_device_id device,
				 cl_context context,
				 cl_program *clProgram,
				 cl_command_queue *clCommandQueue)
{
	cl_int status = 0;
	//װ���ں˳���
	size_t szKernelLength = 0;

	const char* kernelSourceCode = LoadProgSource(cSourceFile, "", &szKernelLength);  

	cl_program program = clCreateProgramWithSource(context,1,&kernelSourceCode,&szKernelLength,&status);
	*clProgram = program;

	//Ϊ����ָ�����豸����CL_program
	status = clBuildProgram(program,1,&device,NULL,NULL,NULL);  
	size_t len = 0;
	char buf[2048];
	if (status != CL_SUCCESS)
	{
		status = clGetProgramBuildInfo(program,device,CL_PROGRAM_BUILD_LOG,sizeof(buf),buf,&len);
		printf("%s\n", buf);
	}

	//����һ��opencl�������
	*clCommandQueue = clCreateCommandQueue(context,device,0,&status);
}