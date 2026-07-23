/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8354: EAGAIN errno-number id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_errno_again_id_8354(void);
 *     - Return soft Linux x86_64 EAGAIN catalog id (11).
 *   uint32_t __gj_errno_again_id_8354  (alias)
 *   __libcgj_batch8354_marker = "libcgj-batch8354"
 *
 * Exclusive continuum CREATE-ONLY (8351-8360: signal/errno id stubs —
 * errno_ok_u, errno_inval_id, errno_nomem_id, errno_again_id,
 * sig_valid_u, sig_term_id, sig_int_id, sig_mask_empty_u,
 * sig_mask_full_u, batch_id_8360). Unique gj_errno_again_id_8354
 * surface only; no multi-def. No real signal API. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8354_marker[] = "libcgj-batch8354";

/*
 * Linux x86_64: EAGAIN (local constant only; no errno.h).
 * EWOULDBLOCK shares this number on Linux; catalog id is one value.
 */
#define B8354_EAGAIN  11u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8354_again_id(void)
{
	return B8354_EAGAIN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_errno_again_id_8354 - soft EAGAIN errno catalog id.
 *
 * Always returns 11 (Linux x86_64 EAGAIN / EWOULDBLOCK). Pure data tag;
 * does not set or read errno. No parent wires.
 */
uint32_t
gj_errno_again_id_8354(void)
{
	(void)NULL;
	return b8354_again_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_errno_again_id_8354(void)
    __attribute__((alias("gj_errno_again_id_8354")));
