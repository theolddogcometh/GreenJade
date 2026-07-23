/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7165: pthread attr detach-state validity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pattr_detach_ok_u_7165(uint32_t state);
 *     - Return 1 if state is JOINABLE (0) or DETACHED (1), else 0.
 *   uint32_t __gj_pattr_detach_ok_u_7165  (alias)
 *   __libcgj_batch7165_marker = "libcgj-batch7165"
 *
 * Exclusive continuum CREATE-ONLY (7161-7170: pthread attr detach stubs —
 * joinable_id, detached_id, is_joinable, is_detached, detach_ok,
 * detach_pack, detach_errorish, detach_default, detach_normalize,
 * batch_id_7170).
 * Unique gj_pattr_detach_ok_u_7165 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7165_marker[] = "libcgj-batch7165";

/* Soft detach-state domain: JOINABLE (0) and DETACHED (1). */
#define B7165_PTHREAD_CREATE_JOINABLE ((uint32_t)0u)
#define B7165_PTHREAD_CREATE_DETACHED ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7165_pattr_detach_ok(uint32_t u32State)
{
	if (u32State == B7165_PTHREAD_CREATE_JOINABLE)
		return 1u;
	if (u32State == B7165_PTHREAD_CREATE_DETACHED)
		return 1u;
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pattr_detach_ok_u_7165 - 1 if detach state is a known value.
 *
 * state: soft pthread_attr detach-state value
 *
 * Returns 1 when state is 0 (JOINABLE) or 1 (DETACHED), else 0.
 * Soft catalog check; does not call pthread_attr_*. No parent wires.
 */
uint32_t
gj_pattr_detach_ok_u_7165(uint32_t u32State)
{
	(void)NULL;
	return b7165_pattr_detach_ok(u32State);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pattr_detach_ok_u_7165(uint32_t u32State)
    __attribute__((alias("gj_pattr_detach_ok_u_7165")));
