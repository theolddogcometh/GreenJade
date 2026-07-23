/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4973: PE Machine AMD64 constant.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pe_machine_amd64_u(void);
 *     - Return IMAGE_FILE_MACHINE_AMD64 (0x8664).
 *   uint32_t __gj_pe_machine_amd64_u  (alias)
 *   __libcgj_batch4973_marker = "libcgj-batch4973"
 *
 * Exclusive continuum CREATE-ONLY PE unique wave (4971-4980). Distinct
 * from gj_pe_machine_u (buffer parse) and gj_pe_machine_i386_u —
 * unique gj_pe_machine_amd64_u surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4973_marker[] = "libcgj-batch4973";

/* IMAGE_FILE_MACHINE_AMD64. */
#define B4973_MACHINE_AMD64  0x8664u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4973_machine_amd64(void)
{
	return B4973_MACHINE_AMD64;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pe_machine_amd64_u - report IMAGE_FILE_MACHINE_AMD64 (0x8664).
 *
 * Always returns 0x8664. Soft compile-time PE Machine constant for
 * x86-64. Does not parse an image; see gj_pe_machine_u for buffer
 * read. No parent wires.
 */
uint32_t
gj_pe_machine_amd64_u(void)
{
	(void)NULL;
	return b4973_machine_amd64();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pe_machine_amd64_u(void)
    __attribute__((alias("gj_pe_machine_amd64_u")));
