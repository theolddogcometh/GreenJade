/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7513: waitpid WCONTINUED option id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_waitpid_wcontinued_id_7513(void);
 *     - Return soft WCONTINUED waitpid option class (8).
 *   uint32_t __gj_waitpid_wcontinued_id_7513  (alias)
 *   __libcgj_batch7513_marker = "libcgj-batch7513"
 *
 * Exclusive continuum CREATE-ONLY (7511-7520: waitpid options stubs —
 * wnohang_id, wuntraced_id, wcontinued_id, wexited_id, opts_mask_id,
 * has_wnohang, has_wuntraced, opts_ok, opts_errorish, batch_id_7520).
 * Unique gj_waitpid_wcontinued_id_7513 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7513_marker[] = "libcgj-batch7513";

/* Soft WCONTINUED: also return for continued children. */
#define B7513_WCONTINUED ((uint32_t)8u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7513_waitpid_wcontinued_id(void)
{
	return B7513_WCONTINUED;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_waitpid_wcontinued_id_7513 - soft WCONTINUED waitpid option constant.
 *
 * Always returns 8. Catalog id only; does not call waitpid.
 * No parent wires.
 */
uint32_t
gj_waitpid_wcontinued_id_7513(void)
{
	(void)NULL;
	return b7513_waitpid_wcontinued_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_waitpid_wcontinued_id_7513(void)
    __attribute__((alias("gj_waitpid_wcontinued_id_7513")));
