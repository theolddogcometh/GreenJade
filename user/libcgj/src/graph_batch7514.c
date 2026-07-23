/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7514: wait-family WEXITED option id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_waitpid_wexited_id_7514(void);
 *     - Return soft WEXITED wait-family option class (4).
 *   uint32_t __gj_waitpid_wexited_id_7514  (alias)
 *   __libcgj_batch7514_marker = "libcgj-batch7514"
 *
 * Exclusive continuum CREATE-ONLY (7511-7520: waitpid options stubs —
 * wnohang_id, wuntraced_id, wcontinued_id, wexited_id, opts_mask_id,
 * has_wnohang, has_wuntraced, opts_ok, opts_errorish, batch_id_7520).
 * Unique gj_waitpid_wexited_id_7514 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7514_marker[] = "libcgj-batch7514";

/* Soft WEXITED: wait for children that have exited (waitid-family). */
#define B7514_WEXITED ((uint32_t)4u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7514_waitpid_wexited_id(void)
{
	return B7514_WEXITED;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_waitpid_wexited_id_7514 - soft WEXITED wait-family option constant.
 *
 * Always returns 4. Catalog id only; does not call waitpid/waitid.
 * No parent wires.
 */
uint32_t
gj_waitpid_wexited_id_7514(void)
{
	(void)NULL;
	return b7514_waitpid_wexited_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_waitpid_wexited_id_7514(void)
    __attribute__((alias("gj_waitpid_wexited_id_7514")));
