/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8356: SIGTERM signal-number id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sig_term_id_8356(void);
 *     - Return soft Linux x86_64 SIGTERM catalog id (15).
 *   uint32_t __gj_sig_term_id_8356  (alias)
 *   __libcgj_batch8356_marker = "libcgj-batch8356"
 *
 * Exclusive continuum CREATE-ONLY (8351-8360: signal/errno id stubs —
 * errno_ok_u, errno_inval_id, errno_nomem_id, errno_again_id,
 * sig_valid_u, sig_term_id, sig_int_id, sig_mask_empty_u,
 * sig_mask_full_u, batch_id_8360). Unique gj_sig_term_id_8356 surface
 * only; no multi-def. No real signal API. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8356_marker[] = "libcgj-batch8356";

/* Linux x86_64: SIGTERM (local constant only; no signal.h). */
#define B8356_SIGTERM  15u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8356_term_id(void)
{
	return B8356_SIGTERM;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sig_term_id_8356 - soft SIGTERM signal catalog id.
 *
 * Always returns 15 (Linux x86_64 SIGTERM). Pure data tag; does not
 * raise, kill, or install handlers. No parent wires.
 */
uint32_t
gj_sig_term_id_8356(void)
{
	(void)NULL;
	return b8356_term_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sig_term_id_8356(void)
    __attribute__((alias("gj_sig_term_id_8356")));
