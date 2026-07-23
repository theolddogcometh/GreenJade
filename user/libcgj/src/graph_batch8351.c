/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8351: errno success (0 is ok) predicate stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_errno_ok_u_8351(uint32_t e);
 *     - Return 1 when e == 0 (no error / success), else 0.
 *   uint32_t __gj_errno_ok_u_8351  (alias)
 *   __libcgj_batch8351_marker = "libcgj-batch8351"
 *
 * Exclusive continuum CREATE-ONLY (8351-8360: signal/errno id stubs —
 * errno_ok_u, errno_inval_id, errno_nomem_id, errno_again_id,
 * sig_valid_u, sig_term_id, sig_int_id, sig_mask_empty_u,
 * sig_mask_full_u, batch_id_8360). Unique gj_errno_ok_u_8351 surface
 * only; no multi-def. No real signal API. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8351_marker[] = "libcgj-batch8351";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8351_errno_ok(uint32_t u32E)
{
	if (u32E == 0u) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_errno_ok_u_8351 - true when the errno-like code is success (0).
 *
 * e: candidate errno / status code (unsigned view)
 *
 * Returns 1 if e == 0, else 0. Soft catalog predicate; does not touch
 * thread-local errno. No parent wires.
 */
uint32_t
gj_errno_ok_u_8351(uint32_t u32E)
{
	(void)NULL;
	return b8351_errno_ok(u32E);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_errno_ok_u_8351(uint32_t u32E)
    __attribute__((alias("gj_errno_ok_u_8351")));
