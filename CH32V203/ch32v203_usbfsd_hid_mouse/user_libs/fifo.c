#include <stdint.h>
#include "fifo.h"

void fifo_init(fifo_t* fifo_inst, uint8_t* fifo_buf, uint16_t fifo_size)
{
	fifo_inst->buf_size = fifo_size;
	fifo_inst->pbuf = fifo_buf;
	fifo_inst->count = 0;
	fifo_inst->front = 0;
	fifo_inst->back = 0;
}

uint8_t fifo_push(fifo_t* fifo_inst, uint8_t val)
{
	if(fifo_full(fifo_inst))
	{
		return 0; // FIFO full
	}

	fifo_inst->pbuf[fifo_inst->back] = val;
	fifo_inst->back += 1;
	fifo_inst->back &= (uint16_t)(fifo_inst->buf_size - 1); // Handle wrap-around
	fifo_inst->count += 1;

	return 1;
}

uint8_t fifo_pop(fifo_t* fifo_inst)
{
	uint8_t val;
	if(fifo_empty(fifo_inst))
	{
		return 0; // Nothing in FIFO
	}

	val = fifo_inst->pbuf[fifo_inst->front];
	fifo_inst->front += 1;
	fifo_inst->front &= (uint16_t)(fifo_inst->buf_size - 1); // Handle wrap-around
	fifo_inst->count -= 1;

	return val;
}

uint8_t fifo_read(fifo_t* fifo_inst, uint8_t* dest, uint16_t num_bytes)
{
	uint16_t to_wrap;
	uint8_t* read_ptr;
	uint16_t idx;

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

uint8_t fifo_write(fifo_t* fifo_inst, uint8_t* src, uint16_t num_bytes)
{
	uint16_t to_wrap;
	uint8_t* write_ptr;
	uint16_t idx;

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
