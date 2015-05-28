#pragma once

#include "cl/cl.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>



#pragma comment(lib,"OpenCL.lib")


char* LoadProgSource(const char* cFilename, const char* cPreamble, size_t* szFinalLength);

//初始化
void OpenCLInit(cl_platform_id *clPlatform ,cl_device_id *clDevice,cl_context *clContext);

//建立核对像
void BuildKernel(cl_platform_id platform ,
				 cl_device_id device,
				 cl_context context,
				 cl_program *clProgram,
				 cl_command_queue *clCommandQueue);