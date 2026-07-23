/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6820: memfd seal continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mfd_seal_continuum_6820(void);
 *     - Returns 1 (soft compile-time product tag: memfd seal stub
 *       continuum 6811-6820 is complete / ready).
 *   uint32_t gj_batch_id_6820(void);
 *     - Returns the compile-time graph batch number for this TU (6820).
 *   uint32_t __gj_mfd_seal_continuum_6820  (alias)
 *   uint32_t __gj_batch_id_6820  (alias)
 *   __libcgj_batch6820_marker = "libcgj-batch6820"
 *
 * Exclusive continuum CREATE-ONLY (6811-6820: memfd seal stubs —
 * cloexec_6811, allow_sealing_6812, hugetlb_6813, fseal_seal_6814,
 * fseal_shrink_6815, fseal_grow_6816, fseal_write_6817,
 * fseal_future_write_6818, seal_ok_6819, continuum + batch_id_6820).
 * Unique surfaces only; no multi-def. Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6820_marker[] = "libcgj-batch6820";

/* Continuum-complete lamp for the 6811-6820 exclusive wave. */
#define B6820_CONTINUUM_READY  1u
#define B6820_BATCH_ID         6820u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6820_continuum(void)
{
	return B6820_CONTINUUM_READY;
}

static uint32_t
b6820_id(void)
{
	return B6820_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mfd_seal_continuum_6820 - continuum-ready tag for 6811-6820.
 *
 * Always returns 1. Soft pure-data product tag that the memfd seal stub
 * exclusive wave is present. Does not call libc. No parent wires.
 */
uint32_t
gj_mfd_seal_continuum_6820(void)
{
	(void)NULL;
	return b6820_continuum();
}

/*
 * gj_batch_id_6820 - report this TU's graph batch number.
 *
 * Always returns 6820.
 */
uint32_t
gj_batch_id_6820(void)
{
	return b6820_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_mfd_seal_continuum_6820(void)
    __attribute__((alias("gj_mfd_seal_continuum_6820")));

uint32_t __gj_batch_id_6820(void)
    __attribute__((alias("gj_batch_id_6820")));
