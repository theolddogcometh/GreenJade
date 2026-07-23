/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5422: GLX FBConfig RGBA bit-depth sum stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_glx_fbconfig_rgba_bits_5422(uint32_t r, uint32_t g,
 *                                           uint32_t b, uint32_t a);
 *     - Return r+g+b+a color-channel bit depths for a soft GLX FBConfig
 *       probe. Saturates each channel to 8 bits before summing (max 32).
 *       Does not talk to a real X11/GLX display.
 *   uint32_t __gj_glx_fbconfig_rgba_bits_5422  (alias)
 *   __libcgj_batch5422_marker = "libcgj-batch5422"
 *
 * Exclusive continuum CREATE-ONLY (5421-5430). Deck GPU path: OpenGL ES
 * / GLX / EGL / shader / texture stubs. Unique gj_*_5422 surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5422_marker[] = "libcgj-batch5422";

/* Soft per-channel bit-depth cap (8 bpp channel). */
#define B5422_CH_MAX  8u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5422_ch(uint32_t uBits)
{
	return (uBits > B5422_CH_MAX) ? B5422_CH_MAX : uBits;
}

static uint32_t
b5422_rgba_sum(uint32_t uR, uint32_t uG, uint32_t uB, uint32_t uA)
{
	return b5422_ch(uR) + b5422_ch(uG) + b5422_ch(uB) + b5422_ch(uA);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_glx_fbconfig_rgba_bits_5422 - soft GLX FBConfig RGBA bit-depth sum.
 *
 * r,g,b,a: requested channel bit depths (each saturated to 0..8).
 * Returns sum of saturated channels (0..32). Freestanding integer stub
 * for Deck GPU GLX path probes; no X11/GLX calls. No parent wires.
 */
uint32_t
gj_glx_fbconfig_rgba_bits_5422(uint32_t uR, uint32_t uG, uint32_t uB,
    uint32_t uA)
{
	(void)NULL;
	return b5422_rgba_sum(uR, uG, uB, uA);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_glx_fbconfig_rgba_bits_5422(uint32_t uR, uint32_t uG,
    uint32_t uB, uint32_t uA)
    __attribute__((alias("gj_glx_fbconfig_rgba_bits_5422")));
