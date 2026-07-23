/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9443: CUPS dest ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_cups_dest_ok_u_9443(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       cups_dest / destination validity probe.
 *   uint32_t __gj_cups_dest_ok_u_9443  (alias)
 *   __libcgj_batch9443_marker = "libcgj-batch9443"
 *
 * Exclusive continuum CREATE-ONLY (9441-9450: cups soft id stubs —
 * cups_printer_ok_u_9441, cups_job_ok_u_9442, cups_dest_ok_u_9443,
 * cups_ppd_ok_u_9444, cups_ipp_ok_u_9445, cups_queue_ok_u_9446,
 * cups_print_ok_u_9447, cups_cancel_ok_u_9448, cups_ready_u_9449,
 * batch_id_9450). Unique surface only; no multi-def. No parent wires.
 * No __int128. No CUPS implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9443_marker[] = "libcgj-batch9443";

/* Soft CUPS dest-ok lamp: always off (not a real dest probe). */
#define B9443_CUPS_DEST_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9443_cups_dest_ok(void)
{
	return B9443_CUPS_DEST_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cups_dest_ok_u_9443 - CUPS dest ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect destinations
 * or call cupsd/libc. No parent wires.
 */
uint32_t
gj_cups_dest_ok_u_9443(void)
{
	(void)NULL;
	return b9443_cups_dest_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cups_dest_ok_u_9443(void)
    __attribute__((alias("gj_cups_dest_ok_u_9443")));
