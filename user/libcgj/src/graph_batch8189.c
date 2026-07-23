/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8189: setjmp/longjmp params-ok gate stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_setjmp_params_ok_8189(const void *env, int val);
 *     - Return 1 if env is non-NULL (val may be any int), else 0.
 *   uint32_t __gj_setjmp_params_ok_8189  (alias)
 *   __libcgj_batch8189_marker = "libcgj-batch8189"
 *
 * Exclusive continuum CREATE-ONLY (8181-8190: setjmp/longjmp stubs —
 * env_present_ok, ret_zero_ok, longjmp_norm_val, savemask_ok,
 * sig_vs_plain_id, jmp_buf_words, nest_depth_ok, longjmp_nonzero,
 * params_ok, continuum + batch_id_8190). Unique
 * gj_setjmp_params_ok_8189 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8189_marker[] = "libcgj-batch8189";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8189_params_ok(const void *pEnv, int nVal)
{
	(void)nVal;
	if (pEnv != NULL)
		return 1u;
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_setjmp_params_ok_8189 - combined env/val gate for longjmp-like calls.
 *
 * env: opaque jmp_buf pointer (must be non-NULL)
 * val: longjmp return code (any int accepted; 0 will be normalized later)
 *
 * Returns 1 when env is non-NULL, else 0. Soft combined param gate;
 * does not call libc. No parent wires.
 */
uint32_t
gj_setjmp_params_ok_8189(const void *pEnv, int nVal)
{
	(void)NULL;
	return b8189_params_ok(pEnv, nVal);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_setjmp_params_ok_8189(const void *pEnv, int nVal)
    __attribute__((alias("gj_setjmp_params_ok_8189")));
