/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5614: Wine prefix magic integer.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wine_prefix_magic_5614(void);
 *     - Returns the soft compile-time Wine prefix magic tag
 *       0x57494E45 ('W','I','N','E' big-endian FourCC). Integer
 *       product tag only; does not inspect WINEPREFIX paths.
 *   uint32_t __gj_wine_prefix_magic_5614  (alias)
 *   __libcgj_batch5614_marker = "libcgj-batch5614"
 *
 * Exclusive continuum CREATE-ONLY (5611-5620: Proton-GE version, wine
 * prefix magic, DXVK/VKD3D readiness integers). Unique
 * gj_wine_prefix_magic_5614 surface only; no multi-def. Distinct from
 * gj_wine_prefix_ok_u (batch2273). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5614_marker[] = "libcgj-batch5614";

/* FourCC 'WINE' as big-endian uint32 product magic. */
#define B5614_WINE_PREFIX_MAGIC  0x57494E45u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5614_magic(void)
{
	return B5614_WINE_PREFIX_MAGIC;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wine_prefix_magic_5614 - report Wine prefix magic integer.
 *
 * Always returns 0x57494E45 ('WINE'). Soft pure-data product tag; does
 * not probe prefixes or call libc. No parent wires.
 */
uint32_t
gj_wine_prefix_magic_5614(void)
{
	(void)NULL;
	return b5614_magic();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wine_prefix_magic_5614(void)
    __attribute__((alias("gj_wine_prefix_magic_5614")));
