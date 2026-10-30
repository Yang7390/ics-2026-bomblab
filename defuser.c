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


typedef struct Node Node;
struct Node {
    Node* next;
    uint32_t val1;
    uint32_t val2;
};

static Node n0 = {NULL, 0xa40, 0xba3d};
static Node n1 = {NULL, 0x753, 0x7873};
static Node n2 = {NULL, 0x814, 0xb146};
static Node n3 = {NULL, 0x543, 0xb816};
static Node n4 = {NULL, 0x7c,  0xc4c3};
static Node n5 = {NULL, 0x97c, 0xdbf0};
static Node n6 = {NULL, 0x30f, 0xa0a9};
static Node n7 = {NULL, 0xb14, 0x2e93};

static void build_link(void)
{
    n0.next = &n1;
    n1.next = &n2;
    n2.next = &n3;
    n3.next = &n4;
    n4.next = &n5;
    n5.next = &n6;
    n6.next = &n7;
    n7.next = NULL;
}

static Node* const p5_heads[8] = {
    &n0,
    &n1,
    &n2,
    &n3,
    &n4,
    &n5,
    &n6,
    &n7,
};

static uint32_t p5_traverse(uint64_t challenge, uint32_t *out_last_val)
{
    uint32_t idx   = challenge & 0x7U;
    uint32_t steps = (challenge >> 3) & 0x3U;

    Node *p = p5_heads[idx];


    uint32_t check = ( (challenge >> 6) & 0xFFFFFFFFU )
                   ^ ( (challenge >> 32) & 0xFFFFFFFFU )
                   ^ 0xeb6U;


    check = check * 3 + p->val2;
    *out_last_val = p->val1;
    check ^= p->val1;

    if (steps == 0) {
        return check;
    }


    uint32_t i = 1;
    while (1)
    {
        p = p->next;
        check = check * 3 + p->val2;
        *out_last_val = p->val1;
        check ^= p->val1;

        if (i >= steps)
            break;
        i++;
    }
    return check;
}

int solve_phase_5(uint64_t challenge, char *out, size_t cap)
{
    build_link();
    uint32_t ans1, ans2;
    ans1 = p5_traverse(challenge, &ans2);

    uint32_t vals[2] = {ans1, ans2};
    return write_answer(out, cap, vals, 2);
}

static inline uint32_t rol32(uint32_t x, int n) {
    return (x << n) | (x >> (32 - n));
}
static inline uint32_t ror32(uint32_t x, int n) {
    return (x >> n) | (x << (32 - n));
}

static uint32_t op0(uint32_t x) {
    uint32_t rax = x ^ 0xdad3;
    rax = rax + x * 4;
    return rax;
}
static uint32_t op1(uint32_t x) {
    uint32_t rax = rol32(x, 3);
    rax ^= 0xfac;
    return rax;
}
static uint32_t op2(uint32_t x) {
    uint32_t rdx = x * 4;
    uint32_t eax = rdx + 0x6dc0;
    rdx += x;
    eax ^= rdx;
    return eax;
}
static uint32_t op3(uint32_t x) {
    uint32_t rax = x >> 1;
    rax ^= x;
    rax += 0x23db;
    return rax;
}
static uint32_t op4(uint32_t x) {
    uint32_t eax = x + 0x8c30;
    uint32_t edi = x >> 3;
    eax ^= edi;
    return eax;
}
static uint32_t op5(uint32_t x) {
    uint32_t rax = ror32(x, 7);
    return rax;
}

static uint32_t (*op_table[8])(uint32_t) = {
    op2, op4, op0, op3, op5, op0, op1, op4
};

static uint32_t p6_calc(uint64_t challenge)
{
    uint32_t x0 = (uint32_t)((challenge >> 6) ^ (challenge >> 32));

    int idx1 = challenge & 0x7;
    int idx2 = (challenge & 0x38) >> 3;

    uint32_t r1 = op_table[idx1](x0);
    uint32_t r2 = r1 ^ 0x1940;
    uint32_t r3 = op_table[idx2](r2);
    uint32_t final = r3 + 0x838;
    return final;
}

int solve_phase_6(uint64_t challenge, char *out, size_t cap)
{
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 1;

    values[0] = p6_calc(challenge);
    return write_answer(out, cap, values, count);
}

static uint32_t secret_mix(uint32_t x)
{
    uint32_t rol1 = (x << 1) | (x >> 31);
    return (rol1 + 0xd597) ^ 0xd9ed;
}

int solve_secret(uint64_t challenge, uint64_t context, char *out, size_t cap)
{
    // r12 = context, r13 = challenge
    uint32_t c   = challenge & 0x3;
    uint32_t x   = (uint32_t)(context >> 32) ^ (uint32_t)context;
    uint32_t y   = (uint32_t)(challenge >> 32) ^ (uint32_t)challenge;
    uint32_t b16 = (uint32_t)(challenge >> 16);
    uint32_t r9  = (c * 0xd597) ^ b16;
    uint32_t arg_mix = x ^ y;

    uint32_t v1 = c;
    uint32_t mix_ret = secret_mix(arg_mix);
    uint32_t v2 = mix_ret ^ r9;
    uint32_t edx = y + c + 0xd9ed + (x >> 1);
    uint32_t v3 = v2 ^ edx;

    // 精确32字节空格填充，OPEN正好落在缓冲区偏移32字节处，匹配魔数校验
    snprintf(out, cap, "%32sOPEN %u %u %u", "", v1, v2, v3);
    return 0;
}
