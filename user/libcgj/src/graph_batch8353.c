/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8353: ENOMEM errno-number id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_errno_nomem_id_8353(void);
 *     - Return soft Linux x86_64 ENOMEM catalog id (12).
 *   uint32_t __gj_errno_nomem_id_8353  (alias)
 *   __libcgj_batch8353_marker = "libcgj-batch8353"
 *
 * Exclusive continuum CREATE-ONLY (8351-8360: signal/errno id stubs —
 * errno_ok_u, errno_inval_id, errno_nomem_id, errno_again_id,
 * sig_valid_u, sig_term_id, sig_int_id, sig_mask_empty_u,
 * sig_mask_full_u, batch_id_8360). Unique gj_errno_nomem_id_8353
 * surface only; no multi-def. No real signal API. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8353_marker[] = "libcgj-batch8353";

/* Linux x86_64: ENOMEM (local constant only; no errno.h). */
#define B8353_ENOMEM  12u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8353_nomem_id(void)
{
	return B8353_ENOMEM;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_errno_nomem_id_8353 - soft ENOMEM errno catalog id.
 *
 * Always returns 12 (Linux x86_64 ENOMEM). Pure data tag; does not set
 * or read errno. No parent wires.
 */
uint32_t
gj_errno_nomem_id_8353(void)
{
	(void)NULL;
	return b8353_nomem_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_errno_nomem_id_8353(void)
    __attribute__((alias("gj_errno_nomem_id_8353")));
