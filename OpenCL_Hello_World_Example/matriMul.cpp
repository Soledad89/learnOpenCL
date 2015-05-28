#include <stdio.h>
#include <sys/malloc.h>
#include <stdlib.h>
#include <time.h>
#include <OpenCL/OpenCL.h>
#define WA 1000
#define WB 1000
#define HA 1000
#define HB 1000
#define KERNEL(...)#__VA_ARGS__
const char *kernelSourceCode=KERNEL(__kernel void mul(__global float*OC,
	int wA,
	int hA,
	int wB,
	int hB,
	__global float*IA,
	__global float*IB){
	int row=get_global_id(1);
	int col=get_global_id(0);
	float sum=0;
	for(int i=0;i<wA;i++)
	{
		sum+=IA[row*wA+i]*IB[i*wB+col];
	}
	OC[row*wB+col]=sum;});

int main()
{
	clock_t start,finish;
	float *input_a,*input_b,*output;
	int wa=WA;
	int ha=HA;
	int wb=WB;
	int hb=HB;
	input_a=(float*)malloc(sizeof(float)*HA*WA);
	input_b=(float*)malloc(sizeof(float)*HB*WB);
	output=(float*)malloc(sizeof(float)*HA*WB);
	for(int i=0;i<HA*WA;i++)
	{
		input_a[i]=1;

	}
	for(int i=0;i<HB*WB;i++)
	{
		input_b[i]=1;

	}
	for(int i=0;i<HA*WB;i++)
	{
		output[i]=0;

	}
	cl_int status=0;
	size_t deviceListSize;
	cl_uint numPlatforms;
	cl_platform_id platform=NULL;
	status=clGetPlatformIDs(0,NULL,&numPlatforms);
	status=clGetPlatformIDs(1,&platform,NULL);
    cl_device_id device;
	status=clGetDeviceIDs(
		platform,
		CL_DEVICE_TYPE_GPU,
		1,
		&device,
		NULL);
	cl_context_properties properties[3];
	properties[0]= CL_CONTEXT_PLATFORM;
	properties[1]= (cl_context_properties) platform;
	properties[2]= 0;
	cl_context context=clCreateContext(
		properties,
		1,
		&device,
		NULL,NULL,NULL);
	cl_command_queue queue=clCreateCommandQueue(
		context,
		device,
		0,
		NULL);
	cl_mem A=clCreateBuffer(
		context,
		CL_MEM_READ_WRITE,
		sizeof(float)*WA*HA,
		NULL,NULL);
	cl_mem B=clCreateBuffer(
		context,
		CL_MEM_READ_WRITE,
		sizeof(float)*WB*HB,
		NULL,NULL);
	cl_mem C=clCreateBuffer(
		context,
		CL_MEM_READ_WRITE,
		sizeof(float)*HA*WB,
		NULL,NULL);
	status=clEnqueueWriteBuffer(
		queue,
		A,
		CL_TRUE,
		0,
		sizeof(float)*WA*HA,
		input_a,
		0,
		NULL,
		NULL);
	status=clEnqueueWriteBuffer(
		queue,
		B,
		CL_TRUE,
		0,
		sizeof(float)*WB*HB,
		input_b,
		0,
		NULL,
		NULL);
	status=clEnqueueWriteBuffer(
		queue,
		C,
		CL_TRUE,
		0,
		sizeof(float)*HA*WB,
		output,
		0,
		NULL,
		NULL);

	cl_program prog=clCreateProgramWithSource(
		context,
		1,
		(const char **) &kernelSourceCode,
		NULL,
		NULL);
	status=clBuildProgram(prog,1,&device,NULL,NULL,NULL);
	cl_kernel mykernel=clCreateKernel(
		prog,
		"mul",
		NULL);
	clSetKernelArg(mykernel,0,sizeof(cl_mem),&C);
	clSetKernelArg(mykernel,1,sizeof(cl_int),&wa);
	clSetKernelArg(mykernel,2,sizeof(cl_int),&ha);
	clSetKernelArg(mykernel,3,sizeof(cl_int),&wb);
	clSetKernelArg(mykernel,4,sizeof(cl_int),&hb);
	clSetKernelArg(mykernel,5,sizeof(cl_mem),&A);
	clSetKernelArg(mykernel,6,sizeof(cl_mem),&B);
	size_t local[2]={10,10} ;
	size_t global[2]={HA,WB};
	start=clock();
	status=clEnqueueNDRangeKernel(
		queue,
		mykernel,
		2,
		NULL,
		global,
		local,
		0,
		NULL,
		NULL);
	finish=clock();
	status=clEnqueueReadBuffer(
		queue,
		C,
		CL_TRUE,
		0,
		HA*WB*sizeof(float),
		output,
		0,
		NULL,
		NULL);
	clFinish(queue);
	/*for(int i=0;i<HA*WB;i++)
	{
		printf("%f ",output[i]);
		if(i%WB==0)
		{
			printf("\n");
		}
	}*/
	clReleaseMemObject(A);
	clReleaseMemObject(B);
	clReleaseMemObject(C);
	clReleaseProgram(prog);
	clReleaseKernel(mykernel);
	clReleaseCommandQueue(queue);
	clReleaseContext(context);
	finish=clock();
	double duration=(double)(finish-start)/CLOCKS_PER_SEC;
	printf("GPU time usage: %f\n",duration);
	start=clock();
	for(int i=0;i<HA;i++)
	{
		for(int j=0;j<WB;j++)
		{
			for(int k=0;k<WA;k++)
			{
				output[i*WB+j]+=input_a[i*WA+k]*input_b[k*WB+j];
			}
		}	
	}
	finish=clock();
	duration=(double)(finish-start)/CLOCKS_PER_SEC;
	printf("CPU time usage: %f",duration);
	getchar();
	free(input_a);
	free(input_b);
	free(output);
    return 0;
}
