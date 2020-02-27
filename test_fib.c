#include "testvm.h"
uint64_t f(uint64_t n)
{
    //debug_printf("f:%d\n", n);
    if (n < 2)
        return n;
    else {
        uint64_t i = f(n - 1);
        uint64_t i2 = f(n - 2);
        return (i + i2); //1 1
    }
}
//tail recursion fib
int fib(int term, int val, int prev) {
    if (term == 0) return prev;
    return fib(term - 1, val + prev, val);
}
void test_fib(int N) {
    label(0);
    add_op0(BLOCK);
    add_op1(GET_LOCAL, 0);
    add_op1(I32, 2);
    add_op0(I32_GE_S);
    add_op1(BR_IF, 0);
    add_op1(GET_LOCAL, 0);
    add_op0(RET);
    add_op0(END);
    add_op1(GET_LOCAL, 0);
    add_op1(I32, -1);
    add_op0(ADD);
    add_op1(CALL, 0);
    add_op1(GET_LOCAL, 0);
    add_op1(I32, -2);
    add_op0(ADD); //1 0
    add_op1(CALL, 0); //returns 0
    add_op0(ADD); //stack should be 1 0
    add_op0(END);
    endlabel();
    clock_t t;
    t = clock();
    int F = exec_label1(0, N);
    t = clock() - t;
    double time_taken = ((double)t) / CLOCKS_PER_SEC; // in seconds 

    printf("F2:%d %fs\n", F, time_taken);
}
void test_native_fib(int N) {
    clock_t t;
    t = clock();
    uint64_t F = f(N);
    t = clock() - t;
    double time_taken = ((double)t) / CLOCKS_PER_SEC; // in seconds 
    printf("F:%"PRIu64" %fs\n", F, time_taken);
    /*{
        clock_t t;
        t = clock();
        uint64_t F = fib(N, 1, 0);
        t = clock() - t;
        double time_taken = ((double)t) / CLOCKS_PER_SEC; // in seconds
        //unsigned long long pid = GetCurrentProcessId();
        printf("F:%"PRIu64" %fs\n", F, time_taken);
    }*/
}