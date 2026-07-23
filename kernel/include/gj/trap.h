/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 */
#pragma once

#include <gj/types.h>

/* Stack frame built by isr stubs + common (reverse push order). */
struct gj_trap_frame {
    u64 u64R15;
    u64 u64R14;
    u64 u64R13;
    u64 u64R12;
    u64 u64R11;
    u64 u64R10;
    u64 u64R9;
    u64 u64R8;
    u64 u64Rbp;
    u64 u64Rdi;
    u64 u64Rsi;
    u64 u64Rdx;
    u64 u64Rcx;
    u64 u64Rbx;
    u64 u64Rax;
    u64 u64Vector;
    u64 u64Error;
    u64 u64Rip;
    u64 u64Cs;
    u64 u64Rflags;
    u64 u64Rsp;
    u64 u64Ss;
};

void trap_dispatch(struct gj_trap_frame *pFrame);
