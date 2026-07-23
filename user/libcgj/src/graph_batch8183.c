/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8183: longjmp value normalize stub.
 *
 * Surface (unique symbols):
 *   int gj_longjmp_norm_val_8183(int val);
 *     - Return val if nonzero, else 1 (POSIX longjmp rule).
 *   int __gj_longjmp_norm_val_8183  (alias)
 *   __libcgj_batch8183_marker = "libcgj-batch8183"
 *
 * Exclusive continuum CREATE-ONLY (8181-8190: setjmp/longjmp stubs —
 * env_present_ok, ret_zero_ok, longjmp_norm_val, savemask_ok,
 * sig_vs_plain_id, jmp_buf_words, nest_depth_ok, longjmp_nonzero,
 * params_ok, continuum + batch_id_8190). Unique
 * gj_longjmp_norm_val_8183 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8183_marker[] = "libcgj-batch8183";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b8183_norm_val(int nVal)
{
	if (nVal == 0)
		return 1;
	return nVal;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_longjmp_norm_val_8183 - normalize longjmp second argument.
 *
 * val: requested longjmp return code
 *
 * Returns val when nonzero, else 1. Mirrors the POSIX rule that
 * longjmp(..., 0) becomes 1 at the matching setjmp. Soft pure
 * normalize; does not call libc longjmp. No parent wires.
 */
int
gj_longjmp_norm_val_8183(int nVal)
{
	(void)NULL;
	return b8183_norm_val(nVal);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_longjmp_norm_val_8183(int nVal)
    __attribute__((alias("gj_longjmp_norm_val_8183")));
