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
	if (size < 14) return 0;

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
	header* firstheader = (header *)&array[1];

	if (firstheader->free != 0) return 0;
	if (firstheader->size != array[0] - sizeof(header) - sizeof(int)) return 0;
	if (firstheader->next != 0) return 0;

	return 1;
}