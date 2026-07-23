/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8184: sigsetjmp savemask flag predicate stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sigsetjmp_savemask_ok_8184(int savemask);
 *     - Return 1 if savemask is 0 or 1, else 0.
 *   uint32_t __gj_sigsetjmp_savemask_ok_8184  (alias)
 *   __libcgj_batch8184_marker = "libcgj-batch8184"
 *
 * Exclusive continuum CREATE-ONLY (8181-8190: setjmp/longjmp stubs —
 * env_present_ok, ret_zero_ok, longjmp_norm_val, savemask_ok,
 * sig_vs_plain_id, jmp_buf_words, nest_depth_ok, longjmp_nonzero,
 * params_ok, continuum + batch_id_8190). Unique
 * gj_sigsetjmp_savemask_ok_8184 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8184_marker[] = "libcgj-batch8184";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8184_savemask_ok(int nSavemask)
{
	if (nSavemask == 0 || nSavemask == 1)
		return 1u;
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sigsetjmp_savemask_ok_8184 - 1 if savemask is a boolean 0/1 flag.
 *
 * savemask: sigsetjmp second argument (whether to save signal mask)
 *
 * Returns 1 for 0 or 1, else 0. Soft flag gate; does not call libc
 * sigsetjmp. No parent wires.
 */
uint32_t
gj_sigsetjmp_savemask_ok_8184(int nSavemask)
{
	(void)NULL;
	return b8184_savemask_ok(nSavemask);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sigsetjmp_savemask_ok_8184(int nSavemask)
    __attribute__((alias("gj_sigsetjmp_savemask_ok_8184")));
