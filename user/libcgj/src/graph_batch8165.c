/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8165: strerror/errno ENOMEM code stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_errno_enomem_8165(void);
 *     - Return soft Linux x86_64 ENOMEM code (12).
 *   uint32_t __gj_errno_enomem_8165  (alias)
 *   __libcgj_batch8165_marker = "libcgj-batch8165"
 *
 * Exclusive continuum CREATE-ONLY (8161-8170: strerror errno stubs —
 * success, eperm, enoent, einval, enomem, enosys, known_ok, table_n,
 * r_min_buf, batch_id_8170).
 * Unique gj_errno_enomem_8165 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8165_marker[] = "libcgj-batch8165";

/* Soft Linux x86_64 ENOMEM (Cannot allocate memory). */
#define B8165_ENOMEM  ((uint32_t)12u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8165_enomem(void)
{
	return B8165_ENOMEM;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_errno_enomem_8165 - soft ENOMEM errno code.
 *
 * Always returns 12. Catalog constant only; does not call strerror(3).
 * No parent wires.
 */
uint32_t
gj_errno_enomem_8165(void)
{
	(void)NULL;
	return b8165_enomem();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_errno_enomem_8165(void)
    __attribute__((alias("gj_errno_enomem_8165")));
