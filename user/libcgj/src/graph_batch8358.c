/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8358: soft signal-mask empty predicate stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sig_mask_empty_u_8358(uint32_t mask);
 *     - Return 1 if soft 32-bit mask word is empty (0), else 0.
 *   uint32_t __gj_sig_mask_empty_u_8358  (alias)
 *   __libcgj_batch8358_marker = "libcgj-batch8358"
 *
 * Exclusive continuum CREATE-ONLY (8351-8360: signal/errno id stubs —
 * errno_ok_u, errno_inval_id, errno_nomem_id, errno_again_id,
 * sig_valid_u, sig_term_id, sig_int_id, sig_mask_empty_u,
 * sig_mask_full_u, batch_id_8360). Unique gj_sig_mask_empty_u_8358
 * surface only; no multi-def. No real signal API (no sigemptyset /
 * sigprocmask). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8358_marker[] = "libcgj-batch8358";

/* Soft empty mask word (local; no signal.h / sigset_t). */
#define B8358_MASK_EMPTY  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8358_mask_empty(uint32_t u32Mask)
{
	if (u32Mask == B8358_MASK_EMPTY) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sig_mask_empty_u_8358 - 1 if soft mask word is empty.
 *
 * mask: soft 32-bit signal-mask word (catalog view; not sigset_t)
 *
 * Returns 1 when mask is 0, else 0. Integer catalog test only; does not
 * call sigemptyset/sigprocmask. No parent wires.
 */
uint32_t
gj_sig_mask_empty_u_8358(uint32_t u32Mask)
{
	(void)NULL;
	return b8358_mask_empty(u32Mask);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sig_mask_empty_u_8358(uint32_t u32Mask)
    __attribute__((alias("gj_sig_mask_empty_u_8358")));
