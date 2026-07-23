/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7169: pthread attr detach-state normalize stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pattr_detach_normalize_u_7169(uint32_t state);
 *     - Pass through JOINABLE/DETACHED; map unknown to JOINABLE (0).
 *   uint32_t __gj_pattr_detach_normalize_u_7169  (alias)
 *   __libcgj_batch7169_marker = "libcgj-batch7169"
 *
 * Exclusive continuum CREATE-ONLY (7161-7170: pthread attr detach stubs —
 * joinable_id, detached_id, is_joinable, is_detached, detach_ok,
 * detach_pack, detach_errorish, detach_default, detach_normalize,
 * batch_id_7170).
 * Unique gj_pattr_detach_normalize_u_7169 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7169_marker[] = "libcgj-batch7169";

/* Soft detach-state domain. */
#define B7169_PTHREAD_CREATE_JOINABLE ((uint32_t)0u)
#define B7169_PTHREAD_CREATE_DETACHED ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7169_pattr_detach_normalize(uint32_t u32State)
{
	if (u32State == B7169_PTHREAD_CREATE_DETACHED)
		return B7169_PTHREAD_CREATE_DETACHED;
	/* JOINABLE and any unknown collapse to JOINABLE default. */
	return B7169_PTHREAD_CREATE_JOINABLE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pattr_detach_normalize_u_7169 - soft-normalize detach state.
 *
 * state: soft pthread_attr detach-state value
 *
 * Returns 1 if DETACHED, else 0 (JOINABLE or unknown). Soft clamp only;
 * does not call pthread_attr_setdetachstate. No parent wires.
 */
uint32_t
gj_pattr_detach_normalize_u_7169(uint32_t u32State)
{
	(void)NULL;
	return b7169_pattr_detach_normalize(u32State);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pattr_detach_normalize_u_7169(uint32_t u32State)
    __attribute__((alias("gj_pattr_detach_normalize_u_7169")));
