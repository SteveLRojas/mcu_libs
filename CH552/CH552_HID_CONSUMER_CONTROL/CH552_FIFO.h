#ifndef _CH552_FIFO_H_
#define _CH552_FIFO_H_

typedef struct _FIFO {
	UINT16 buf_size;
	UINT8* pbuf;
	UINT16 count;
	UINT16 front;
	UINT16 back;
} fifo_t;

#define fifo_num_used(fifo_inst) ((fifo_inst)->count)
#define fifo_num_free(fifo_inst) ((fifo_inst)->buf_size - (fifo_inst)->count)
#define fifo_empty(fifo_inst) ((fifo_inst)->count == 0)
#define fifo_full(fifo_inst) ((fifo_inst)->count >= (fifo_inst)->buf_size)
#define fifo_peek(fifo_inst) ((fifo_inst)->pbuf[(fifo_inst)->front])

// HINT: fifo_size must be a power of 2
void fifo_init(fifo_t* fifo_inst, UINT8* fifo_buf, UINT16 fifo_size);
UINT8 fifo_push(fifo_t* fifo_inst, UINT8 val) reentrant;
UINT8 fifo_pop(fifo_t* fifo_inst) reentrant;
UINT8 fifo_read(fifo_t* fifo_inst, UINT8* dest, UINT16 num_bytes);
UINT8 fifo_write(fifo_t* fifo_inst, UINT8* src, UINT16 num_bytes);

#endif
