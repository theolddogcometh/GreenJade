/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7168: pthread attr default detach-state stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pattr_detach_default_u_7168(void);
 *     - Return soft default detach state (JOINABLE = 0).
 *   uint32_t __gj_pattr_detach_default_u_7168  (alias)
 *   __libcgj_batch7168_marker = "libcgj-batch7168"
 *
 * Exclusive continuum CREATE-ONLY (7161-7170: pthread attr detach stubs —
 * joinable_id, detached_id, is_joinable, is_detached, detach_ok,
 * detach_pack, detach_errorish, detach_default, detach_normalize,
 * batch_id_7170).
 * Unique gj_pattr_detach_default_u_7168 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7168_marker[] = "libcgj-batch7168";

/*
 * POSIX default after pthread_attr_init: PTHREAD_CREATE_JOINABLE (0).
 */
#define B7168_PTHREAD_CREATE_JOINABLE ((uint32_t)0u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7168_pattr_detach_default(void)
{
	return B7168_PTHREAD_CREATE_JOINABLE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pattr_detach_default_u_7168 - soft default detach state.
 *
 * Always returns 0 (JOINABLE), matching pthread_attr_init defaults.
 * Catalog constant only; does not call pthread_attr_init. No parent wires.
 */
uint32_t
gj_pattr_detach_default_u_7168(void)
{
	(void)NULL;
	return b7168_pattr_detach_default();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pattr_detach_default_u_7168(void)
    __attribute__((alias("gj_pattr_detach_default_u_7168")));
