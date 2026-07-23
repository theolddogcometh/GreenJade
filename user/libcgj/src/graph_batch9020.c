/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9020: mesa/gl soft continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_9020(void);
 *     - Returns the compile-time graph batch number for this TU (9020).
 *   uint32_t __gj_batch_id_9020  (alias)
 *   __libcgj_batch9020_marker = "libcgj-batch9020"
 *
 * Exclusive continuum CREATE-ONLY (9011-9020: mesa/gl soft id stubs —
 * gl_api_es2_id_9011, gl_api_es3_id_9012, gl_api_core_id_9013,
 * gl_rgba8_id_9014, gl_depth24_id_9015, gl_shader_ok_u_9016,
 * gl_program_ok_u_9017, gl_tex_ok_u_9018, mesa_ready_u_9019,
 * batch_id_9020). Unique surfaces only; no multi-def. Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* symbols
 * — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9020_marker[] = "libcgj-batch9020";

#define B9020_BATCH_ID  9020u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9020_id(void)
{
	return B9020_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_9020 - report this TU's graph batch number.
 *
 * Always returns 9020.
 */
uint32_t
gj_batch_id_9020(void)
{
	(void)NULL;
	return b9020_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_9020(void)
    __attribute__((alias("gj_batch_id_9020")));
