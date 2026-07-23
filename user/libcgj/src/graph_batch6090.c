/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6090: SD path continuum + batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sd_path_continuum_6090(void);
 *     - Returns 1 (soft compile-time product tag: SD / microSD path
 *       stub continuum 6081-6090 is complete / ready).
 *   uint32_t gj_batch_id_6090(void);
 *     - Returns the compile-time graph batch number for this TU (6090).
 *   uint32_t __gj_sd_path_continuum_6090  (alias)
 *   uint32_t __gj_batch_id_6090  (alias)
 *   __libcgj_batch6090_marker = "libcgj-batch6090"
 *
 * Exclusive continuum CREATE-ONLY (6081-6090: SD card / microSD path
 * stubs —
 * sd_dev_mmcblk_prefix_len_6081, sd_mmcblk_index_ok_6082,
 * sd_part_index_ok_6083, microsd_sysfs_block_prefix_len_6084,
 * sd_mount_path_max_6085, sd_path_join_need_6086,
 * microsd_by_id_prefix_len_6087, sd_path_is_abs_hint_6088,
 * sd_path_stub_ready_6089, continuum + batch_id_6090). Unique surfaces
 * only; no multi-def. Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6090_marker[] = "libcgj-batch6090";

/* Continuum-complete lamp for the 6081-6090 exclusive wave. */
#define B6090_CONTINUUM_READY  1u
#define B6090_BATCH_ID         6090u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6090_continuum(void)
{
	return B6090_CONTINUUM_READY;
}

static uint32_t
b6090_id(void)
{
	return B6090_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sd_path_continuum_6090 - continuum-ready tag for 6081-6090.
 *
 * Always returns 1. Soft pure-data product tag that the SD card /
 * microSD path stub exclusive wave is present. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_sd_path_continuum_6090(void)
{
	(void)NULL;
	return b6090_continuum();
}

/*
 * gj_batch_id_6090 - report this TU's graph batch number.
 *
 * Always returns 6090.
 */
uint32_t
gj_batch_id_6090(void)
{
	return b6090_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_sd_path_continuum_6090(void)
    __attribute__((alias("gj_sd_path_continuum_6090")));

uint32_t __gj_batch_id_6090(void)
    __attribute__((alias("gj_batch_id_6090")));
