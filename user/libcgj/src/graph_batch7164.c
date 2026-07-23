/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7164: pthread attr detached-state presence stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pattr_is_detached_u_7164(uint32_t state);
 *     - Return 1 if state is PTHREAD_CREATE_DETACHED (1), else 0.
 *   uint32_t __gj_pattr_is_detached_u_7164  (alias)
 *   __libcgj_batch7164_marker = "libcgj-batch7164"
 *
 * Exclusive continuum CREATE-ONLY (7161-7170: pthread attr detach stubs —
 * joinable_id, detached_id, is_joinable, is_detached, detach_ok,
 * detach_pack, detach_errorish, detach_default, detach_normalize,
 * batch_id_7170).
 * Unique gj_pattr_is_detached_u_7164 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7164_marker[] = "libcgj-batch7164";

/* Soft PTHREAD_CREATE_DETACHED (1). */
#define B7164_PTHREAD_CREATE_DETACHED ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7164_pattr_is_detached(uint32_t u32State)
{
	return (u32State == B7164_PTHREAD_CREATE_DETACHED) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pattr_is_detached_u_7164 - 1 if detach state is DETACHED.
 *
 * state: soft pthread_attr detach-state value
 *
 * Returns 1 when state equals 1 (DETACHED), else 0. Soft equality only;
 * does not call pthread_attr_getdetachstate. No parent wires.
 */
uint32_t
gj_pattr_is_detached_u_7164(uint32_t u32State)
{
	(void)NULL;
	return b7164_pattr_is_detached(u32State);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pattr_is_detached_u_7164(uint32_t u32State)
    __attribute__((alias("gj_pattr_is_detached_u_7164")));
