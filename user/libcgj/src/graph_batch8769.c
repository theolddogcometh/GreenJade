/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8769: statx mask STATX_SIZE id soft tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_statx_size_id_8769(void);
 *     - Returns soft STATX_SIZE bit (0x00000200).
 *   uint32_t __gj_statx_size_id_8769  (alias)
 *   __libcgj_batch8769_marker = "libcgj-batch8769"
 *
 * Exclusive continuum CREATE-ONLY (8761-8770: statx mask id stubs).
 * Unique gj_statx_size_id_8769 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8769_marker[] = "libcgj-batch8769";

/* Soft Linux STATX_SIZE (stx_size present). */
#define B8769_STATX_SIZE  0x00000200u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8769_size(void)
{
	return B8769_STATX_SIZE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_statx_size_id_8769 - soft STATX_SIZE request bit id.
 *
 * Always returns 0x00000200. Soft pure-data product tag for the
 * statx mask bit that asks for file size. Does not call the
 * statx syscall. No parent wires.
 */
uint32_t
gj_statx_size_id_8769(void)
{
	(void)NULL;
	return b8769_size();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_statx_size_id_8769(void)
    __attribute__((alias("gj_statx_size_id_8769")));
