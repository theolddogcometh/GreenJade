/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9016: Mesa/GL shader ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_gl_shader_ok_u_9016(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       GLSL shader compile/status probe for the mesa/gl continuum.
 *   uint32_t __gj_gl_shader_ok_u_9016  (alias)
 *   __libcgj_batch9016_marker = "libcgj-batch9016"
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

const char __libcgj_batch9016_marker[] = "libcgj-batch9016";

/* Soft shader-ok lamp: always off (not a real compile probe). */
#define B9016_SHADER_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9016_shader_ok(void)
{
	return B9016_SHADER_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gl_shader_ok_u_9016 - GL shader ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not compile shaders,
 * query GL_COMPILE_STATUS, or call libc. No parent wires.
 */
uint32_t
gj_gl_shader_ok_u_9016(void)
{
	(void)NULL;
	return b9016_shader_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_gl_shader_ok_u_9016(void)
    __attribute__((alias("gj_gl_shader_ok_u_9016")));
