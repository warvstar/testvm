#pragma once
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <time.h>
enum ops { SET_LOCAL, GET_LOCAL, CALL, CALL_NATIVE, BRANCHL, ADD, RET, COPY_LOCAL, I32, DROP, BLOCK, BR_IF, BR, I32_EQ, I32_LT_S, I32_GT_S, SELECT, END, I32_EQZ, I32_GE_S };
struct Op { void(*func)(struct Op*, int* i); uint64_t a, b; };
#define debug_printf
//#define debug_printf printf
#define LOCALSN 40
#define min(x, y) (((x) > (y)) ? (x) : (y)) //actually max, just whatever
struct BlockType {
    int start, end;
};
struct Label { int start, end; uint64_t stack[LOCALSN]; uint64_t locals[LOCALSN]; uint64_t saved_locals[LOCALSN][4]; uint64_t saved_locals_count; uint64_t stack_ptr;  uint16_t blocks[20]; uint64_t blocks_count; uint16_t block_dist[8]; };
