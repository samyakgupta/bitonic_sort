// kernel.cl
// Perform bitonic sort
// Device code.

// l defines the number of times the bitonic sequence is formed. For 16 elements it's 4.
// n is the number of elements

// OpenCL Kernel
__kernel void bitonic(__global int* A, uint n, uint l)
{

	uint j, t, index, index2, signo, i;
	uint id = get_global_id(0);
	local int LA[16];

	LA[id] = A[id];
	LA[id + 8] = A[id + 8];
	barrier(CLK_LOCAL_MEM_FENCE);

	for (i = 1; i <= l; i++)
	{
		signo = (id >> (i - 1)) & 1;
		//if(id==4)
		//printf("i is %d signo is %d in process id %d\n", i,signo,id);

		for (j = i; j>0; j--)
		{
			t = 1 << (j - 1);
			index = (id >> (j - 1));
			index = index << j;

			index = index + (id & (t - 1));
			//if (id == 4)
			//printf("| j == %d | index == %d    | signo = %d | process = %d   |\n", j, index, signo, id);

			index2 = index + t;
			//if (id == 4)
			//printf("| j == %d | index2 == %d   | signo = %d | process = %d   |\n", j, index2, signo, id);

			if ((LA[index] > LA[index2]) ^ (signo))
			{
				float aux = LA[index];
				LA[index] = LA[index2];
				LA[index2] = aux;
			}

			barrier(CLK_LOCAL_MEM_FENCE);
		}
	}

	A[id] = LA[id];
	A[id + 8] = LA[id + 8];


}
