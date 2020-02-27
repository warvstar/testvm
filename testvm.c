#include "testvm.h"
struct Op stack2[32];
struct Label Labels[128];
struct Label* cur_label;
void exe_stack2(int start, int end) {
    debug_printf("Starting stack (%d,%d)\n", start, end);
    for (int i = start; i < end; ++i) {
        stack2[i].func(&stack2[i], &i);
    }
}
int op_count = 0, highest_saved_locals_count = 0, highest_blocks_count = 0, highest_label_stack_ptr = 0;
void DebugPrint(const char* format, ...) {
#ifdef _DEBUG
    va_list argptr;
    va_start(argptr, format);
    vfprintf(stderr, format, argptr);
    va_end(argptr);
    debug_printf(" //stack[%d][", (int)cur_label->stack_ptr);
    for (int i = 0; i < cur_label->stack_ptr; ++i)
        debug_printf("%d ", cur_label->stack[i]);
    debug_printf("]\n");
#endif
}
inline int PushLocal() {
    //highest_stack_count = min(cur_label->stack_ptr, highest_stack_count);
    return (int)cur_label->stack_ptr++;
}
inline uint64_t PopStack() {
    return cur_label->stack[--cur_label->stack_ptr];
}
inline int PopLocal() {
    return (int)cur_label->stack_ptr++;
}
inline void PushStack(uint64_t v) {
    //highest_stack_count = min(cur_label->stack_ptr, highest_stack_count);
    cur_label->stack[cur_label->stack_ptr++] = v;
}
void SaveLocals(struct Label* label) {
    for (int i = 0; i < 4; ++i)
        label->saved_locals[label->saved_locals_count][i] = label->locals[i];
    label->saved_locals_count++;
    //highest_saved_locals_count = min(label->saved_locals_count, highest_saved_locals_count);
}
void RestoreLocals(struct Label* label) {
    label->saved_locals_count--;
    for (int i = 0; i < 4; ++i)
        label->locals[i] = label->saved_locals[label->saved_locals_count-1][i];
}
void SetLocal(struct Op* op) {
    cur_label->locals[op->a] = cur_label->stack[--cur_label->stack_ptr];
    debug_printf("SetLocal %d(%d)\n", op->a, cur_label->locals[op->a]);
}
void GetLocal(struct Op* op) {
    PushStack(cur_label->locals[op->a]);
    DebugPrint("GetLocal %d(%d)", (int)op->a, (int)cur_label->locals[op->a]);
}
void SetI32(struct Op* op) {
    int test = op->a;
    PushStack(op->a);
    DebugPrint("SetI32 %d", (int)op->a);
}
struct Label* label_stack[LOCALSN];
int label_stack_ptr = 0;
int exec_label2(int i, int a, int b) {
    cur_label = &Labels[i];
    cur_label->locals[0] = a;
    label_stack[label_stack_ptr++] = cur_label;
    //highest_label_stack_ptr = min(label_stack_ptr, highest_label_stack_ptr);
    SaveLocals(cur_label);
    exe_stack2(Labels[i].start, Labels[i].end);
    RestoreLocals(cur_label);
    uint64_t ret_local = cur_label->stack[cur_label->stack_ptr - 1];
    debug_printf("Function returned %d\n", ret_local);
    cur_label = label_stack[--label_stack_ptr];
    return (int)ret_local;
}
int exec_label1(int i, int a) {
    return exec_label2(i, a, cur_label->locals[1]);
}
int exec_label0(int i) {
    return exec_label2(i, cur_label->stack[--cur_label->stack_ptr], cur_label->locals[1]);
}
int working_label = 0;
void label(int i) {
    Labels[i].start = op_count;
    cur_label = &Labels[i];
    highest_blocks_count = min(cur_label->blocks_count, highest_blocks_count);
}
void endlabel() {
    cur_label->end = op_count;
    for (int i = 0; i < op_count; ++i)
        for (int n = 0; n < cur_label->blocks_count; ++n)
            if (cur_label->blocks[n] > i) { cur_label->block_dist[i] = cur_label->blocks[n]; break; }
}
static int p = 0;
void Call(struct Op* op) {
    exec_label0(op->a);
    debug_printf("Calling label %d\n", op->a);
    //if(p++ > BLOCKN)
    //exit(1);
}
void CallNative(struct Op* op) {
    /*debug_printf("Doing native call\n");
    int64_t ret;
    uint64_t _add = (uint64_t)op->a;
    uint64_t a1 = cur_label->stack[0];
    uint64_t a2 = cur_label->stack[1];
    uint64_t a3 = cur_label->stack[2];
    uint64_t a4 = cur_label->stack[3];
    _asm
    {
        mov rcx, a1;
        mov rdx, a2;
        mov r8, a3;
        mov r9, a4;
        call _add;
    }
    debug_printf("Call returned %d\n", ret);*/
}
void BranchL_Native(struct Op* op) {
    uint64_t b = cur_label->stack[--cur_label->stack_ptr];
    uint64_t a = cur_label->stack[--cur_label->stack_ptr];
    debug_printf("is a:%d lesser than b:%d\n", a, b);
    if (a < b)
        exec_label0(op->a);
    else
        exec_label0(op->b);
    /*_asm
    {
        mov rcx, a;
        cmp rcx, b;
        jl islesser;
        jmp greater;
    islesser:
        jmp lesser;
    }*/
    /*__asm__ __volatile__(
        "cmp $1, %%rax\n"
        "jl %%rax\n"
        "jmp %3"
        : : "r" (op->a), "r" (op->b), "r" (op->c), "r" (op->d)
        :
    );*/
}
void Ret(struct Op* op, int* i) {
    debug_printf("Ret called\n");
    *i = cur_label->end;
}
void Drop(struct Op* op) {
    cur_label->stack_ptr = 0;
}
void Block(struct Op* op) {

}
void I32_eqz(struct Op* op) {
    PushStack(PopStack() == 0);
    //DebugPrint("I32_eqz 0 == %d", (int)b);
}
void I32_eq(struct Op* op) {
    PushStack(PopStack() == PopStack());
    //DebugPrint("I32_eq %d == %d", (int)a, (int)b);
}
void I32_lt_s(struct Op* op) {
    PushStack(PopStack() > PopStack());
    //DebugPrint("I32_lt_s %d < %d", (int)a, (int)b);
}
void Select(struct Op* op) {
    PushStack(!PopStack() ? PopStack() : PopStack());
    //DebugPrint("Select (a:%d b:%d) %d", a, b, v);
}
void I32_gt_s(struct Op* op) {

}
void I32_ge_s(struct Op* op) {
    PushStack(PopStack() <= PopStack());
    //DebugPrint("I32_ge_s %d >= %d", (int)a, (int)b);
}
void End(struct Op* op) {
    debug_printf("BlockEnd\n");
}
void Br(struct Op* op) {

}
void Br_if(struct Op* op, int* i) {
    uint64_t b = cur_label->stack[--cur_label->stack_ptr];
    if (b)
        (*i) = cur_label->block_dist[*i] - 1;
}
void Add(struct Op* op) {
    PushStack(PopStack() + PopStack());
    //DebugPrint("Adding %d to %d", (int)a, (int)b);
}
void* func_array[] = { [SET_LOCAL] = SetLocal,[GET_LOCAL] = GetLocal,[CALL] = Call,[CALL_NATIVE] = CallNative,[BRANCHL] = BranchL_Native,[RET] = Ret,[ADD] = Add,[I32] = SetI32,[DROP] = Drop,[BLOCK] = Block,[I32_EQ] = I32_eq,[I32_LT_S] = I32_lt_s,[I32_GT_S] = I32_gt_s,[END] = End,[BR] = Br,
[BR_IF] = Br_if,[I32_EQZ] = I32_eqz,[SELECT] = Select,[I32_GE_S] = I32_ge_s };
void add_op2(int op, uint64_t a, uint64_t b) {
    struct Op* o = &stack2[op_count++];
    o->a = a; o->b = b;
    o->func = func_array[op];
    if (op == END)
        cur_label->blocks[cur_label->blocks_count++] = op_count;
}
void add_op1(int op, uint64_t a) {
    add_op2(op, a, 0);
}
void add_op0(int op) {
    add_op2(op, 0, 0);
}
//test against wasm3, awsm and lua
int main() {
    int N = 40;
    printf("N:%d\n", N);
    struct Label label2;
    test_native_fib(N);
    cur_label = &label2;
    test_fib(N);
    //debug_printf("highest_blocks_count:%d\n", highest_blocks_count); debug_printf("highest_saved_locals_count:%d\n", highest_saved_locals_count); debug_printf("highest_label_stack_ptr:%d\n", highest_label_stack_ptr);
}