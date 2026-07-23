/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1967: OpenGL enum soft-ok hint.
 *
 * Surface (unique symbols):
 *   int gj_gl_enum_ok_hint(uint32_t e);
 *     — Soft validity hint for a GL enumeration token: return 1 if
 *       e is non-zero and within the classic 16-bit GL token band
 *       (1..0xFFFF), else 0. Not a guarantee the token is defined or
 *       accepted by a particular GL context; pure range data only.
 *   int __gj_gl_enum_ok_hint  (alias)
 *   __libcgj_batch1967_marker = "libcgj-batch1967"
 *
 * Vulkan/GL enum exclusive pure-data wave (1961–1970). Clean-room
 * freestanding pure C (integer only). Compiles with -ffreestanding
 * -msse2 -Wall -Wextra -Werror. No malloc, no errno, no libc. No
 * third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1967_marker[] = "libcgj-batch1967";

/* Classic GL enum tokens live in a 16-bit-ish public band. */
#define B1967_GL_ENUM_MIN  1u
#define B1967_GL_ENUM_MAX  0xffffu

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if e is in [1, 0xFFFF]. */
static int
b1967_enum_ok(uint32_t uE)
{
	return (uE >= B1967_GL_ENUM_MIN && uE <= B1967_GL_ENUM_MAX) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gl_enum_ok_hint — soft ok if 1 <= e <= 0xFFFF.
 *
 * e: GLenum-style token as uint32_t
 * Returns 1 when in soft range, else 0 (including 0 / out-of-band).
 */
int
gj_gl_enum_ok_hint(uint32_t uE)
{
	(void)NULL;
	return b1967_enum_ok(uE);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_gl_enum_ok_hint(uint32_t uE)
    __attribute__((alias("gj_gl_enum_ok_hint")));
