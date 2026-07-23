/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6240: HDR PQ/HLG transfer continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_hdr_pq_hlg_transfer_continuum_6240(void);
 *     - Returns 1 (soft compile-time product tag: HDR PQ/HLG transfer
 *       stubs continuum 6231-6240 is complete / ready).
 *   uint32_t gj_batch_id_6240(void);
 *     - Returns the compile-time graph batch number for this TU (6240).
 *   uint32_t __gj_hdr_pq_hlg_transfer_continuum_6240  (alias)
 *   uint32_t __gj_batch_id_6240  (alias)
 *   __libcgj_batch6240_marker = "libcgj-batch6240"
 *
 * Exclusive continuum CREATE-ONLY (6231-6240: HDR PQ/HLG transfer stubs —
 * pq_code_clamp_6231, pq_nits_clamp_6232, pq_nits_to_norm_milli_6233,
 * pq_norm_milli_to_nits_6234, hlg_signal_clamp_6235, hlg_lw_ok_6236,
 * hlg_system_gamma_milli_6237, transfer_id_ok_6238,
 * pq_hlg_peak_ratio_milli_6239, continuum + batch_id_6240). Unique
 * surfaces only; no multi-def. Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6240_marker[] = "libcgj-batch6240";

/* Continuum-complete lamp for the 6231-6240 exclusive wave. */
#define B6240_CONTINUUM_READY  1u
#define B6240_BATCH_ID         6240u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6240_continuum(void)
{
	return B6240_CONTINUUM_READY;
}

static uint32_t
b6240_id(void)
{
	return B6240_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hdr_pq_hlg_transfer_continuum_6240 - continuum-ready tag for 6231-6240.
 *
 * Always returns 1. Soft pure-data product tag that the HDR PQ/HLG
 * transfer stubs exclusive wave is present. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_hdr_pq_hlg_transfer_continuum_6240(void)
{
	(void)NULL;
	return b6240_continuum();
}

/*
 * gj_batch_id_6240 - report this TU's graph batch number.
 *
 * Always returns 6240.
 */
uint32_t
gj_batch_id_6240(void)
{
	return b6240_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_hdr_pq_hlg_transfer_continuum_6240(void)
    __attribute__((alias("gj_hdr_pq_hlg_transfer_continuum_6240")));

uint32_t __gj_batch_id_6240(void)
    __attribute__((alias("gj_batch_id_6240")));
