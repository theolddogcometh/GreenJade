/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14541: scsi mid ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_scsi_mid_ok_u_14541(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       SCSI mid probe for the scsi mid soft continuum.
 *   uint32_t __gj_scsi_mid_ok_u_14541  (alias)
 *   __libcgj_batch14541_marker = "libcgj-batch14541"
 *
 * Exclusive continuum CREATE-ONLY (14541-14550: scsi mid soft
 * stubs — ok slots all→0; soft_ready→0; batch_id→14550). Unique surface
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

const char __libcgj_batch14541_marker[] = "libcgj-batch14541";

/* SCSI mid soft lamp: always off. */
#define B14541_SCSI  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14541_ok(void)
{
	return B14541_SCSI;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_scsi_mid_ok_u_14541 - soft continuum surface
 *
 * Always returns 0u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_scsi_mid_ok_u_14541(void)
{
	(void)NULL;
	return b14541_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_scsi_mid_ok_u_14541(void)
    __attribute__((alias("gj_scsi_mid_ok_u_14541")));
