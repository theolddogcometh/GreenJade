/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped setjmp.h (x86_64). Not GNU glibc.
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* rbx, rbp, r12-r15, rsp, rip */
typedef long jmp_buf[8];
typedef long sigjmp_buf[8];

int  setjmp(jmp_buf env) __attribute__((returns_twice));
int  _setjmp(jmp_buf env) __attribute__((returns_twice));
void longjmp(jmp_buf env, int nVal) __attribute__((noreturn));
void __longjmp_chk(jmp_buf env, int nVal) __attribute__((noreturn));
int  sigsetjmp(sigjmp_buf env, int nSavemask) __attribute__((returns_twice));
int  __sigsetjmp(sigjmp_buf env, int nSavemask) __attribute__((returns_twice));
void siglongjmp(sigjmp_buf env, int nVal) __attribute__((noreturn));

#ifdef __cplusplus
}
#endif
