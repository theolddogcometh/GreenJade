/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7166: pthread attr detach-state pack stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pattr_detach_pack_u_7166(uint32_t want_detached);
 *     - Pack soft want into detach state (0=JOINABLE, 1=DETACHED).
 *   uint32_t __gj_pattr_detach_pack_u_7166  (alias)
 *   __libcgj_batch7166_marker = "libcgj-batch7166"
 *
 * Exclusive continuum CREATE-ONLY (7161-7170: pthread attr detach stubs —
 * joinable_id, detached_id, is_joinable, is_detached, detach_ok,
 * detach_pack, detach_errorish, detach_default, detach_normalize,
 * batch_id_7170).
 * Unique gj_pattr_detach_pack_u_7166 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7166_marker[] = "libcgj-batch7166";

/* Soft PTHREAD_CREATE_* values. */
#define B7166_PTHREAD_CREATE_JOINABLE ((uint32_t)0u)
#define B7166_PTHREAD_CREATE_DETACHED ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7166_pattr_detach_pack(uint32_t u32WantDetached)
{
	if (u32WantDetached != 0u)
		return B7166_PTHREAD_CREATE_DETACHED;
	return B7166_PTHREAD_CREATE_JOINABLE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pattr_detach_pack_u_7166 - soft-pack detach-state want flag.
 *
 * want_detached: non-zero selects DETACHED (1), zero selects JOINABLE (0)
 *
 * Returns 0 or 1. Pure integer pack; does not call
 * pthread_attr_setdetachstate. No parent wires.
 */
uint32_t
gj_pattr_detach_pack_u_7166(uint32_t u32WantDetached)
{
	(void)NULL;
	return b7166_pattr_detach_pack(u32WantDetached);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pattr_detach_pack_u_7166(uint32_t u32WantDetached)
    __attribute__((alias("gj_pattr_detach_pack_u_7166")));
