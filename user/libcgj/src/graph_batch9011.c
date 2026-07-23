/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9011: Mesa/GL ES2 API soft identity tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_gl_api_es2_id_9011(void);
 *     - Returns the soft OpenGL ES 2.0 API id tag for the mesa/gl soft
 *       id continuum (always 0x45533200, fourCC "ES2\0"). Not a GL
 *       context or driver probe.
 *   uint32_t __gj_gl_api_es2_id_9011  (alias)
 *   __libcgj_batch9011_marker = "libcgj-batch9011"
 *
 * Exclusive continuum CREATE-ONLY (9011-9020: mesa/gl soft id stubs —
 * gl_api_es2_id_9011, gl_api_es3_id_9012, gl_api_core_id_9013,
 * gl_rgba8_id_9014, gl_depth24_id_9015, gl_shader_ok_u_9016,
 * gl_program_ok_u_9017, gl_tex_ok_u_9018, mesa_ready_u_9019,
 * batch_id_9020). Unique surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9011_marker[] = "libcgj-batch9011";

/* Soft OpenGL ES 2.0 API fourCC "ES2\0". */
#define B9011_GL_API_ES2_ID  0x45533200u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9011_es2_id(void)
{
	return B9011_GL_API_ES2_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gl_api_es2_id_9011 - soft OpenGL ES 2.0 API identity tag.
 *
 * Always returns 0x45533200 ("ES2\0"). Soft pure-data API id for the
 * 9011-9020 mesa/gl continuum. Does not open a GL context, load Mesa,
 * or call libc. No parent wires.
 */
uint32_t
gj_gl_api_es2_id_9011(void)
{
	(void)NULL;
	return b9011_es2_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_gl_api_es2_id_9011(void)
    __attribute__((alias("gj_gl_api_es2_id_9011")));
