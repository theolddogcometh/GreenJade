/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7517: waitpid has-WUNTRACED option stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_waitpid_has_wuntraced_u_7517(uint32_t opts);
 *     - Return 1 if opts has WUNTRACED (bit value 2) set, else 0.
 *   uint32_t __gj_waitpid_has_wuntraced_u_7517  (alias)
 *   __libcgj_batch7517_marker = "libcgj-batch7517"
 *
 * Exclusive continuum CREATE-ONLY (7511-7520: waitpid options stubs —
 * wnohang_id, wuntraced_id, wcontinued_id, wexited_id, opts_mask_id,
 * has_wnohang, has_wuntraced, opts_ok, opts_errorish, batch_id_7520).
 * Unique gj_waitpid_has_wuntraced_u_7517 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7517_marker[] = "libcgj-batch7517";

/* Soft WUNTRACED bit in waitpid options. */
#define B7517_WUNTRACED ((uint32_t)2u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7517_waitpid_has_wuntraced(uint32_t u32Opts)
{
	return ((u32Opts & B7517_WUNTRACED) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_waitpid_has_wuntraced_u_7517 - 1 if waitpid opts includes WUNTRACED.
 *
 * opts: soft waitpid options bitmask
 *
 * Returns 1 when bit value 2 is set, else 0.
 * Soft bit test only; does not call waitpid. No parent wires.
 */
uint32_t
gj_waitpid_has_wuntraced_u_7517(uint32_t u32Opts)
{
	(void)NULL;
	return b7517_waitpid_has_wuntraced(u32Opts);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_waitpid_has_wuntraced_u_7517(uint32_t u32Opts)
    __attribute__((alias("gj_waitpid_has_wuntraced_u_7517")));
