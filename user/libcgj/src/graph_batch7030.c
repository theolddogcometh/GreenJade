/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7030: rseq signature continuum lamp + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rseq_sig_continuum_ok_7030(void);
 *     - Returns 1 (soft lamp: 7021-7030 rseq signature stubs continuum
 *       complete / ready).
 *   uint32_t gj_batch_id_7030(void);
 *     - Returns the compile-time graph batch number for this TU (7030).
 *   uint32_t __gj_rseq_sig_continuum_ok_7030  (alias)
 *   uint32_t __gj_batch_id_7030  (alias)
 *   __libcgj_batch7030_marker = "libcgj-batch7030"
 *
 * Exclusive continuum CREATE-ONLY (7021-7030: rseq signature stubs —
 * sig_7021, len_min_7022, flag_unregister_7023, cs_size_7024,
 * cpu_id_special_7025, cs_flags_7026, sig_match_7027, len_ok_7028,
 * flags_ok_7029, continuum + batch_id_7030). Unique surfaces only; no
 * multi-def. Does NOT redefine bare gj_batch_id. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7030_marker[] = "libcgj-batch7030";

#define B7030_CONTINUUM_OK  1u
#define B7030_BATCH_ID      7030u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7030_continuum(void)
{
	return B7030_CONTINUUM_OK;
}

static uint32_t
b7030_id(void)
{
	return B7030_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rseq_sig_continuum_ok_7030 - continuum-ready tag for 7021-7030.
 *
 * Always returns 1. Soft pure-data product tag that the rseq signature
 * stubs exclusive wave is present. Does not call libc.
 */
uint32_t
gj_rseq_sig_continuum_ok_7030(void)
{
	(void)NULL;
	return b7030_continuum();
}

/*
 * gj_batch_id_7030 - report this TU's graph batch number.
 *
 * Always returns 7030.
 */
uint32_t
gj_batch_id_7030(void)
{
	return b7030_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_rseq_sig_continuum_ok_7030(void)
    __attribute__((alias("gj_rseq_sig_continuum_ok_7030")));

uint32_t __gj_batch_id_7030(void)
    __attribute__((alias("gj_batch_id_7030")));
