/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped <setjmp.h> for libcgj (GreenJade freestanding libc).
 * Not GNU glibc source; dual MIT OR Apache-2.0 only.
 *
 * Scope
 * -----
 * setjmp/longjmp and sigsetjmp/siglongjmp. Product asm is arch-specific
 * (x86_64 bring-up first); jmp_buf size must match the saved register set.
 *
 * Design notes
 * ------------
 * sigsetjmp may not save the signal mask in early bring-up — document callers
 * should not rely on mask restore until wired. Pure C cannot implement this
 * portably; see libcgj src for asm.
 *
 * Non-goals
 * ---------
 * C++ exception personality integration.
 *
 * Soft residual lean (this unit; Soft!=product; G-AC-1; Dual DoD A/B OPEN):
 *   - jmp_buf shape + x86_64 asm bring-up; sigmask restore soft residual
 *   - Soft!=product: libcgj setjmp surface != UDX/DDI product close
 *   - product = UDX/DDI + hot/cold Linux ABI; freestanding rtl/USB SKIP
 *   - agent PASS != close Dual DoD; no stamp storms; bar v2026.08.04.75 parent-only
 *   - dual MIT OR Apache-2.0; no GPL; no Linux .ko product AC (G-AC-1)
 * Grep: libcgj setjmp.h: soft residual lean Soft!=product
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
