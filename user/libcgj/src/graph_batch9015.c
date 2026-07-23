/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9015: Mesa/GL depth24 format soft identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_gl_depth24_id_9015(void);
 *     - Returns the soft 24-bit depth format id for the mesa/gl soft id
 *       continuum (always 0x81A6, matching classic GL_DEPTH_COMPONENT24).
 *       Soft pure-data only; not a depth buffer or attachment probe.
 *   uint32_t __gj_gl_depth24_id_9015  (alias)
 *   __libcgj_batch9015_marker = "libcgj-batch9015"
 *
 * Exclusive continuum CREATE-ONLY (9011-9020: mesa/gl soft id stubs —
 * gl_api_es2_id_9011, gl_api_es3_id_9012, gl_api_core_id_9013,
 * gl_rgba8_id_9014, gl_depth24_id_9015, gl_shader_ok_u_9016,
 * gl_program_ok_u_9017, gl_tex_ok_u_9018, mesa_ready_u_9019,
 * batch_id_9020). Unique surface only; no multi-def. Distinct from
 * gj_gl_rgba8_id_9014. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9015_marker[] = "libcgj-batch9015";

/* Soft depth24 format id (classic GL_DEPTH_COMPONENT24 = 0x81A6). */
#define B9015_GL_DEPTH24_ID  0x81A6u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9015_depth24_id(void)
{
	return B9015_GL_DEPTH24_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gl_depth24_id_9015 - soft 24-bit depth format identity tag.
 *
 * Always returns 0x81A6. Soft pure-data format id for the 9011-9020
 * mesa/gl continuum. Does not allocate depth buffers, query formats,
 * or call libc. No parent wires.
 */
uint32_t
gj_gl_depth24_id_9015(void)
{
	(void)NULL;
	return b9015_depth24_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_gl_depth24_id_9015(void)
    __attribute__((alias("gj_gl_depth24_id_9015")));
