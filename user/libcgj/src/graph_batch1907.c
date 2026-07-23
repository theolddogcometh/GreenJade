/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1907: framebuffer color-depth soft validity.
 *
 * Surface (unique symbols):
 *   int gj_color_depth_ok(int bits);
 *     — Return 1 if bits is one of the conventional desktop color
 *       depths 15, 16, 24, or 32 (bits per pixel), else 0.
 *   int __gj_color_depth_ok  (alias)
 *   __libcgj_batch1907_marker = "libcgj-batch1907"
 *
 * Desktop window exclusive pure-data wave (1901–1910). Unique surface
 * only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1907_marker[] = "libcgj-batch1907";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if bits is a conventional bpp value. */
static int
b1907_depth_ok(int nBits)
{
	if (nBits == 15) {
		return 1;
	}
	if (nBits == 16) {
		return 1;
	}
	if (nBits == 24) {
		return 1;
	}
	if (nBits == 32) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_color_depth_ok — 1 iff bits is 15, 16, 24, or 32.
 *
 * bits: framebuffer / visual color depth in bits per pixel
 * Returns 1 for the four conventional desktop depths, else 0.
 */
int
gj_color_depth_ok(int nBits)
{
	(void)NULL;
	return b1907_depth_ok(nBits);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_color_depth_ok(int nBits)
    __attribute__((alias("gj_color_depth_ok")));
