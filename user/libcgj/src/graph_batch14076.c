/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14076: IPP ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_ipp_ok_u_14076(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       IPP request / response validity probe.
 *   uint32_t __gj_ipp_ok_u_14076  (alias)
 *   __libcgj_batch14076_marker = "libcgj-batch14076"
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

const char __libcgj_batch14076_marker[] = "libcgj-batch14076";

/* Soft IPP-ok lamp: always off (not a real IPP probe). */
#define B14076_IPP_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14076_ipp_ok(void)
{
	return B14076_IPP_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ipp_ok_u_14076 - IPP ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not send IPP requests or
 * call cupsd/libc. No parent wires.
 */
uint32_t
gj_ipp_ok_u_14076(void)
{
	(void)NULL;
	return b14076_ipp_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ipp_ok_u_14076(void)
    __attribute__((alias("gj_ipp_ok_u_14076")));
