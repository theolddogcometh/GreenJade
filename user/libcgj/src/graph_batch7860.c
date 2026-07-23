/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7860: iconv codeset continuum lamp.
 *
 * Surface (unique symbols):
 *   uint32_t gj_iconv_codeset_continuum_ok_7860(void);
 *     - Returns 1 (soft lamp: 7851-7860 iconv codeset stubs
 *       continuum complete / ready).
 *   uint32_t gj_batch_id_7860(void);
 *     - Returns the compile-time graph batch number for this TU (7860).
 *   uint32_t __gj_iconv_codeset_continuum_ok_7860  (alias)
 *   uint32_t __gj_batch_id_7860  (alias)
 *   __libcgj_batch7860_marker = "libcgj-batch7860"
 *
 * Exclusive continuum CREATE-ONLY (7851-7860: iconv codeset stubs —
 * name_max, name_len_ok, nonempty, utf8_id, ascii_id, latin1_id,
 * unknown_id, same_class_ok, pair_req_ok, continuum + batch_id_7860).
 * Unique surfaces only; no multi-def. Does NOT redefine bare
 * gj_batch_id. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7860_marker[] = "libcgj-batch7860";

#define B7860_CONTINUUM_OK  1u
#define B7860_BATCH_ID      7860u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7860_continuum(void)
{
	return B7860_CONTINUUM_OK;
}

static uint32_t
b7860_id(void)
{
	return B7860_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_iconv_codeset_continuum_ok_7860 - continuum-ready tag for 7851-7860.
 *
 * Always returns 1. Soft pure-data product tag that the iconv codeset
 * stubs exclusive wave is present. Does not call libc.
 */
uint32_t
gj_iconv_codeset_continuum_ok_7860(void)
{
	(void)NULL;
	return b7860_continuum();
}

/*
 * gj_batch_id_7860 - report this TU's graph batch number.
 *
 * Always returns 7860.
 */
uint32_t
gj_batch_id_7860(void)
{
	return b7860_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_iconv_codeset_continuum_ok_7860(void)
    __attribute__((alias("gj_iconv_codeset_continuum_ok_7860")));

uint32_t __gj_batch_id_7860(void)
    __attribute__((alias("gj_batch_id_7860")));
