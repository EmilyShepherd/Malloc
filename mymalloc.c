int myinit(int *array, int size)
{
	if (size < 14) return 0;

	array[0] = size;

	array[1] = size - 2;

	return 1;
}

int* mymalloc(int *array, int size)
{
	//char*   arr       = (char *)array;
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

	} 



}

int myfree(int *array, int * block)
{
	//Always fail, until implemented
	return 0;
}

int mydispose(int *array)
{
/*	header* firstheader = (header *)&array[1];

	if (firstheader->free != 0) return 0;
	if (firstheader->size != array[0] - sizeof(header) - sizeof(int)) return 0;
	if (firstheader->next != 0) return 0;
*/
	return 1; 
}
