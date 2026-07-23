/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5369: OpenGL GL_TRIANGLES primitive constant.
 *
 * Surface (unique symbols):
 *   uint32_t gj_gl_triangles_u(void);
 *     - Returns the OpenGL triangles primitive mode (0x0004 /
 *       GL_TRIANGLES).
 *   uint32_t __gj_gl_triangles_u  (alias)
 *   __libcgj_batch5369_marker = "libcgj-batch5369"
 *
 * Exclusive continuum CREATE-ONLY (5361-5370: vk_api_1_0_u, vk_api_1_1_u,
 * vk_api_1_2_u, vk_api_1_3_u, vk_success_u, vk_is_success_u, gl_true_u,
 * gl_false_u, gl_triangles_u, batch_id_5370). Unique gj_gl_triangles_u
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5369_marker[] = "libcgj-batch5369";

/* OpenGL GL_TRIANGLES primitive mode. */
#define B5369_GL_TRIANGLES  ((uint32_t)0x0004u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5369_gl_triangles(void)
{
	return B5369_GL_TRIANGLES;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gl_triangles_u - report the OpenGL GL_TRIANGLES primitive mode.
 *
 * Always returns 0x0004. Soft compile-time graphics API tag; does not
 * call GL or libc. No parent wires.
 */
uint32_t
gj_gl_triangles_u(void)
{
	(void)NULL;
	return b5369_gl_triangles();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_gl_triangles_u(void)
    __attribute__((alias("gj_gl_triangles_u")));
