/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4977: PE optional header PE32 magic.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pe_opt_hdr32_magic_u(void);
 *     - Return IMAGE_NT_OPTIONAL_HDR32_MAGIC (0x10B) for PE32.
 *   uint32_t __gj_pe_opt_hdr32_magic_u  (alias)
 *   __libcgj_batch4977_marker = "libcgj-batch4977"
 *
 * Exclusive continuum CREATE-ONLY PE unique wave (4971-4980). Distinct
 * from gj_pe_opt_hdr64_magic_u — unique gj_pe_opt_hdr32_magic_u surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4977_marker[] = "libcgj-batch4977";

/* IMAGE_NT_OPTIONAL_HDR32_MAGIC (PE32). */
#define B4977_OPT_HDR32_MAGIC  0x010Bu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4977_opt_hdr32_magic(void)
{
	return B4977_OPT_HDR32_MAGIC;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pe_opt_hdr32_magic_u - report PE32 optional-header magic (0x10B).
 *
 * Always returns 0x010B. Soft compile-time PE product constant.
 * No parent wires.
 */
uint32_t
gj_pe_opt_hdr32_magic_u(void)
{
	(void)NULL;
	return b4977_opt_hdr32_magic();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pe_opt_hdr32_magic_u(void)
    __attribute__((alias("gj_pe_opt_hdr32_magic_u")));
