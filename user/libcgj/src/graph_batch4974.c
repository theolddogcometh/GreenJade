/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4974: PE Machine I386 constant.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pe_machine_i386_u(void);
 *     - Return IMAGE_FILE_MACHINE_I386 (0x014C).
 *   uint32_t __gj_pe_machine_i386_u  (alias)
 *   __libcgj_batch4974_marker = "libcgj-batch4974"
 *
 * Exclusive continuum CREATE-ONLY PE unique wave (4971-4980). Distinct
 * from gj_pe_machine_u (buffer parse) and gj_pe_machine_amd64_u —
 * unique gj_pe_machine_i386_u surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4974_marker[] = "libcgj-batch4974";

/* IMAGE_FILE_MACHINE_I386. */
#define B4974_MACHINE_I386  0x014Cu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4974_machine_i386(void)
{
	return B4974_MACHINE_I386;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pe_machine_i386_u - report IMAGE_FILE_MACHINE_I386 (0x014C).
 *
 * Always returns 0x014C. Soft compile-time PE Machine constant for
 * 32-bit x86. Does not parse an image. No parent wires.
 */
uint32_t
gj_pe_machine_i386_u(void)
{
	(void)NULL;
	return b4974_machine_i386();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pe_machine_i386_u(void)
    __attribute__((alias("gj_pe_machine_i386_u")));
