/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7460: linkat flags continuum lamp.
 *
 * Surface (unique symbols):
 *   uint32_t gj_linkat_flags_continuum_ok_7460(void);
 *     - Returns 1 (soft lamp: 7451-7460 linkat flags stubs
 *       continuum complete / ready).
 *   uint32_t gj_batch_id_7460(void);
 *     - Returns the compile-time graph batch number for this TU (7460).
 *   uint32_t __gj_linkat_flags_continuum_ok_7460  (alias)
 *   uint32_t __gj_batch_id_7460  (alias)
 *   __libcgj_batch7460_marker = "libcgj-batch7460"
 *
 * Exclusive continuum CREATE-ONLY (7451-7460: linkat flags stubs —
 * symlink_follow, empty_path, both_mask, known_flags, known_flags_ok,
 * has_known, both_u, mask_u, any_u, continuum + batch_id_7460).
 * Unique surfaces only; no multi-def. Does NOT redefine bare gj_batch_id.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7460_marker[] = "libcgj-batch7460";

#define B7460_CONTINUUM_OK  1u
#define B7460_BATCH_ID      7460u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7460_continuum(void)
{
	return B7460_CONTINUUM_OK;
}

static uint32_t
b7460_id(void)
{
	return B7460_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_linkat_flags_continuum_ok_7460 - continuum-ready tag for 7451-7460.
 *
 * Always returns 1. Soft pure-data product tag that the linkat flags
 * stubs exclusive wave is present. Does not call libc.
 */
uint32_t
gj_linkat_flags_continuum_ok_7460(void)
{
	(void)NULL;
	return b7460_continuum();
}

/*
 * gj_batch_id_7460 - report this TU's graph batch number.
 *
 * Always returns 7460.
 */
uint32_t
gj_batch_id_7460(void)
{
	return b7460_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_linkat_flags_continuum_ok_7460(void)
    __attribute__((alias("gj_linkat_flags_continuum_ok_7460")));

uint32_t __gj_batch_id_7460(void)
    __attribute__((alias("gj_batch_id_7460")));
