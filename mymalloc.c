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