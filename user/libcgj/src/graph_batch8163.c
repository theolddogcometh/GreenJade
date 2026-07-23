/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8163: strerror/errno ENOENT code stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_errno_enoent_8163(void);
 *     - Return soft Linux x86_64 ENOENT code (2).
 *   uint32_t __gj_errno_enoent_8163  (alias)
 *   __libcgj_batch8163_marker = "libcgj-batch8163"
 *
 * Exclusive continuum CREATE-ONLY (8161-8170: strerror errno stubs —
 * success, eperm, enoent, einval, enomem, enosys, known_ok, table_n,
 * r_min_buf, batch_id_8170).
 * Unique gj_errno_enoent_8163 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8163_marker[] = "libcgj-batch8163";

/* Soft Linux x86_64 ENOENT (No such file or directory). */
#define B8163_ENOENT  ((uint32_t)2u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8163_enoent(void)
{
	return B8163_ENOENT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_errno_enoent_8163 - soft ENOENT errno code.
 *
 * Always returns 2. Catalog constant only; does not call strerror(3).
 * No parent wires.
 */
uint32_t
gj_errno_enoent_8163(void)
{
	(void)NULL;
	return b8163_enoent();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_errno_enoent_8163(void)
    __attribute__((alias("gj_errno_enoent_8163")));
