/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room <linux/filter.h> for libcgj. OpenSSH sandbox-seccomp-filter
 * (HAVE_LINUX_FILTER_H) needs sock_filter / sock_fprog and BPF_* macros.
 * ABI numbers match Linux classic BPF. Dual DoD B OPEN.
 */
#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct sock_filter {
    uint16_t code;
    uint8_t  jt;
    uint8_t  jf;
    uint32_t k;
};

struct sock_fprog {
    unsigned short      len;
    struct sock_filter *filter;
};

#ifndef BPF_LD
#define BPF_LD  0x00
#define BPF_LDX 0x01
#define BPF_ST  0x02
#define BPF_STX 0x03
#define BPF_ALU 0x04
#define BPF_JMP 0x05
#define BPF_RET 0x06
#define BPF_MISC 0x07
#endif

#ifndef BPF_W
#define BPF_W   0x00
#define BPF_H   0x08
#define BPF_B   0x10
#endif

#ifndef BPF_IMM
#define BPF_IMM 0x00
#define BPF_ABS 0x20
#define BPF_IND 0x40
#define BPF_MEM 0x60
#define BPF_LEN 0x80
#define BPF_MSH 0xa0
#endif

#ifndef BPF_ADD
#define BPF_ADD 0x00
#define BPF_SUB 0x10
#define BPF_MUL 0x20
#define BPF_DIV 0x30
#define BPF_OR  0x40
#define BPF_AND 0x50
#define BPF_LSH 0x60
#define BPF_RSH 0x70
#define BPF_NEG 0x80
#define BPF_MOD 0x90
#define BPF_XOR 0xa0
#endif

#ifndef BPF_JA
#define BPF_JA  0x00
#define BPF_JEQ 0x10
#define BPF_JGT 0x20
#define BPF_JGE 0x30
#define BPF_JSET 0x40
#endif

#ifndef BPF_K
#define BPF_K 0x00
#define BPF_X 0x08
#endif

#ifndef BPF_STMT
#define BPF_STMT(nCode, nK) \
    { (unsigned short)(nCode), 0, 0, (nK) }
#endif
#ifndef BPF_JUMP
#define BPF_JUMP(nCode, nK, nJt, nJf) \
    { (unsigned short)(nCode), (nJt), (nJf), (nK) }
#endif

#ifdef __cplusplus
}
#endif
