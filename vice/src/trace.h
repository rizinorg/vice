
#ifndef _TRACE_H
#define _TRACE_H

#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct trace_regs_t {
	uint8_t a, x, y, sp;
	uint16_t pc;
	uint8_t sr;
} TraceRegs;

typedef struct trace_mem_t {
	uint16_t addr;
	uint8_t val;
} TraceMem;

#define TRACE_MEM_MAX 0x10

typedef struct trace_operands_t {
	TraceRegs regs;
	TraceMem mems[TRACE_MEM_MAX];
	size_t mems_count;
} TraceOperands;

typedef struct trace_frame_t {
	uint8_t op[3];
	size_t op_size;
	TraceOperands pre;
	TraceOperands post;
} TraceFrame;

int trace_open(const char *filename);
void trace_close(void);
int trace_is_open(void);
void trace_push(TraceFrame *tf);

extern TraceFrame build_frame;

#ifdef __cplusplus
}
#endif

#endif
