#include "mymalloc.h"
#include <pthread.h>

pthread_mutex_t mutex;

static int inc_header(int *array, int *curheader)
{
	if (array[*curheader] < 0)
	{
		*curheader += -1*array[*curheader] + 1;
	}
	else
	{
		*curheader += array[*curheader] + 1;
	}

	if (*curheader >= array[0])
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

int myinit(int *array, int size)
{
	if (size < 14) return 0;

	array[0] = size;

	array[1] = size - 2;

	return 1;
}

int* mymalloc(int *array, int size)
{
	int curheader   = 1;
	int bestheader  = 0;
	int bestsize    = array[0];
	int wasteheader = 0;

	while (1)
	{
		// positive header int, implies free space of this size
		if (array[curheader] >= size)
		{
			//Perfect fit, use it.
			if (size == array[curheader])
			{
				array[curheader] *= -1;
				return &array[curheader+1];
			}
			else if (size + 1 == array[curheader])
			{
				wasteheader = curheader;
			}
			else if (array[curheader] < bestsize)
			{
				bestheader = curheader;
				bestsize   = array[curheader];
			}
		}

		if (!inc_header(array, &curheader)) break; //return (int *) 0;
	} 

	if (bestheader != 0)
	{
		// Allocate space, then put header after data for new section of free data
		array[bestheader + size + 1] = array[bestheader] - size - 1;
		array[bestheader] = -1*size;

		return &array[bestheader+1];
	}
	else if (wasteheader != 0)
	{
		array[wasteheader] *= -1;

		return &array[wasteheader+1];
	}

	return (int *)0;
}

int myfree(int *array, int *block)
{
	int curheader  = 1;
	int lastheader = 0;
	
	while (1)
	{
		if (&array[curheader + 1] == block)
		{
			//This block exists, but it is not being used
			//ignore this free request
			if (array[curheader] > 0)
			{
				return 0;
			}

			//Mark it as free
			array[curheader] *= -1;

			int nextheader = curheader + 1 + array[curheader];

			//Next block is free - combine
			if (nextheader < array[0] && array[nextheader] > 0)
			{
				array[curheader] += array[nextheader] + 1;
			}

			//Last block is free - combine
			if (lastheader != 0 && array[lastheader] > 0)
			{
				array[lastheader] += array[curheader] + 1;
			}

			return 1;
		}

		//Save this header and move unto the next one
		lastheader = curheader;
		if (!inc_header(array, &curheader)) return 0;
	}
}

int mydispose(int *array)
{

	// Return one if free space pointer correctly pointers to the rest of the array
	if (array[1] == array[0] - 2) return 1;
	return 0; 
}

int myinit_mt(int *array, int size)
{
	pthread_mutex_lock(&mutex);
	int result = myinit(array, size);
	pthread_mutex_unlock(&mutex);

	return result;
}

int* mymalloc_mt(int *array, int size)
{
	pthread_mutex_lock(&mutex);
	int *result = mymalloc(array, size);
	pthread_mutex_unlock(&mutex);

	return result;
}

int myfree_mt(int *array, int *block)
{
	pthread_mutex_lock(&mutex);
	int result = myfree(array, block);
	pthread_mutex_unlock(&mutex);

	return result;
}

int mydispose_mt(int *array)
{
	pthread_mutex_lock(&mutex);
	int result = mydispose(array);
	pthread_mutex_unlock(&mutex);

	return result;
}


