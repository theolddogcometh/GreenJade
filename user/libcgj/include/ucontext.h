/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped ucontext.h (x86_64 bring-up). Not GNU glibc.
 * Integer GPRs only (no SSE/x87 state).
 */
#pragma once

#include <signal.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define NGREG 23

enum {
    REG_R8 = 0,
    REG_R9,
    REG_R10,
    REG_R11,
    REG_R12,
    REG_R13,
    REG_R14,
    REG_R15,
    REG_RDI,
    REG_RSI,
    REG_RBP,
    REG_RBX,
    REG_RDX,
    REG_RAX,
    REG_RCX,
    REG_RSP,
    REG_RIP,
    REG_EFL,
    REG_CSGSFS,
    REG_ERR,
    REG_TRAPNO,
    REG_OLDMASK,
    REG_CR2
};

typedef long greg_t;
typedef greg_t gregset_t[NGREG];

typedef struct {
    gregset_t gregs;
    /* FPU/SSE omitted under -mgeneral-regs-only */
    unsigned long __reserved[8];
} mcontext_t;

#ifndef _CGJ_STACK_T_DEFINED
#define _CGJ_STACK_T_DEFINED
typedef struct {
    void  *ss_sp;
    int    ss_flags;
    size_t ss_size;
} stack_t;
#endif

typedef struct ucontext {
    unsigned long      uc_flags;
    struct ucontext   *uc_link;
    stack_t            uc_stack;
    mcontext_t         uc_mcontext;
    sigset_t           uc_sigmask;
} ucontext_t;

int  getcontext(ucontext_t *pUc);
int  setcontext(const ucontext_t *pUc);
void makecontext(ucontext_t *pUc, void (*pfn)(void), int nArg, ...);
int  swapcontext(ucontext_t *pOuc, const ucontext_t *pUc);

#ifdef __cplusplus
}
#endif
