/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8164: strerror/errno EINVAL code stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_errno_einval_8164(void);
 *     - Return soft Linux x86_64 EINVAL code (22).
 *   uint32_t __gj_errno_einval_8164  (alias)
 *   __libcgj_batch8164_marker = "libcgj-batch8164"
 *
 * Exclusive continuum CREATE-ONLY (8161-8170: strerror errno stubs —
 * success, eperm, enoent, einval, enomem, enosys, known_ok, table_n,
 * r_min_buf, batch_id_8170).
 * Unique gj_errno_einval_8164 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8164_marker[] = "libcgj-batch8164";

/* Soft Linux x86_64 EINVAL (Invalid argument). */
#define B8164_EINVAL  ((uint32_t)22u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8164_einval(void)
{
	return B8164_EINVAL;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_errno_einval_8164 - soft EINVAL errno code.
 *
 * Always returns 22. Catalog constant only; does not call strerror(3).
 * No parent wires.
 */
uint32_t
gj_errno_einval_8164(void)
{
	(void)NULL;
	return b8164_einval();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_errno_einval_8164(void)
    __attribute__((alias("gj_errno_einval_8164")));
