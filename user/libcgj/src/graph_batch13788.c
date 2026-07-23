/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13788: media ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_media_ok_u_13788(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       media / paper-size validity probe.
 *   uint32_t __gj_media_ok_u_13788  (alias)
 *   __libcgj_batch13788_marker = "libcgj-batch13788"
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

const char __libcgj_batch13788_marker[] = "libcgj-batch13788";

/* Soft media-ok lamp: always off (not a real media probe). */
#define B13788_MEDIA_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13788_media_ok(void)
{
	return B13788_MEDIA_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_media_ok_u_13788 - media ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect media sizes
 * or call cupsd/libc. No parent wires.
 */
uint32_t
gj_media_ok_u_13788(void)
{
	(void)NULL;
	return b13788_media_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_media_ok_u_13788(void)
    __attribute__((alias("gj_media_ok_u_13788")));
