/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8188: longjmp value nonzero predicate stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_longjmp_val_nonzero_u_8188(int val);
 *     - Return 1 if val is nonzero, else 0.
 *   uint32_t __gj_longjmp_val_nonzero_u_8188  (alias)
 *   __libcgj_batch8188_marker = "libcgj-batch8188"
 *
 * Exclusive continuum CREATE-ONLY (8181-8190: setjmp/longjmp stubs —
 * env_present_ok, ret_zero_ok, longjmp_norm_val, savemask_ok,
 * sig_vs_plain_id, jmp_buf_words, nest_depth_ok, longjmp_nonzero,
 * params_ok, continuum + batch_id_8190). Unique
 * gj_longjmp_val_nonzero_u_8188 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8188_marker[] = "libcgj-batch8188";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8188_nonzero(int nVal)
{
	if (nVal != 0)
		return 1u;
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_longjmp_val_nonzero_u_8188 - 1 if longjmp value is already nonzero.
 *
 * val: requested longjmp return code before normalize
 *
 * Returns 1 when val != 0, else 0. Complements longjmp_norm_val;
 * soft pure predicate. Does not call libc. No parent wires.
 */
uint32_t
gj_longjmp_val_nonzero_u_8188(int nVal)
{
	(void)NULL;
	return b8188_nonzero(nVal);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_longjmp_val_nonzero_u_8188(int nVal)
    __attribute__((alias("gj_longjmp_val_nonzero_u_8188")));
