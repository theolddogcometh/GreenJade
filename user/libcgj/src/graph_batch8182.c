/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8182: setjmp first-return zero predicate stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_setjmp_ret_zero_ok_8182(uint32_t ret);
 *     - Return 1 if ret is 0 (setjmp first-return path), else 0.
 *   uint32_t __gj_setjmp_ret_zero_ok_8182  (alias)
 *   __libcgj_batch8182_marker = "libcgj-batch8182"
 *
 * Exclusive continuum CREATE-ONLY (8181-8190: setjmp/longjmp stubs —
 * env_present_ok, ret_zero_ok, longjmp_norm_val, savemask_ok,
 * sig_vs_plain_id, jmp_buf_words, nest_depth_ok, longjmp_nonzero,
 * params_ok, continuum + batch_id_8190). Unique
 * gj_setjmp_ret_zero_ok_8182 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8182_marker[] = "libcgj-batch8182";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8182_ret_zero(uint32_t u32Ret)
{
	if (u32Ret == 0u)
		return 1u;
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_setjmp_ret_zero_ok_8182 - 1 if setjmp return is the first-call zero.
 *
 * ret: value observed from a setjmp-like call
 *
 * Returns 1 when ret == 0 (initial setjmp return), else 0. Soft pure
 * path tag; does not call libc setjmp. No parent wires.
 */
uint32_t
gj_setjmp_ret_zero_ok_8182(uint32_t u32Ret)
{
	(void)NULL;
	return b8182_ret_zero(u32Ret);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_setjmp_ret_zero_ok_8182(uint32_t u32Ret)
    __attribute__((alias("gj_setjmp_ret_zero_ok_8182")));
