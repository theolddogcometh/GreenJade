/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8352: EINVAL errno-number id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_errno_inval_id_8352(void);
 *     - Return soft Linux x86_64 EINVAL catalog id (22).
 *   uint32_t __gj_errno_inval_id_8352  (alias)
 *   __libcgj_batch8352_marker = "libcgj-batch8352"
 *
 * Exclusive continuum CREATE-ONLY (8351-8360: signal/errno id stubs —
 * errno_ok_u, errno_inval_id, errno_nomem_id, errno_again_id,
 * sig_valid_u, sig_term_id, sig_int_id, sig_mask_empty_u,
 * sig_mask_full_u, batch_id_8360). Unique gj_errno_inval_id_8352
 * surface only; no multi-def. No real signal API. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8352_marker[] = "libcgj-batch8352";

/* Linux x86_64: EINVAL (local constant only; no errno.h). */
#define B8352_EINVAL  22u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8352_inval_id(void)
{
	return B8352_EINVAL;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_errno_inval_id_8352 - soft EINVAL errno catalog id.
 *
 * Always returns 22 (Linux x86_64 EINVAL). Pure data tag; does not set
 * or read errno. No parent wires.
 */
uint32_t
gj_errno_inval_id_8352(void)
{
	(void)NULL;
	return b8352_inval_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_errno_inval_id_8352(void)
    __attribute__((alias("gj_errno_inval_id_8352")));
