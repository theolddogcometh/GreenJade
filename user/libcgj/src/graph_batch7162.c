/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7162: pthread attr DETACHED detach-state id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pattr_detached_id_7162(void);
 *     - Return soft PTHREAD_CREATE_DETACHED constant (1).
 *   uint32_t __gj_pattr_detached_id_7162  (alias)
 *   __libcgj_batch7162_marker = "libcgj-batch7162"
 *
 * Exclusive continuum CREATE-ONLY (7161-7170: pthread attr detach stubs —
 * joinable_id, detached_id, is_joinable, is_detached, detach_ok,
 * detach_pack, detach_errorish, detach_default, detach_normalize,
 * batch_id_7170).
 * Unique gj_pattr_detached_id_7162 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7162_marker[] = "libcgj-batch7162";

/* Soft PTHREAD_CREATE_DETACHED (POSIX / glibc: 1). */
#define B7162_PTHREAD_CREATE_DETACHED ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7162_pattr_detached_id(void)
{
	return B7162_PTHREAD_CREATE_DETACHED;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pattr_detached_id_7162 - soft PTHREAD_CREATE_DETACHED constant.
 *
 * Always returns 1 (detached detach state). Catalog id only; does not
 * call pthread_attr_*. No parent wires.
 */
uint32_t
gj_pattr_detached_id_7162(void)
{
	(void)NULL;
	return b7162_pattr_detached_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pattr_detached_id_7162(void)
    __attribute__((alias("gj_pattr_detached_id_7162")));
