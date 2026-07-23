/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6328: shader compiler backend id soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sc_backend_id_ok_6328(uint32_t backend_id);
 *     - Returns 1 if backend_id is in the known continuum range
 *       [1..7] (SPIR-V..NIR), else 0. Soft pure-data gate.
 *   uint32_t __gj_sc_backend_id_ok_6328  (alias)
 *   __libcgj_batch6328_marker = "libcgj-batch6328"
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

const char __libcgj_batch6328_marker[] = "libcgj-batch6328";

/* Known backend id range: 1 (SPIR-V) .. 7 (NIR). */
#define B6328_ID_MIN  1u
#define B6328_ID_MAX  7u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6328_id_ok(uint32_t u32Id)
{
	if (u32Id < B6328_ID_MIN) {
		return 0u;
	}
	if (u32Id > B6328_ID_MAX) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sc_backend_id_ok_6328 - 1 if backend_id is a known continuum id.
 *
 * backend_id: candidate soft backend enumerator
 *
 * Accepts 1..7 only. Soft pure-data compare. No parent wires.
 */
uint32_t
gj_sc_backend_id_ok_6328(uint32_t backend_id)
{
	(void)NULL;
	return b6328_id_ok(backend_id);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sc_backend_id_ok_6328(uint32_t backend_id)
    __attribute__((alias("gj_sc_backend_id_ok_6328")));
