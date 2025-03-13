#ifndef _FIFO_H_
#define _FIFO_H_

typedef struct _FIFO {
	uint16_t buf_size;
	uint8_t* pbuf;
	uint16_t count;
	uint16_t front;
	uint16_t back;
} fifo_t;

#define fifo_num_used(fifo_inst) ((fifo_inst)->count)
#define fifo_num_free(fifo_inst) ((fifo_inst)->buf_size - (fifo_inst)->count)
#define fifo_empty(fifo_inst) ((fifo_inst)->count == 0)
#define fifo_full(fifo_inst) ((fifo_inst)->count >= (fifo_inst)->buf_size)
#define fifo_peek(fifo_inst) ((fifo_inst)->pbuf[(fifo_inst)->front])

void fifo_init(fifo_t* fifo_inst, uint8_t* fifo_buf, uint16_t fifo_size);
uint8_t fifo_push(fifo_t* fifo_inst, uint8_t val);
uint8_t fifo_pop(fifo_t* fifo_inst);
uint8_t fifo_read(fifo_t* fifo_inst, uint8_t* dest, uint16_t num_bytes);
uint8_t fifo_write(fifo_t* fifo_inst, uint8_t* src, uint16_t num_bytes);

#endif
