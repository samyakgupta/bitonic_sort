// kernel.cl
// Perform bitonic sort
// Device code.

// l defines the number of times the bitonic sequence is formed. For 16 elements it's 4.
// n is the number of elements

// OpenCL Kernel
__kernel void bitonic(__global int* A, uint n, uint l)
{

	uint j,t,index,index2,signo,i;
	int aux;
	uint id = get_global_id(0);

	barrier(CLK_LOCAL_MEM_FENCE);


	for (i = 1; i <= l; i++) 
	{
		signo = (id >> (i - 1)) & 1;
		//if(id==4)
		//printf("i is %d signo is %d in process id %d\n", i,signo,id);

		for ( j = i; j>0; j--) 
		{
			t = 1 << (j - 1);
			index = (id >> (j - 1));
			index = index << j;

			index = index + (id & (t - 1));
			//if(id==5)
			//printf("| j == %d | index == %d    | signo = %d | process = %d   |\n", j,index,signo,id);

			index2 = index + t;
			//if(id==5)
			//printf("| j == %d | index2 == %d   | signo = %d | process = %d   |\n", j,index2,signo,id);

			if ((A[index] > A[index2]) ^ (signo)) 
			{
				aux = A[index];
				A[index] = A[index2];
				A[index2] = aux;
			}

			barrier(CLK_LOCAL_MEM_FENCE);
		}
	}


}
#pragma once
