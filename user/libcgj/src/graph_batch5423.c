/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5423: EGL display handle validity probe.
 *
 * Surface (unique symbols):
 *   uint32_t gj_egl_display_ok_5423(uint64_t display);
 *     - Return 1 when display is a non-zero soft EGLDisplay token,
 *       else 0. Models EGL_NO_DISPLAY rejection without calling libEGL.
 *   uint32_t __gj_egl_display_ok_5423  (alias)
 *   __libcgj_batch5423_marker = "libcgj-batch5423"
 *
 * Exclusive continuum CREATE-ONLY (5421-5430). Deck GPU path: OpenGL ES
 * / GLX / EGL / shader / texture stubs. Unique gj_*_5423 surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5423_marker[] = "libcgj-batch5423";

/* Soft EGL_NO_DISPLAY token (integer model of NULL display). */
#define B5423_EGL_NO_DISPLAY  0ull

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5423_display_ok(uint64_t u64Disp)
{
	return (u64Disp != B5423_EGL_NO_DISPLAY) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_egl_display_ok_5423 - soft EGL display non-null probe.
 *
 * display: opaque EGLDisplay token as u64 (0 = EGL_NO_DISPLAY).
 * Returns 1 if display is usable (non-zero), else 0. Freestanding
 * integer stub for Deck GPU EGL path; no libEGL. No parent wires.
 */
uint32_t
gj_egl_display_ok_5423(uint64_t u64Disp)
{
	(void)NULL;
	return b5423_display_ok(u64Disp);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_egl_display_ok_5423(uint64_t u64Disp)
    __attribute__((alias("gj_egl_display_ok_5423")));
