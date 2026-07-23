/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6088: SD path absolute-hint (leading slash).
 *
 * Surface (unique symbols):
 *   uint32_t gj_sd_path_is_abs_hint_6088(uint32_t first_byte);
 *     - Returns 1 when first_byte is ASCII '/' (0x2F), else 0.
 *   uint32_t __gj_sd_path_is_abs_hint_6088  (alias)
 *   __libcgj_batch6088_marker = "libcgj-batch6088"
 *
 * Exclusive continuum CREATE-ONLY (6081-6090: SD card / microSD path
 * stubs). Unique gj_sd_path_is_abs_hint_6088 surface only; no multi-def.
 * Soft one-byte absolute-path hint; does not parse full paths. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6088_marker[] = "libcgj-batch6088";

/* ASCII solidus — absolute POSIX path first-byte hint. */
#define B6088_PATH_ABS_BYTE  0x2Fu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6088_is_abs_hint(uint32_t uFirst)
{
	if (uFirst == B6088_PATH_ABS_BYTE) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sd_path_is_abs_hint_6088 - soft absolute SD path first-byte hint.
 *
 * first_byte: first byte of a candidate path (0..255 meaningful).
 * Returns 1 when first_byte == '/', else 0. Does not walk the string.
 * No parent wires.
 */
uint32_t
gj_sd_path_is_abs_hint_6088(uint32_t uFirstByte)
{
	(void)NULL;
	return b6088_is_abs_hint(uFirstByte);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sd_path_is_abs_hint_6088(uint32_t uFirstByte)
    __attribute__((alias("gj_sd_path_is_abs_hint_6088")));
