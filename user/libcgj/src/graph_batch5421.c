/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5421: OpenGL ES version pack probe (Deck GPU).
 *
 * Surface (unique symbols):
 *   uint32_t gj_gles_version_pack_5421(uint32_t major, uint32_t minor);
 *     - Pack GLES major/minor into a single u32 as (major << 16) | minor.
 *       Saturates each field to 16 bits. Soft freestanding probe only;
 *       does not call real GLES entry points.
 *   uint32_t __gj_gles_version_pack_5421  (alias)
 *   __libcgj_batch5421_marker = "libcgj-batch5421"
 *
 * Exclusive continuum CREATE-ONLY (5421-5430: gles_version_pack_5421,
 * glx_fbconfig_rgba_bits_5422, egl_display_ok_5423,
 * egl_config_attr_match_5424, shader_compile_ok_5425,
 * shader_info_log_len_5426, tex_format_bpp_5427,
 * tex_format_supported_5428, deck_gpu_gles_ready_5429,
 * batch_id_5430). Deck GPU path: OpenGL ES / GLX / EGL / shader /
 * texture stubs. Unique gj_*_5421 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5421_marker[] = "libcgj-batch5421";

/* 16-bit field mask for major/minor packing. */
#define B5421_FIELD_MASK  0xffffu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5421_pack(uint32_t uMajor, uint32_t uMinor)
{
	return ((uMajor & B5421_FIELD_MASK) << 16) |
	       (uMinor & B5421_FIELD_MASK);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gles_version_pack_5421 - pack GLES major/minor into one u32.
 *
 * major: GLES major version (e.g. 2 or 3); high bits masked to 16.
 * minor: GLES minor version; high bits masked to 16.
 * Returns (major << 16) | minor for soft Deck GPU path version probes.
 * Self-contained; no parent wires.
 */
uint32_t
gj_gles_version_pack_5421(uint32_t uMajor, uint32_t uMinor)
{
	(void)NULL;
	return b5421_pack(uMajor, uMinor);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_gles_version_pack_5421(uint32_t uMajor, uint32_t uMinor)
    __attribute__((alias("gj_gles_version_pack_5421")));
