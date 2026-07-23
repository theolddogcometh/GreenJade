/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6322: shader compiler DXIL backend id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sc_backend_dxil_id_6322(void);
 *     - Returns the soft DXIL (DirectX Intermediate Language) shader-
 *       compiler backend id (always 2). Integer-only routing tag.
 *   uint32_t __gj_sc_backend_dxil_id_6322  (alias)
 *   __libcgj_batch6322_marker = "libcgj-batch6322"
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

const char __libcgj_batch6322_marker[] = "libcgj-batch6322";

/* Soft DXIL backend enumerator (shader compiler continuum). */
#define B6322_DXIL_ID  2u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6322_dxil_id(void)
{
	return B6322_DXIL_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sc_backend_dxil_id_6322 - soft DXIL backend id.
 *
 * Always returns 2. Compile-time backend tag for freestanding shader
 * compiler routing; does not invoke a real compiler. No parent wires.
 */
uint32_t
gj_sc_backend_dxil_id_6322(void)
{
	(void)NULL;
	return b6322_dxil_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sc_backend_dxil_id_6322(void)
    __attribute__((alias("gj_sc_backend_dxil_id_6322")));
