/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9445: CUPS IPP ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_cups_ipp_ok_u_9445(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       cups_ipp / IPP request validity probe.
 *   uint32_t __gj_cups_ipp_ok_u_9445  (alias)
 *   __libcgj_batch9445_marker = "libcgj-batch9445"
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

const char __libcgj_batch9445_marker[] = "libcgj-batch9445";

/* Soft CUPS IPP-ok lamp: always off (not a real IPP probe). */
#define B9445_CUPS_IPP_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9445_cups_ipp_ok(void)
{
	return B9445_CUPS_IPP_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cups_ipp_ok_u_9445 - CUPS IPP ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not send IPP requests or
 * call cupsd/libc. No parent wires.
 */
uint32_t
gj_cups_ipp_ok_u_9445(void)
{
	(void)NULL;
	return b9445_cups_ipp_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cups_ipp_ok_u_9445(void)
    __attribute__((alias("gj_cups_ipp_ok_u_9445")));
