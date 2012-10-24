typedef struct header
{
	/**
	 * 0 = Free
	 * 1 = Used
	 */
	char free;
	
	struct header *next;

	int size;
} header;

int myinit(int *array, int size)
{
	array[0] = size;

	header* firstheader = (header *)&array[1];
	firstheader->free   = 0;
	firstheader->size   = size - sizeof(header) - sizeof(int);
	firstheader->next   = 0;

	return 1;
}

int mymalloc(int *array, int size)
{
}

int myfree(int *array, int * block)
{
}

int mydispose(int *array)
{
}