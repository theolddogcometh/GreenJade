/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14647: scsi queue ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_scsi_queue_ok_u_14647(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       SCSI queue probe for the scsi mid soft continuum.
 *   uint32_t __gj_scsi_queue_ok_u_14647  (alias)
 *   __libcgj_batch14647_marker = "libcgj-batch14647"
 *
 * Exclusive continuum CREATE-ONLY (14641-14650: scsi mid soft
 * stubs — ok slots all→0; soft_ready→0; batch_id→14650). Unique surface
 * only; no multi-def. Distinct from sibling scsi mid soft stubs in prior waves.
 * No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14647_marker[] = "libcgj-batch14647";

/* SCSI mid soft lamp: always off. */
#define B14647_SCSI  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14647_ok(void)
{
	return B14647_SCSI;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_scsi_queue_ok_u_14647 - soft continuum surface
 *
 * Always returns 0u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_scsi_queue_ok_u_14647(void)
{
	(void)NULL;
	return b14647_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_scsi_queue_ok_u_14647(void)
    __attribute__((alias("gj_scsi_queue_ok_u_14647")));
