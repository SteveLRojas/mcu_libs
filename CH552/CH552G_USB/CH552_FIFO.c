#include "CH552.H"
#include "CH552_FIFO.h"

// HINT: fifo_size must be a power of 2
void fifo_init(fifo_t* fifo_inst, UINT8* fifo_buf, UINT16 fifo_size)
{
	fifo_inst->buf_size = fifo_size;
	fifo_inst->pbuf = fifo_buf;
	fifo_inst->count = 0;
	fifo_inst->front = 0;
	fifo_inst->back = 0;
}

UINT8 fifo_push(fifo_t* fifo_inst, UINT8 val) reentrant
{
	if(fifo_full(fifo_inst))
	{
		return 0; // FIFO full
	}

	fifo_inst->pbuf[fifo_inst->back] = val;
	fifo_inst->back += 1;
	fifo_inst->back &= (UINT16)(fifo_inst->buf_size - 1); // Handle wrap-around
	fifo_inst->count += 1;

	return 1;
}

UINT8 fifo_pop(fifo_t* fifo_inst) reentrant
{
	UINT8 val;
	if(fifo_empty(fifo_inst))
	{
		return 0; // Nothing in FIFO
	}

	val = fifo_inst->pbuf[fifo_inst->front];
	fifo_inst->front += 1;
	fifo_inst->front &= (UINT16)(fifo_inst->buf_size - 1); // Handle wrap-around
	fifo_inst->count -= 1;

	return val;
}

UINT8 fifo_read(fifo_t* fifo_inst, UINT8* dest, UINT16 num_bytes)
{
	UINT16 to_wrap;
	UINT8* read_ptr;
	UINT16 idx;

	if(num_bytes > fifo_inst->count)
	{
		return 0; // Not enough data to read
	}

	to_wrap = fifo_inst->buf_size - fifo_inst->front;
	if(num_bytes >= to_wrap)
	{
		// read until wrap around
		read_ptr = fifo_inst->pbuf + fifo_inst->front;
		for(idx = 0; idx < to_wrap; ++idx)
		{
			*dest = read_ptr[idx];
			++dest;
		}

		fifo_inst->front = 0;
		fifo_inst->count -= to_wrap;
		num_bytes -= to_wrap;
	}

	//read remaining - no wrap around
	read_ptr = fifo_inst->pbuf + fifo_inst->front;
	for(idx = 0; idx < num_bytes; ++idx)
	{
		*dest = read_ptr[idx];
		++dest;
	}

	fifo_inst->front += num_bytes;
	fifo_inst->count -= num_bytes;

	return 1;
}

UINT8 fifo_write(fifo_t* fifo_inst, UINT8* src, UINT16 num_bytes)
{
	UINT16 to_wrap;
	UINT8* write_ptr;
	UINT16 idx;

	if(num_bytes > (fifo_inst->buf_size - fifo_inst->count))
	{
		return 0; // Not enough space to write
	}

	to_wrap = fifo_inst->buf_size - fifo_inst->back;
	if(num_bytes >= to_wrap)
	{
		//write until wrap around
		write_ptr = fifo_inst->pbuf + fifo_inst->back;
		for(idx = 0; idx < to_wrap; ++idx)
		{
			write_ptr[idx] = *src;
			++src;
		}

		fifo_inst->back = 0;
		fifo_inst->count += to_wrap;
		num_bytes -= to_wrap;
	}

	//write remaining - no wrap around
	write_ptr = fifo_inst->pbuf + fifo_inst->back;
	for(idx = 0; idx < num_bytes; ++idx)
	{
		write_ptr[idx] = *src;
		++src;
	}

	fifo_inst->back += num_bytes;
	fifo_inst->count += num_bytes;

	return 1;
}
