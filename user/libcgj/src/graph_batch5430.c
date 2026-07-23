/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5430: MILESTONE 5430 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_5430(void);
 *     - Returns the compile-time graph batch number for this TU (5430).
 *   uint32_t gj_graph_milestone_5430(void);
 *     - Returns the current graph milestone revision (5430).
 *   uint32_t __gj_batch_id_5430  (alias)
 *   uint32_t __gj_graph_milestone_5430  (alias)
 *   __libcgj_batch5430_marker = "libcgj-batch5430"
 *
 * MILESTONE 5430 for the exclusive continuum CREATE-ONLY wave
 * (batches 5421-5429: gles_version_pack_5421, glx_fbconfig_rgba_bits_5422,
 * egl_display_ok_5423, egl_config_attr_match_5424, shader_compile_ok_5425,
 * shader_info_log_len_5426, tex_format_bpp_5427, tex_format_supported_5428,
 * deck_gpu_gles_ready_5429). Deck GPU path: OpenGL ES / GLX / EGL /
 * shader / texture stubs. Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5430_marker[] = "libcgj-batch5430";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5430_id(void)
{
	return 5430u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_5430 - report this TU's graph batch number.
 *
 * Always returns 5430.
 */
uint32_t
gj_batch_id_5430(void)
{
	(void)NULL;
	return b5430_id();
}

/*
 * gj_graph_milestone_5430 - report this TU's graph milestone revision.
 *
 * Always returns 5430 (MILESTONE 5430). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_5430(void)
{
	return b5430_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_5430(void)
    __attribute__((alias("gj_batch_id_5430")));

uint32_t __gj_graph_milestone_5430(void)
    __attribute__((alias("gj_graph_milestone_5430")));
