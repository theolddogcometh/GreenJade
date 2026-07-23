/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8187: setjmp nest-depth predicate stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_setjmp_nest_depth_ok_8187(uint32_t depth);
 *     - Return 1 if depth is in [1, 64], else 0.
 *   uint32_t __gj_setjmp_nest_depth_ok_8187  (alias)
 *   __libcgj_batch8187_marker = "libcgj-batch8187"
 *
 * Exclusive continuum CREATE-ONLY (8181-8190: setjmp/longjmp stubs —
 * env_present_ok, ret_zero_ok, longjmp_norm_val, savemask_ok,
 * sig_vs_plain_id, jmp_buf_words, nest_depth_ok, longjmp_nonzero,
 * params_ok, continuum + batch_id_8190). Unique
 * gj_setjmp_nest_depth_ok_8187 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8187_marker[] = "libcgj-batch8187";

#define B8187_DEPTH_MIN  1u
#define B8187_DEPTH_MAX  64u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8187_depth_ok(uint32_t u32Depth)
{
	if (u32Depth >= B8187_DEPTH_MIN && u32Depth <= B8187_DEPTH_MAX)
		return 1u;
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_setjmp_nest_depth_ok_8187 - 1 if nested setjmp depth is plausible.
 *
 * depth: soft nest level counter (1 = single active setjmp frame)
 *
 * Returns 1 when depth is in [1, 64], else 0. Soft bound check only;
 * does not call libc. No parent wires.
 */
uint32_t
gj_setjmp_nest_depth_ok_8187(uint32_t u32Depth)
{
	(void)NULL;
	return b8187_depth_ok(u32Depth);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_setjmp_nest_depth_ok_8187(uint32_t u32Depth)
    __attribute__((alias("gj_setjmp_nest_depth_ok_8187")));
