/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8161: strerror/errno success code stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_errno_success_8161(void);
 *     - Return soft errno Success code (0).
 *   uint32_t __gj_errno_success_8161  (alias)
 *   __libcgj_batch8161_marker = "libcgj-batch8161"
 *
 * Exclusive continuum CREATE-ONLY (8161-8170: strerror errno stubs —
 * success, eperm, enoent, einval, enomem, enosys, known_ok, table_n,
 * r_min_buf, batch_id_8170).
 * Unique gj_errno_success_8161 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8161_marker[] = "libcgj-batch8161";

/* Soft Success errno (0) for strerror catalog. */
#define B8161_ERRNO_SUCCESS  ((uint32_t)0u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8161_success(void)
{
	return B8161_ERRNO_SUCCESS;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_errno_success_8161 - soft errno Success code.
 *
 * Always returns 0. Catalog constant only; does not read or set errno.
 * No parent wires.
 */
uint32_t
gj_errno_success_8161(void)
{
	(void)NULL;
	return b8161_success();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_errno_success_8161(void)
    __attribute__((alias("gj_errno_success_8161")));
