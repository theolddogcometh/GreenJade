/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7512: waitpid WUNTRACED option id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_waitpid_wuntraced_id_7512(void);
 *     - Return soft WUNTRACED waitpid option class (2).
 *   uint32_t __gj_waitpid_wuntraced_id_7512  (alias)
 *   __libcgj_batch7512_marker = "libcgj-batch7512"
 *
 * Exclusive continuum CREATE-ONLY (7511-7520: waitpid options stubs —
 * wnohang_id, wuntraced_id, wcontinued_id, wexited_id, opts_mask_id,
 * has_wnohang, has_wuntraced, opts_ok, opts_errorish, batch_id_7520).
 * Unique gj_waitpid_wuntraced_id_7512 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7512_marker[] = "libcgj-batch7512";

/* Soft WUNTRACED: also return for stopped (untraced) children. */
#define B7512_WUNTRACED ((uint32_t)2u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7512_waitpid_wuntraced_id(void)
{
	return B7512_WUNTRACED;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_waitpid_wuntraced_id_7512 - soft WUNTRACED waitpid option constant.
 *
 * Always returns 2. Catalog id only; does not call waitpid.
 * No parent wires.
 */
uint32_t
gj_waitpid_wuntraced_id_7512(void)
{
	(void)NULL;
	return b7512_waitpid_wuntraced_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_waitpid_wuntraced_id_7512(void)
    __attribute__((alias("gj_waitpid_wuntraced_id_7512")));
