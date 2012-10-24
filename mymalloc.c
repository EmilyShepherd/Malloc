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