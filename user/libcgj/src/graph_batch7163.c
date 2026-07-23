/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7163: pthread attr joinable-state presence stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pattr_is_joinable_u_7163(uint32_t state);
 *     - Return 1 if state is PTHREAD_CREATE_JOINABLE (0), else 0.
 *   uint32_t __gj_pattr_is_joinable_u_7163  (alias)
 *   __libcgj_batch7163_marker = "libcgj-batch7163"
 *
 * Exclusive continuum CREATE-ONLY (7161-7170: pthread attr detach stubs —
 * joinable_id, detached_id, is_joinable, is_detached, detach_ok,
 * detach_pack, detach_errorish, detach_default, detach_normalize,
 * batch_id_7170).
 * Unique gj_pattr_is_joinable_u_7163 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7163_marker[] = "libcgj-batch7163";

/* Soft PTHREAD_CREATE_JOINABLE (0). */
#define B7163_PTHREAD_CREATE_JOINABLE ((uint32_t)0u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7163_pattr_is_joinable(uint32_t u32State)
{
	return (u32State == B7163_PTHREAD_CREATE_JOINABLE) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pattr_is_joinable_u_7163 - 1 if detach state is JOINABLE.
 *
 * state: soft pthread_attr detach-state value
 *
 * Returns 1 when state equals 0 (JOINABLE), else 0. Soft equality only;
 * does not call pthread_attr_getdetachstate. No parent wires.
 */
uint32_t
gj_pattr_is_joinable_u_7163(uint32_t u32State)
{
	(void)NULL;
	return b7163_pattr_is_joinable(u32State);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pattr_is_joinable_u_7163(uint32_t u32State)
    __attribute__((alias("gj_pattr_is_joinable_u_7163")));
