/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5424: EGL config attribute match probe.
 *
 * Surface (unique symbols):
 *   uint32_t gj_egl_config_attr_match_5424(uint32_t want, uint32_t have,
 *                                          uint32_t mask);
 *     - Return 1 when (have & mask) equals (want & mask), else 0.
 *       Soft EGL config choose attribute match (color/depth/stencil
 *       capability bits) without calling eglChooseConfig.
 *   uint32_t __gj_egl_config_attr_match_5424  (alias)
 *   __libcgj_batch5424_marker = "libcgj-batch5424"
 *
 * Exclusive continuum CREATE-ONLY (5421-5430). Deck GPU path: OpenGL ES
 * / GLX / EGL / shader / texture stubs. Unique gj_*_5424 surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5424_marker[] = "libcgj-batch5424";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5424_attr_match(uint32_t uWant, uint32_t uHave, uint32_t uMask)
{
	return ((uHave & uMask) == (uWant & uMask)) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_egl_config_attr_match_5424 - soft EGL config attr equality under mask.
 *
 * want: requested attribute bitfield (e.g. RGB565 | DEPTH16).
 * have: candidate config capability bitfield.
 * mask: bits that must match exactly.
 * Returns 1 on match, 0 otherwise. Freestanding; no libEGL.
 * No parent wires.
 */
uint32_t
gj_egl_config_attr_match_5424(uint32_t uWant, uint32_t uHave, uint32_t uMask)
{
	(void)NULL;
	return b5424_attr_match(uWant, uHave, uMask);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_egl_config_attr_match_5424(uint32_t uWant, uint32_t uHave,
    uint32_t uMask)
    __attribute__((alias("gj_egl_config_attr_match_5424")));
