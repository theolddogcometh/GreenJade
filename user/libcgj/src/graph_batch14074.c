/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14074: dest ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dest_ok_u_14074(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       print destination validity probe.
 *   uint32_t __gj_dest_ok_u_14074  (alias)
 *   __libcgj_batch14074_marker = "libcgj-batch14074"
 *
 * Exclusive continuum CREATE-ONLY (14071-14080: cups/print soft stubs —
 * cups_ok_u_14071, printer_ok_u_14072, job_ok_u_14073, dest_ok_u_14074,
 * ppd_ok_u_14075, ipp_ok_u_14076, queue_ok_u_14077, media_ok_u_14078,
 * print_soft_ready_u_14079, batch_id_14080). Unique surface only; no
 * multi-def. No parent wires. No __int128. No CUPS implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14074_marker[] = "libcgj-batch14074";

/* Soft dest-ok lamp: always off (not a real dest probe). */
#define B14074_DEST_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14074_dest_ok(void)
{
	return B14074_DEST_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dest_ok_u_14074 - print dest ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect destinations
 * or call cupsd/libc. No parent wires.
 */
uint32_t
gj_dest_ok_u_14074(void)
{
	(void)NULL;
	return b14074_dest_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dest_ok_u_14074(void)
    __attribute__((alias("gj_dest_ok_u_14074")));
