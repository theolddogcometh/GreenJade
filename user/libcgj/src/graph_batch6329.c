/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6329: shader compiler backend id pack stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sc_backend_id_pack_6329(uint32_t primary_id,
 *                                      uint32_t secondary_id);
 *     - Pack soft backend id fields into one summary word:
 *         bits  0..15 = primary_id   clamped to 0..0xffff
 *         bits 16..31 = secondary_id clamped to 0..0xffff
 *       Compact probe tag for dual-target shader compiler routing.
 *   uint32_t __gj_sc_backend_id_pack_6329  (alias)
 *   __libcgj_batch6329_marker = "libcgj-batch6329"
 *
 * Exclusive continuum CREATE-ONLY (6321-6330: shader compiler backend id
 * stubs — sc_backend_spirv_id_6321, sc_backend_dxil_id_6322,
 * sc_backend_msl_id_6323, sc_backend_glsl_id_6324,
 * sc_backend_hlsl_id_6325, sc_backend_wgsl_id_6326,
 * sc_backend_nir_id_6327, sc_backend_id_ok_6328,
 * sc_backend_id_pack_6329, batch_id / wave_ready_6330).
 * Unique surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6329_marker[] = "libcgj-batch6329";

#define B6329_U16_MAX  0xffffu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6329_clamp16(uint32_t u32V)
{
	if (u32V > B6329_U16_MAX) {
		return B6329_U16_MAX;
	}
	return u32V;
}

static uint32_t
b6329_pack(uint32_t u32Pri, uint32_t u32Sec)
{
	return b6329_clamp16(u32Pri) | (b6329_clamp16(u32Sec) << 16);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sc_backend_id_pack_6329 - pack primary + secondary backend ids.
 *
 * primary_id:   first soft backend enumerator (clamped to 16 bits)
 * secondary_id: second soft backend enumerator (clamped to 16 bits)
 *
 * Returns packed summary word. Soft pure-data only. No parent wires.
 */
uint32_t
gj_sc_backend_id_pack_6329(uint32_t primary_id, uint32_t secondary_id)
{
	(void)NULL;
	return b6329_pack(primary_id, secondary_id);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sc_backend_id_pack_6329(uint32_t primary_id,
    uint32_t secondary_id)
    __attribute__((alias("gj_sc_backend_id_pack_6329")));
