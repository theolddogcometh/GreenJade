/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13786: IPP ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_ipp_ok_u_13786(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       IPP request / response validity probe.
 *   uint32_t __gj_ipp_ok_u_13786  (alias)
 *   __libcgj_batch13786_marker = "libcgj-batch13786"
 *
 * Exclusive continuum CREATE-ONLY (13781-13790: print soft stubs —
 * cups_ok_u_13781, printer_ok_u_13782, job_ok_u_13783, dest_ok_u_13784,
 * ppd_ok_u_13785, ipp_ok_u_13786, queue_ok_u_13787, media_ok_u_13788,
 * print_soft_ready_u_13789, batch_id_13790). Unique surface only; no
 * multi-def. No parent wires. No __int128. No CUPS implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13786_marker[] = "libcgj-batch13786";

/* Soft IPP-ok lamp: always off (not a real IPP probe). */
#define B13786_IPP_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13786_ipp_ok(void)
{
	return B13786_IPP_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ipp_ok_u_13786 - IPP ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not send IPP requests or
 * call cupsd/libc. No parent wires.
 */
uint32_t
gj_ipp_ok_u_13786(void)
{
	(void)NULL;
	return b13786_ipp_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ipp_ok_u_13786(void)
    __attribute__((alias("gj_ipp_ok_u_13786")));
