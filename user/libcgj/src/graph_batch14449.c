/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14449: scsi mid soft ready lamp (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_scsi_mid_soft_ready_u_14449(void);
 *     - Always returns 0. Soft continuum ready lamp intentionally off;
 *       pure-data stub for the exclusive wave 14441-14450 surfaces.
 *   uint32_t __gj_scsi_mid_soft_ready_u_14449  (alias)
 *   __libcgj_batch14449_marker = "libcgj-batch14449"
 *
 * Exclusive continuum CREATE-ONLY (14441-14450: scsi mid soft
 * stubs — ok slots all→0; soft_ready→0; batch_id→14450). Unique gj_scsi_mid_soft_ready_u_14449
 * surface only; no multi-def. Distinct from gj_scsi_smart_ok_u_9768 and sibling scsi mid
 * soft stubs in this wave. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14449_marker[] = "libcgj-batch14449";

/* SCSI mid soft lamp: always off. */
#define B14449_SCSI_MID  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14449_ok(void)
{
	return B14449_SCSI_MID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_scsi_mid_soft_ready_u_14449 - scsi mid soft ready lamp
 *
 * Always returns 0u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_scsi_mid_soft_ready_u_14449(void)
{
	(void)NULL;
	return b14449_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_scsi_mid_soft_ready_u_14449(void)
    __attribute__((alias("gj_scsi_mid_soft_ready_u_14449")));
