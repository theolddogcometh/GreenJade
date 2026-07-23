/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6480: broadcast/stream continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bstream_continuum_6480(void);
 *     - Returns 1 (soft compile-time product tag: broadcast/streaming
 *       bitrate stubs continuum 6471-6480 is complete / ready).
 *   uint32_t gj_batch_id_6480(void);
 *     - Returns the compile-time graph batch number for this TU (6480).
 *   uint32_t __gj_bstream_continuum_6480  (alias)
 *   uint32_t __gj_batch_id_6480  (alias)
 *   __libcgj_batch6480_marker = "libcgj-batch6480"
 *
 * Exclusive continuum CREATE-ONLY (6471-6480: broadcast/streaming
 * bitrate stubs — bitrate_clamp_kbps_6471, cbr_target_ok_6472,
 * vbr_range_ok_6473, abr_rung_kbps_6474, bytes_to_kbps_6475,
 * audio_bitrate_ok_6476, video_budget_kbps_6477, mux_overhead_kbps_6478,
 * budget_gate_6479, continuum + batch_id_6480). Unique surfaces only;
 * no multi-def. Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6480_marker[] = "libcgj-batch6480";

/* Continuum-complete lamp for the 6471-6480 exclusive wave. */
#define B6480_CONTINUUM_READY  1u
#define B6480_BATCH_ID         6480u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6480_continuum(void)
{
	return B6480_CONTINUUM_READY;
}

static uint32_t
b6480_id(void)
{
	return B6480_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bstream_continuum_6480 - continuum-ready tag for 6471-6480.
 *
 * Always returns 1. Soft pure-data product tag that the
 * broadcast/streaming bitrate stubs exclusive wave is present.
 * Does not call libc. No parent wires.
 */
uint32_t
gj_bstream_continuum_6480(void)
{
	(void)NULL;
	return b6480_continuum();
}

/*
 * gj_batch_id_6480 - report this TU's graph batch number.
 *
 * Always returns 6480.
 */
uint32_t
gj_batch_id_6480(void)
{
	return b6480_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_bstream_continuum_6480(void)
    __attribute__((alias("gj_bstream_continuum_6480")));

uint32_t __gj_batch_id_6480(void)
    __attribute__((alias("gj_batch_id_6480")));
