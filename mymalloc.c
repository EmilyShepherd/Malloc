#include <stdio.h>

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

	if (*curheader >= array[0])
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

int decode_header(int *header)
{
    unsigned char *head = (unsigned char *)header;
    int shift = 0;
    unsigned int value = 0;
    int free = 0;
    
    //The 7th bit has in the first byte is the free flag,
    //so check that. If it's 1, set it to 0 so it doesn't
    //get used as part of the value by the code below.
    if ((*head & 0x40) == 0x40)
    {
        free   = 1;
        *head &= 0xBF; //1011 1111
    }
    
    while (1)
    {
        value += (*head & 127) << (shift * 7);
        
        if (*head >> 7 == 1)
        {
            shift++;
            head++;
        }
        else
        {
            return (signed int)value;
        }
    }
}

int encode_header(int *array, int free, int value)
{
    //unsigned char *arr = (unsigned char *)array;
    int pos = 0;
    unsigned int uvalue = (unsigned int)value;
    
    //Split the number into 5 bytes, each containing
    //7 bits of the value.
    unsigned char byte[5];
    byte[0] = uvalue >> 28 & 0x0F;
    byte[1] = uvalue >> 21 & 0x7F;
    byte[2] = uvalue >> 14 & 0x7F;
    byte[3] = uvalue >> 7  & 0x7F;
    byte[4] = uvalue       & 0x7F;
    
    //Loop through the bytes, looking for the first
    //one that's used - so we need not save the higher
    //unused bytes.
    for (pos = 0; byte[pos] == 0; pos++);
    
    //Check if this byte uses its 7th bit. If it does
    //we'll need to create a new byte, because we need
    //the 7th bit of the first byte for the sign.
    if ((byte[pos] & 0x40) == 0x40)
    {
        pos--;
        
        byte[pos] = 0x80; //1000 0000
    }
    
    if (free)
    {
        byte[pos] = byte[pos] | 0x40; //0100 0000
    }
    
    for (int i = pos; i < 5; i++)
    {
        printf("%d\n", byte[i]);
    }
    
    return 0;
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
