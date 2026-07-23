/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6950: sendfile offset continuum lamp.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sendfile_offset_continuum_ok_6950(void);
 *     - Returns 1 (soft lamp: 6941-6950 sendfile offset stubs
 *       continuum complete / ready).
 *   uint32_t gj_batch_id_6950(void);
 *     - Returns the compile-time graph batch number for this TU (6950).
 *   uint32_t __gj_sendfile_offset_continuum_ok_6950  (alias)
 *   uint32_t __gj_batch_id_6950  (alias)
 *   __libcgj_batch6950_marker = "libcgj-batch6950"
 *
 * Exclusive continuum CREATE-ONLY (6941-6950: sendfile offset stubs —
 * word_bits_6941, null_means_cur_6942, remaining_6943, count_clamp_6944,
 * advance_6945, plan_count_6946, offset_ok_6947, add_sat_6948,
 * at_eof/past_eof_6949, continuum + batch_id_6950). Unique surfaces
 * only; no multi-def. Does NOT redefine bare gj_batch_id.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6950_marker[] = "libcgj-batch6950";

#define B6950_CONTINUUM_OK  1u
#define B6950_BATCH_ID      6950u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6950_continuum(void)
{
	return B6950_CONTINUUM_OK;
}

static uint32_t
b6950_id(void)
{
	return B6950_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sendfile_offset_continuum_ok_6950 - continuum-ready tag for 6941-6950.
 *
 * Always returns 1. Soft pure-data product tag that the sendfile offset
 * stubs exclusive wave is present. Does not call libc.
 */
uint32_t
gj_sendfile_offset_continuum_ok_6950(void)
{
	(void)NULL;
	return b6950_continuum();
}

/*
 * gj_batch_id_6950 - report this TU's graph batch number.
 *
 * Always returns 6950.
 */
uint32_t
gj_batch_id_6950(void)
{
	return b6950_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_sendfile_offset_continuum_ok_6950(void)
    __attribute__((alias("gj_sendfile_offset_continuum_ok_6950")));

uint32_t __gj_batch_id_6950(void)
    __attribute__((alias("gj_batch_id_6950")));
