#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <CL/cl.h>


int main(int argc, char** argv)
{
	int SIZE=16;
	
	
	int* h_A = (int*)malloc(sizeof(int)* SIZE);

	h_A[0] = 1;
	h_A[1] = 2;
	h_A[2] = 10;
	h_A[3] = 9;
	h_A[4] = 8;
	h_A[5] = 7;
	h_A[6] = 1;
	h_A[7] = 0;
	h_A[8] = 10;
	h_A[9] = 11;
	h_A[10] = 3;
	h_A[11] = 4;
	h_A[12] = 13;
	h_A[13] = 16;
	h_A[14] = 1;
	h_A[15] = 5;

	
	
	// Retrieving Platforms
	cl_int status;
	cl_uint numPlatforms;
	cl_platform_id *platforms = NULL;

	status = clGetPlatformIDs(0, NULL, &numPlatforms);
	platforms = (cl_platform_id *)malloc(numPlatforms * sizeof(cl_platform_id));
	status = clGetPlatformIDs(numPlatforms, platforms, NULL);
	if (status != 0) 
	{
		printf("Error: %s\n", status);
	}


	// Retrieving Devices
	cl_uint numDevices;
	cl_device_id *devices = NULL;

	status = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_GPU, 0, NULL, &numDevices);
	devices = (cl_device_id *)malloc(numDevices * sizeof(cl_device_id));
	status = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_GPU, numDevices, devices, NULL);
	if (status != 0) 
	{
		printf("Error: %s\n", status);
	}


	// Creating Context
	cl_context context = NULL;
	context = clCreateContext(NULL, numDevices, devices, NULL, NULL, &status);
	if (status != 0) 
	{
		printf("Error: %s\n", status);
	}


	// Creating a Command Queue
	cl_command_queue cmdQueue;
	cmdQueue = clCreateCommandQueue(context, devices[0], CL_QUEUE_PROFILING_ENABLE, &status);
	if (status != 0) 
	{
		printf("Error: %s\n", status);
	}


	// Setup device memory
	cl_mem d_A;
	d_A = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(int)* SIZE, h_A, &status);
	if (status != 0) 
	{
		printf("Error: %s\n", status);
	}
	

	//Load and build OpenCL kernel
	cl_kernel clKernel;
	const char fileName[] = "bitonic_sort.cl";
	size_t sourceSize;
	char *source_str;
	FILE* fp = fopen(fileName, "r");
	if (!fp) 
	{
		printf("Error while loading the source code %s\n", fp);
		exit(1);
	}
	source_str = (char *)malloc(sizeof(char) * 1000000);
	sourceSize = fread(source_str, 1, 1000000, fp);
	fclose(fp);


	cl_program clProgram;
	clProgram = clCreateProgramWithSource(context, 1, (const char **)&source_str, (const size_t*)&sourceSize, &status);
	if (status != 0) 
	{
		printf("Error: %d\n", status);
	}
	
	status = clBuildProgram(clProgram, 0, NULL, NULL, NULL, NULL);
	if (status != 0) 
	{
		printf("Error: %d\n", status);
	}
	
	clKernel = clCreateKernel(clProgram, "bitonic", &status);
	if (status != 0) 
	{
		printf("Error: %d\n", status);
	}

	cl_int n = 16;
	cl_int l = 4;
	status = clSetKernelArg(clKernel, 0, sizeof(cl_mem), (void *)&d_A);
	status = clSetKernelArg(clKernel, 1, sizeof(int), (void *)&n);
	status = clSetKernelArg(clKernel, 2, sizeof(int), (void *)&l);
	if (status != 0) 
	{
		printf("Error: %d\n", status);
	}

	size_t  globalWorkSize[2];
	size_t localWorkSize[2];
	localWorkSize[0] = 8;
	localWorkSize[1] = 1;
	globalWorkSize[0] = 8;
	globalWorkSize[1] = 1;


	cl_event profiling_event;

	status = clEnqueueNDRangeKernel(cmdQueue, clKernel, 2, NULL, globalWorkSize, localWorkSize, 0, NULL, &profiling_event);
	if (status != 0) 
	{
		printf("Error: %s\n", status);
	}
	
	clWaitForEvents(1, &profiling_event);

	cl_ulong start_time;
	cl_ulong finish_time;
	clGetEventProfilingInfo(profiling_event, CL_PROFILING_COMMAND_START, sizeof(start_time), &start_time, NULL);
	clGetEventProfilingInfo(profiling_event, CL_PROFILING_COMMAND_END, sizeof(finish_time), &finish_time, NULL);


	cl_ulong total_time = finish_time - start_time;
	printf("Start time in nanoseconds = %lu\n", start_time);
	printf("End time in nanoseconds = %lu\n", finish_time);
	printf("Total time in nanoseconds = %lu\n", total_time);
	
	//Retrieve result from device
	status = clEnqueueReadBuffer(cmdQueue, d_A, CL_TRUE, 0, sizeof(int)* SIZE, h_A, 0, NULL, NULL);
	if (status != 0) 
	{
		printf("Error: %s\n", status);
	}

	//Print out the results
	for (unsigned i = 0; i < 16; i++)
	{
		printf("%d\n", h_A[i]);
	}


	free(h_A);

	clReleaseMemObject(d_A);

	free(devices);
	free(platforms);

	clReleaseKernel(clKernel);
	clReleaseProgram(clProgram);
	clReleaseCommandQueue(cmdQueue);

	getchar();
}
