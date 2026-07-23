/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4978: PE data-directory entry count.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pe_subdir_count_u(void);
 *     - Return IMAGE_NUMBEROF_DIRECTORY_ENTRIES (16) — the fixed
 *       count of DataDirectory slots in the PE optional header.
 *   uint32_t __gj_pe_subdir_count_u  (alias)
 *   __libcgj_batch4978_marker = "libcgj-batch4978"
 *
 * Exclusive continuum CREATE-ONLY PE unique wave (4971-4980). Unique
 * gj_pe_subdir_count_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4978_marker[] = "libcgj-batch4978";

/* IMAGE_NUMBEROF_DIRECTORY_ENTRIES. */
#define B4978_SUBDIR_COUNT  16u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4978_subdir_count(void)
{
	return B4978_SUBDIR_COUNT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pe_subdir_count_u - report PE DataDirectory slot count (16).
 *
 * Always returns 16 (IMAGE_NUMBEROF_DIRECTORY_ENTRIES). Soft
 * compile-time PE product constant. No parent wires.
 */
uint32_t
gj_pe_subdir_count_u(void)
{
	(void)NULL;
	return b4978_subdir_count();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pe_subdir_count_u(void)
    __attribute__((alias("gj_pe_subdir_count_u")));
