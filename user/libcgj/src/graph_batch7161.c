/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7161: pthread attr JOINABLE detach-state id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pattr_joinable_id_7161(void);
 *     - Return soft PTHREAD_CREATE_JOINABLE constant (0).
 *   uint32_t __gj_pattr_joinable_id_7161  (alias)
 *   __libcgj_batch7161_marker = "libcgj-batch7161"
 *
 * Exclusive continuum CREATE-ONLY (7161-7170: pthread attr detach stubs —
 * joinable_id, detached_id, is_joinable, is_detached, detach_ok,
 * detach_pack, detach_errorish, detach_default, detach_normalize,
 * batch_id_7170).
 * Unique gj_pattr_joinable_id_7161 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7161_marker[] = "libcgj-batch7161";

/* Soft PTHREAD_CREATE_JOINABLE (POSIX / glibc: 0). */
#define B7161_PTHREAD_CREATE_JOINABLE ((uint32_t)0u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7161_pattr_joinable_id(void)
{
	return B7161_PTHREAD_CREATE_JOINABLE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pattr_joinable_id_7161 - soft PTHREAD_CREATE_JOINABLE constant.
 *
 * Always returns 0 (joinable detach state). Catalog id only; does not
 * call pthread_attr_*. No parent wires.
 */
uint32_t
gj_pattr_joinable_id_7161(void)
{
	(void)NULL;
	return b7161_pattr_joinable_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pattr_joinable_id_7161(void)
    __attribute__((alias("gj_pattr_joinable_id_7161")));
