/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9013: Mesa/GL desktop core API soft identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_gl_api_core_id_9013(void);
 *     - Returns the soft OpenGL desktop core-profile API id tag for the
 *       mesa/gl soft id continuum (always 0x434F5245, fourCC "CORE").
 *       Not a GL context or driver probe.
 *   uint32_t __gj_gl_api_core_id_9013  (alias)
 *   __libcgj_batch9013_marker = "libcgj-batch9013"
 *
 * Exclusive continuum CREATE-ONLY (9011-9020: mesa/gl soft id stubs —
 * gl_api_es2_id_9011, gl_api_es3_id_9012, gl_api_core_id_9013,
 * gl_rgba8_id_9014, gl_depth24_id_9015, gl_shader_ok_u_9016,
 * gl_program_ok_u_9017, gl_tex_ok_u_9018, mesa_ready_u_9019,
 * batch_id_9020). Unique surface only; no multi-def. Distinct from
 * gj_gl_api_es2_id_9011 / gj_gl_api_es3_id_9012. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9013_marker[] = "libcgj-batch9013";

/* Soft OpenGL desktop core-profile API fourCC "CORE". */
#define B9013_GL_API_CORE_ID  0x434F5245u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9013_core_id(void)
{
	return B9013_GL_API_CORE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gl_api_core_id_9013 - soft OpenGL desktop core API identity tag.
 *
 * Always returns 0x434F5245 ("CORE"). Soft pure-data API id for the
 * 9011-9020 mesa/gl continuum. Does not open a GL context, load Mesa,
 * or call libc. No parent wires.
 */
uint32_t
gj_gl_api_core_id_9013(void)
{
	(void)NULL;
	return b9013_core_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_gl_api_core_id_9013(void)
    __attribute__((alias("gj_gl_api_core_id_9013")));
