int inc_header(int *array, int *curheader)
{
	if (array[*curheader] < 0)
	{
		*curheader += -1*array[*curheader] + 1;
	}
	else
	{
		*curheader += array[*curheader] + 1;
	}

	if (*curheader > array[0])
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
	int curheader = 1;

	while (1)
	{
		// positive header int, implies free space of this size
		if (array[curheader] >= size)
		{
			
			if (size + 1 < array[curheader])
			{
				// Allocate space, then put header after data for new section of free data
				array[curheader + size + 1] = array[curheader] - size - 1;
				array[curheader] = -1*size;

			} 
			else
			{
				// No size for a new header, the entire array is being used
				array[curheader] *= -1; 	
			}

			return &array[curheader+1];
		}

		if (!inc_header(array, &curheader)) return (int *) 0;
	} 



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
			if (array[nextheader] > 0)
			{
				array[curheader] += array[nextheader] + 1;
			}

			//Last block is free - combine
			if (array[lastheader] > 0)
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
	if (array[1] = array[0] -2) return 1;
	return 0; 
}
