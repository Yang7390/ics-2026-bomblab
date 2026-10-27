#include "defuser.h"

#include <inttypes.h>
#include <stdio.h>

#define MAX_FIELDS 8

/*
 * 把 values 中的 count 个整数写成一行“空格分隔的无符号十进制整数”，存入 out。
 * 成功返回 0；count 为 0（还没有实现）、snprintf 出错或结果被截断时返回非零。
 */
static int write_answer(char *out, size_t cap, const uint32_t *values, size_t count) {
    size_t used = 0;
    for (size_t i = 0; i < count; ++i) {
        int size = snprintf(out + used, cap - used, "%s%" PRIu32, i ? " " : "", values[i]);
        if (size < 0 || (size_t)size >= cap - used) return 1;
        used += (size_t)size;
    }
    return count == 0;
}


static uint32_t p1_step2(uint32_t a, uint32_t b){
    return (a + b * 8) ^ 0xf153;
}

static uint32_t p1_step0(uint32_t a, uint32_t b){
    return (a * 3) ^ (b + 0x82c3);
}

static uint32_t p1_step1(uint32_t a, uint32_t b){
    return (b ^ 0x310b) + a * 4;
}

int solve_phase_1(uint64_t challenge, char *out, size_t cap) {
    uint32_t high32 = (uint32_t)(challenge >> 32);
    uint32_t low32  = (uint32_t)(challenge & 0xFFFFFFFF);
    uint32_t shift16 = (uint32_t)(challenge >> 16); 

    uint32_t res_step2 = p1_step2(high32, low32);
    uint32_t res_step0 = p1_step0(low32, shift16);
    uint32_t ans = p1_step1(res_step0, res_step2);

    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;
    values[0] = ans;
    count = 1;
    return write_answer(out, cap, values, count);
}



int solve_phase_2(uint64_t challenge, char *out, size_t cap) {
    (void)challenge;
    // TODO: 根据 phase_02 的汇编，用 challenge 算出口令中的各个整数，依次存入 values，并把 count 改为整数个数。
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 6; 
    values[0] = (uint32_t)(((challenge >> 47) & 0xFF) + 0x17);
    for (int i = 1; i < 6; i++) {
        uint32_t prev = values[i-1];   
        uint32_t prev_x5 = prev * 5;   
        uint32_t calc = (uint32_t)((challenge >> (i + 2)) & 0x3F);
        calc += i;                      
        calc ^= prev_x5;                

        values[i] = calc;
    }

    return write_answer(out, cap, values, count);
}

static uint32_t p3_dispatch(uint32_t a,uint32_t b){
    if (a>7){
        return 0;
    }
    switch(a){
        case 0:  return 0x13768 + b * 5;
        case 1:  return (b * 4) ^ 0x894d;
        case 2:  return 0x25d6 + b * 3;
        case 3:  return (b ^ 0x40eb) + (b >> 1);
        case 4:  return (b ^ 0x7152) + (b >> 1);
        case 5:  return 0x2ef90 + b * 5;
        case 6:  return (b * 2) ^ 0xb01e;
        case 7:  return 0x1772 + b * 5;
        default: return 0;
    }
}             
int solve_phase_3(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 2;
    uint32_t num1=challenge & 0x7;
    uint32_t val=(uint32_t)(challenge >> 3)^(uint32_t)(challenge >>32);
    uint32_t num2=p3_dispatch(num1,val);
    values[0]=num1;
    values[1]=num2;
    return write_answer(out, cap, values, count);
}

static uint32_t p4_walk(uint32_t b, uint32_t c, uint32_t d, uint32_t e)
{
    uint32_t a = d + (c - d) / 2;
    if (a == b) {
        return (0xf7 + a) ^ e;
    }

    uint32_t temp = a + e;
    if (a < b) {
        uint32_t res = p4_walk(b, c, a + 1, e);
        res = res * 5;
        res ^=temp;
        res ^= 0x1a;
        return res;
    } else {
        uint32_t res = p4_walk(b, a - 1, d, e);
        res = res * 5;
        res ^= temp;
        res ^= 0xa;
        return res;
    }
}

int solve_phase_4(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 1;

    uint32_t high32 = (uint32_t)(challenge >> 32);
    uint32_t high57 = (uint32_t)(challenge >> 7) ^ high32;
    uint32_t m = challenge & 0x7f;
    uint32_t n = 0x7f;
    uint32_t t = 0;

    values[0] = p4_walk(m, n, t, high57);
    return write_answer(out, cap, values, count);
}

struct Node {
    struct Node* next;
    uint32_t val1;
    uint32_t val2;
};
extern struct Node* node_array[8];

static uint32_t p5_walk(uint64_t val, uint32_t *out)
{
    uint32_t idx = val & 0x7;
    uint32_t steps = (val >> 3) & 0x3;
    struct Node* cur = node_array[idx];

    uint32_t check = (uint32_t)(val >> 6);
    check ^= (uint32_t)(val >> 32);
    check ^= 0xeb6;

    check = check * 3 + cur->val2;
    uint32_t v1 = cur->val1;
    check ^= v1;
    *out = v1;

    for (uint32_t i = 0; i < steps; i++) {
        cur = cur->next;
        check = check * 3 + cur->val2;
        v1 = cur->val1;
        check ^= v1;
        *out = v1;
    }
    return check;
}

int solve_phase_5(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 2;

    uint32_t last_val;
    uint32_t check = p5_walk(challenge, &last_val);

    values[0] = check;
    values[1] = last_val;

    return write_answer(out, cap, values, count);
}

int solve_phase_6(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;
    (void)challenge;
    // TODO: 根据 phase_06 的汇编，用 challenge 算出口令中的各个整数，依次存入 values，并把 count 改为整数个数。
    return write_answer(out, cap, values, count);
}

int solve_secret(uint64_t challenge, uint64_t context, char *out, size_t cap) {
    (void)challenge;
    (void)context;
    (void)out;
    (void)cap;
    // TODO（选做）: 根据 secret_gate、secret_copy 和 secret_phase 的汇编构造完整的第七行，写入 out 后返回 0。
    // 第七行不一定只有整数，可以直接用 snprintf 写入；还没完成时保持返回 1，测试会显示 not attempted。
    return 1;
}
