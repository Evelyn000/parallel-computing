#include <stdio.h>
#include <stdlib.h>
#include "OpenCL/opencl.h"
#include "./util.h"
//#define MEM_SIZE (128)
#define MAX_SOURCE_SIZE (0x100000)
void printwriterr(cl_int ret)
{
	if (ret==CL_INVALID_COMMAND_QUEUE)
		printf("CL_INVALID_COMMAND_QUEUE\n");
	else if (ret==CL_INVALID_CONTEXT)
		printf("CL_INVALID_CONTEXT\n");
	else if (ret==CL_INVALID_MEM_OBJECT)
		printf("CL_INVALID_MEM_OBJECT\n");
	else if (ret==CL_INVALID_VALUE)
		printf("CL_INVALID_VALUE\n");
	else if (ret==CL_INVALID_EVENT_WAIT_LIST)
		printf("CL_INVALID_EVENT_WAIT_LIST \n");
	else if (ret==CL_MEM_OBJECT_ALLOCATION_FAILURE)
		printf("CL_MEM_OBJECT_ALLOCATION_FAILURE\n");
	else if (ret==CL_OUT_OF_HOST_MEMORY)
		printf("CL_OUT_OF_HOST_MEMORY\n");
}

void printcreatebuffererr(int i)
{
	if (i==CL_INVALID_CONTEXT)
		printf("CL_INVALID_CONTEXT\n");
	else if (i==CL_INVALID_VALUE)
		printf("CL_INVALID_VALUE\n");
	else if (i==CL_INVALID_BUFFER_SIZE)
		printf("CL_INVALID_BUFFER_SIZE\n");
	else if (i==CL_INVALID_HOST_PTR)
		printf("CL_INVALID_HOST_PTR\n");
	else if (i==CL_MEM_OBJECT_ALLOCATION_FAILURE)
		printf("CL_MEM_OBJECT_ALLOCATION_FAILURE\n");
	else if (i==CL_OUT_OF_HOST_MEMORY)
		printf("CL_OUT_OF_HOST_MEMORY\n");
}

int main()
{
	cl_device_id device_id = NULL;
	cl_context context=NULL;
	cl_command_queue command_queue=NULL;
	cl_mem inmat, invec, outvec;
	cl_program program=NULL;
	cl_kernel kernel=NULL;
	cl_platform_id platform_id=NULL;
	cl_uint ret_num_devices;
	cl_uint ret_num_platforms;
	cl_int ret;
	
	float *a;
	float *b, *c;
	int w, h, w1, h1; //h*w

	read_matrix("matrix_a", &a, &h, &w); 
	//print_matrix(a, h, w);
	read_vector("matrix_b", &b, &h1, &w1);
	//print_vector(b, h1);
	size_t sizeA=h*w*sizeof(float);
	size_t sizeB=w*sizeof(float);
	size_t sizeC=h*sizeof(float);
	size_t work_size = h;
	if (w!=h1||w1!=1)
	{
		printf("Incompatible matrix/vector dimensions");
		return -1;
	}
	c=(float*)malloc(h*sizeof(float));
	FILE *fp;
	char fileName[]="./mvmultiply.cl";
	char *source_str;
	size_t source_size;
	fp=fopen(fileName, "r");
	if (!fp)
	{
		fprintf(stderr, "Failed to load kernel.\n");
		exit(1);
	}
	source_str=(char*)malloc(MAX_SOURCE_SIZE);
	source_size=fread(source_str, 1, MAX_SOURCE_SIZE, fp);
	fclose(fp);

	ret=clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
	ret=clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id, &ret_num_devices);

	context=clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);
	command_queue=clCreateCommandQueue(context, device_id, 0, &ret);

	inmat=clCreateBuffer(context, CL_MEM_READ_WRITE, sizeA, NULL, &ret);
	printcreatebuffererr(ret);
	invec=clCreateBuffer(context, CL_MEM_READ_WRITE, sizeB, NULL, &ret);
	printcreatebuffererr(ret);
	outvec=clCreateBuffer(context, CL_MEM_READ_WRITE, sizeC, NULL, &ret);
	printcreatebuffererr(ret);

	ret=clEnqueueWriteBuffer(command_queue, inmat, CL_TRUE, 0, sizeA, (void*)a, 0, NULL, NULL);
	printwriterr(ret);
	ret=clEnqueueWriteBuffer(command_queue, invec, CL_TRUE, 0, sizeB, (void*)b, 0, NULL, NULL);
	printwriterr(ret);
	program=clCreateProgramWithSource(context, 1, (const char **)&source_str, (const size_t *)&source_size, &ret);

	ret=clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);

	kernel=clCreateKernel(program, "mvmult", &ret);

	ret=clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*)&inmat);
	ret=clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*)&invec);
	ret=clSetKernelArg(kernel, 2, sizeof(cl_mem), (void*)&outvec);
	ret=clSetKernelArg(kernel, 3, sizeof(cl_int), (void*)&h);
	ret=clSetKernelArg(kernel, 4, sizeof(cl_int), (void*)&w);

	ret=clEnqueueTask(command_queue, kernel, 0, NULL, NULL);

	ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, 0, &work_size, 0, 0, 0, 0);
	//ret=clEnqueueTask(command_queue, kernel, 0, NULL, NULL);

	ret=clEnqueueReadBuffer(command_queue, outvec, CL_TRUE, 0, sizeC, c, 0, NULL, NULL);

	//print_vector(c, h);

	ret=clFlush(command_queue);
	ret=clFinish(command_queue);
	ret=clReleaseKernel(kernel);
	ret=clReleaseProgram(program);
	ret=clReleaseMemObject(inmat);
	ret=clReleaseMemObject(invec);
	ret=clReleaseMemObject(outvec);
	ret=clReleaseCommandQueue(command_queue);
	ret=clReleaseContext(context);

	free(source_str);

	return 0;
}