/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8166: strerror/errno ENOSYS code stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_errno_enosys_8166(void);
 *     - Return soft Linux x86_64 ENOSYS code (38).
 *   uint32_t __gj_errno_enosys_8166  (alias)
 *   __libcgj_batch8166_marker = "libcgj-batch8166"
 *
 * Exclusive continuum CREATE-ONLY (8161-8170: strerror errno stubs —
 * success, eperm, enoent, einval, enomem, enosys, known_ok, table_n,
 * r_min_buf, batch_id_8170).
 * Unique gj_errno_enosys_8166 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8166_marker[] = "libcgj-batch8166";

/* Soft Linux x86_64 ENOSYS (Function not implemented). */
#define B8166_ENOSYS  ((uint32_t)38u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8166_enosys(void)
{
	return B8166_ENOSYS;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_errno_enosys_8166 - soft ENOSYS errno code.
 *
 * Always returns 38. Catalog constant only; does not call strerror(3).
 * No parent wires.
 */
uint32_t
gj_errno_enosys_8166(void)
{
	(void)NULL;
	return b8166_enosys();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_errno_enosys_8166(void)
    __attribute__((alias("gj_errno_enosys_8166")));
