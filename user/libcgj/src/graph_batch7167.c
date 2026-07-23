/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7167: pthread attr detach-state errorish stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pattr_detach_errorish_u_7167(uint32_t state);
 *     - Return 1 if state is neither JOINABLE (0) nor DETACHED (1).
 *   uint32_t __gj_pattr_detach_errorish_u_7167  (alias)
 *   __libcgj_batch7167_marker = "libcgj-batch7167"
 *
 * Exclusive continuum CREATE-ONLY (7161-7170: pthread attr detach stubs —
 * joinable_id, detached_id, is_joinable, is_detached, detach_ok,
 * detach_pack, detach_errorish, detach_default, detach_normalize,
 * batch_id_7170).
 * Unique gj_pattr_detach_errorish_u_7167 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7167_marker[] = "libcgj-batch7167";

/* Soft detach-state domain: JOINABLE (0) and DETACHED (1). */
#define B7167_PTHREAD_CREATE_JOINABLE ((uint32_t)0u)
#define B7167_PTHREAD_CREATE_DETACHED ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7167_pattr_detach_errorish(uint32_t u32State)
{
	if (u32State == B7167_PTHREAD_CREATE_JOINABLE)
		return 0u;
	if (u32State == B7167_PTHREAD_CREATE_DETACHED)
		return 0u;
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pattr_detach_errorish_u_7167 - 1 if detach state is unknown.
 *
 * state: soft pthread_attr detach-state value
 *
 * Returns 1 when state is neither 0 nor 1, else 0. Soft inverse of
 * detach_ok; does not call pthread_attr_*. No parent wires.
 */
uint32_t
gj_pattr_detach_errorish_u_7167(uint32_t u32State)
{
	(void)NULL;
	return b7167_pattr_detach_errorish(u32State);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pattr_detach_errorish_u_7167(uint32_t u32State)
    __attribute__((alias("gj_pattr_detach_errorish_u_7167")));
