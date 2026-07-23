/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6928: copy_file_range x86_64 syscall number.
 *
 * Surface (unique symbols):
 *   uint32_t gj_cfr_nr_x86_64_6928(void);
 *     - Returns 326 (soft Linux x86_64 NR_copy_file_range catalog tag).
 *   uint32_t __gj_cfr_nr_x86_64_6928  (alias)
 *   __libcgj_batch6928_marker = "libcgj-batch6928"
 *
 * Exclusive continuum CREATE-ONLY (6921-6930: copy_file_range stubs).
 * Unique gj_cfr_nr_x86_64_6928 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6928_marker[] = "libcgj-batch6928";

/* Soft Linux x86_64 NR_copy_file_range (catalog only). */
#define B6928_NR_X86_64  326u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6928_nr(void)
{
	return B6928_NR_X86_64;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cfr_nr_x86_64_6928 - soft x86_64 copy_file_range syscall number.
 *
 * Always returns 326. Pure-data catalog stub; does not issue a syscall.
 * No parent wires.
 */
uint32_t
gj_cfr_nr_x86_64_6928(void)
{
	(void)NULL;
	return b6928_nr();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cfr_nr_x86_64_6928(void)
    __attribute__((alias("gj_cfr_nr_x86_64_6928")));
