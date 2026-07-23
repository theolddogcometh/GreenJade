/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6330: shader compiler backend id wave closer.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_6330(void);
 *     - Returns the compile-time graph batch number for this TU (6330).
 *   uint32_t gj_sc_backend_wave_ready_6330(void);
 *     - Returns 1 (shader compiler backend id continuum ready for wave
 *       6321-6330).
 *   uint32_t __gj_batch_id_6330  (alias)
 *   uint32_t __gj_sc_backend_wave_ready_6330  (alias)
 *   __libcgj_batch6330_marker = "libcgj-batch6330"
 *
 * Exclusive continuum CREATE-ONLY (6321-6330: shader compiler backend id
 * stubs — sc_backend_spirv_id_6321, sc_backend_dxil_id_6322,
 * sc_backend_msl_id_6323, sc_backend_glsl_id_6324,
 * sc_backend_hlsl_id_6325, sc_backend_wgsl_id_6326,
 * sc_backend_nir_id_6327, sc_backend_id_ok_6328,
 * sc_backend_id_pack_6329, batch_id / wave_ready_6330).
 * Does NOT redefine gj_batch_id / prior batch_id_*. Unique surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6330_marker[] = "libcgj-batch6330";

#define B6330_BATCH_ID    6330u
#define B6330_WAVE_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6330_id(void)
{
	return B6330_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_6330 - report this TU's graph batch number.
 *
 * Always returns 6330 (shader compiler backend id wave closer).
 * No parent wires.
 */
uint32_t
gj_batch_id_6330(void)
{
	(void)NULL;
	return b6330_id();
}

/*
 * gj_sc_backend_wave_ready_6330 - backend-id continuum ready lamp.
 *
 * Always returns 1 (ready). Soft pure-data only. No parent wires.
 */
uint32_t
gj_sc_backend_wave_ready_6330(void)
{
	return B6330_WAVE_READY;
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_6330(void)
    __attribute__((alias("gj_batch_id_6330")));

uint32_t __gj_sc_backend_wave_ready_6330(void)
    __attribute__((alias("gj_sc_backend_wave_ready_6330")));
