/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9019: Mesa/GL soft continuum ready lamp (on).
 *
 * Surface (unique symbols):
 *   uint32_t gj_mesa_ready_u_9019(void);
 *     - Returns 1 (soft lamp only). Indicates the mesa/gl soft-id stub
 *       surfaces in this continuum are present; not a real Mesa driver
 *       or GL runtime readiness probe.
 *   uint32_t __gj_mesa_ready_u_9019  (alias)
 *   __libcgj_batch9019_marker = "libcgj-batch9019"
 *
 * Exclusive continuum CREATE-ONLY (9011-9020: mesa/gl soft id stubs —
 * gl_api_es2_id_9011, gl_api_es3_id_9012, gl_api_core_id_9013,
 * gl_rgba8_id_9014, gl_depth24_id_9015, gl_shader_ok_u_9016,
 * gl_program_ok_u_9017, gl_tex_ok_u_9018, mesa_ready_u_9019,
 * batch_id_9020). Unique surface only; no multi-def. Shader/program/
 * tex ok units remain 0. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9019_marker[] = "libcgj-batch9019";

/* Soft continuum lamp: surfaces present (soft ready only). */
#define B9019_MESA_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9019_mesa_ready(void)
{
	return B9019_MESA_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mesa_ready_u_9019 - mesa/gl soft continuum ready lamp.
 *
 * Always returns 1. Soft pure-data product tag that exclusive wave
 * 9011-9020 surfaces are present. Does not claim shader/program/tex
 * ready and does not call libc or load Mesa. No parent wires.
 */
uint32_t
gj_mesa_ready_u_9019(void)
{
	(void)NULL;
	return b9019_mesa_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mesa_ready_u_9019(void)
    __attribute__((alias("gj_mesa_ready_u_9019")));
