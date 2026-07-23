/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8181: setjmp env-present predicate stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_setjmp_env_present_ok_8181(const void *env);
 *     - Return 1 if env is non-NULL, else 0.
 *   uint32_t __gj_setjmp_env_present_ok_8181  (alias)
 *   __libcgj_batch8181_marker = "libcgj-batch8181"
 *
 * Exclusive continuum CREATE-ONLY (8181-8190: setjmp/longjmp stubs —
 * env_present_ok, ret_zero_ok, longjmp_norm_val, savemask_ok,
 * sig_vs_plain_id, jmp_buf_words, nest_depth_ok, longjmp_nonzero,
 * params_ok, continuum + batch_id_8190). Unique
 * gj_setjmp_env_present_ok_8181 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8181_marker[] = "libcgj-batch8181";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8181_env_present(const void *pEnv)
{
	if (pEnv != NULL)
		return 1u;
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_setjmp_env_present_ok_8181 - 1 if setjmp/longjmp env pointer is set.
 *
 * env: opaque jmp_buf / sigjmp_buf pointer (or NULL)
 *
 * Returns 1 when env is non-NULL, else 0. Soft presence gate for
 * setjmp/longjmp call sites; does not call libc setjmp. No parent wires.
 */
uint32_t
gj_setjmp_env_present_ok_8181(const void *pEnv)
{
	(void)NULL;
	return b8181_env_present(pEnv);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_setjmp_env_present_ok_8181(const void *pEnv)
    __attribute__((alias("gj_setjmp_env_present_ok_8181")));
