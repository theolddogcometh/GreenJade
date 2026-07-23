/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14447: scsi queue ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_scsi_queue_ok_u_14447(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       SCSI request-queue probe.
 *   uint32_t __gj_scsi_queue_ok_u_14447  (alias)
 *   __libcgj_batch14447_marker = "libcgj-batch14447"
 *
 * Exclusive continuum CREATE-ONLY (14441-14450: scsi mid soft
 * stubs — ok slots all→0; soft_ready→0; batch_id→14450). Unique gj_scsi_queue_ok_u_14447
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

const char __libcgj_batch14447_marker[] = "libcgj-batch14447";

/* SCSI mid soft lamp: always off. */
#define B14447_SCSI_MID  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14447_ok(void)
{
	return B14447_SCSI_MID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_scsi_queue_ok_u_14447 - scsi queue ok soft unit
 *
 * Always returns 0u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_scsi_queue_ok_u_14447(void)
{
	(void)NULL;
	return b14447_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_scsi_queue_ok_u_14447(void)
    __attribute__((alias("gj_scsi_queue_ok_u_14447")));
