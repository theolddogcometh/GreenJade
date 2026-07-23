/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6860: openat2 resolve flags continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_oat2_resolve_continuum_6860(void);
 *     - Returns 1 (soft compile-time product tag: openat2 resolve flags
 *       stub continuum 6851-6860 is complete / ready).
 *   uint32_t gj_batch_id_6860(void);
 *     - Returns the compile-time graph batch number for this TU (6860).
 *   uint32_t __gj_oat2_resolve_continuum_6860  (alias)
 *   uint32_t __gj_batch_id_6860  (alias)
 *   __libcgj_batch6860_marker = "libcgj-batch6860"
 *
 * Exclusive continuum CREATE-ONLY (6851-6860: openat2 resolve flags
 * stubs —
 * resolve_no_xdev_6851, resolve_no_magiclinks_6852,
 * resolve_no_symlinks_6853, resolve_beneath_6854, resolve_in_root_6855,
 * resolve_cached_6856, resolve_known_mask_6857, resolve_bit_count_6858,
 * resolve_flag_ok_6859, continuum + batch_id_6860). Unique surfaces
 * only; no multi-def. Does NOT redefine gj_batch_id / gj_graph_milestone
 * / prior batch_id_* symbols — avoid multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6860_marker[] = "libcgj-batch6860";

/* Continuum-complete lamp for the 6851-6860 exclusive wave. */
#define B6860_CONTINUUM_READY  1u
#define B6860_BATCH_ID         6860u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6860_continuum(void)
{
	return B6860_CONTINUUM_READY;
}

static uint32_t
b6860_id(void)
{
	return B6860_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_oat2_resolve_continuum_6860 - continuum-ready tag for 6851-6860.
 *
 * Always returns 1. Soft pure-data product tag that the openat2 resolve
 * flags stub exclusive wave is present. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_oat2_resolve_continuum_6860(void)
{
	(void)NULL;
	return b6860_continuum();
}

/*
 * gj_batch_id_6860 - report this TU's graph batch number.
 *
 * Always returns 6860.
 */
uint32_t
gj_batch_id_6860(void)
{
	return b6860_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_oat2_resolve_continuum_6860(void)
    __attribute__((alias("gj_oat2_resolve_continuum_6860")));

uint32_t __gj_batch_id_6860(void)
    __attribute__((alias("gj_batch_id_6860")));
