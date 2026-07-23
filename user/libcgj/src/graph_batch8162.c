/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8162: strerror/errno EPERM code stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_errno_eperm_8162(void);
 *     - Return soft Linux x86_64 EPERM code (1).
 *   uint32_t __gj_errno_eperm_8162  (alias)
 *   __libcgj_batch8162_marker = "libcgj-batch8162"
 *
 * Exclusive continuum CREATE-ONLY (8161-8170: strerror errno stubs —
 * success, eperm, enoent, einval, enomem, enosys, known_ok, table_n,
 * r_min_buf, batch_id_8170).
 * Unique gj_errno_eperm_8162 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8162_marker[] = "libcgj-batch8162";

/* Soft Linux x86_64 EPERM (Operation not permitted). */
#define B8162_EPERM  ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8162_eperm(void)
{
	return B8162_EPERM;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_errno_eperm_8162 - soft EPERM errno code.
 *
 * Always returns 1. Catalog constant only; does not call strerror(3).
 * No parent wires.
 */
uint32_t
gj_errno_eperm_8162(void)
{
	(void)NULL;
	return b8162_eperm();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_errno_eperm_8162(void)
    __attribute__((alias("gj_errno_eperm_8162")));
