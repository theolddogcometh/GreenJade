/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5427: texture format bytes-per-pixel probe.
 *
 * Surface (unique symbols):
 *   uint32_t gj_tex_format_bpp_5427(uint32_t format_code);
 *     - Map a local Deck GPU texture format code to bytes per texel:
 *         0 = R8        → 1
 *         1 = RG8       → 2
 *         2 = RGB8      → 3
 *         3 = RGBA8     → 4
 *         4 = RGB565    → 2
 *         5 = RGBA4     → 2
 *         6 = RGB5_A1   → 2
 *         7 = R16F      → 2
 *         8 = RG16F     → 4
 *         9 = RGBA16F   → 8
 *       Unknown codes return 0. Soft freestanding format table only.
 *   uint32_t __gj_tex_format_bpp_5427  (alias)
 *   __libcgj_batch5427_marker = "libcgj-batch5427"
 *
 * Exclusive continuum CREATE-ONLY (5421-5430). Deck GPU path: OpenGL ES
 * / GLX / EGL / shader / texture stubs. Unique gj_*_5427 surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5427_marker[] = "libcgj-batch5427";

/* Local texture format codes (not GL enum values — freestanding). */
#define B5427_FMT_R8       0u
#define B5427_FMT_RG8      1u
#define B5427_FMT_RGB8     2u
#define B5427_FMT_RGBA8    3u
#define B5427_FMT_RGB565   4u
#define B5427_FMT_RGBA4    5u
#define B5427_FMT_RGB5_A1  6u
#define B5427_FMT_R16F     7u
#define B5427_FMT_RG16F    8u
#define B5427_FMT_RGBA16F  9u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5427_bpp(uint32_t uFmt)
{
	switch (uFmt) {
	case B5427_FMT_R8:
		return 1u;
	case B5427_FMT_RG8:
	case B5427_FMT_RGB565:
	case B5427_FMT_RGBA4:
	case B5427_FMT_RGB5_A1:
	case B5427_FMT_R16F:
		return 2u;
	case B5427_FMT_RGB8:
		return 3u;
	case B5427_FMT_RGBA8:
	case B5427_FMT_RG16F:
		return 4u;
	case B5427_FMT_RGBA16F:
		return 8u;
	default:
		return 0u;
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tex_format_bpp_5427 - bytes-per-texel for a local Deck format code.
 *
 * format_code: local enum 0..9 (see header table); others → 0.
 * Returns bytes per texel for soft Deck GPU texture path sizing.
 * Freestanding; no GL. No parent wires.
 */
uint32_t
gj_tex_format_bpp_5427(uint32_t uFmt)
{
	(void)NULL;
	return b5427_bpp(uFmt);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_tex_format_bpp_5427(uint32_t uFmt)
    __attribute__((alias("gj_tex_format_bpp_5427")));
