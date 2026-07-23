/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13984: dest ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dest_ok_u_13984(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       print destination validity probe.
 *   uint32_t __gj_dest_ok_u_13984  (alias)
 *   __libcgj_batch13984_marker = "libcgj-batch13984"
 *
 * Exclusive continuum CREATE-ONLY (13981-13990: print soft stubs —
 * cups_ok_u_13981, printer_ok_u_13982, job_ok_u_13983, dest_ok_u_13984,
 * ppd_ok_u_13985, ipp_ok_u_13986, queue_ok_u_13987, media_ok_u_13988,
 * print_soft_ready_u_13989, batch_id_13990). Unique surface only; no
 * multi-def. No parent wires. No __int128. No CUPS implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13984_marker[] = "libcgj-batch13984";

/* Soft dest-ok lamp: always off (not a real dest probe). */
#define B13984_DEST_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13984_dest_ok(void)
{
	return B13984_DEST_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dest_ok_u_13984 - print dest ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect destinations
 * or call cupsd/libc. No parent wires.
 */
uint32_t
gj_dest_ok_u_13984(void)
{
	(void)NULL;
	return b13984_dest_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dest_ok_u_13984(void)
    __attribute__((alias("gj_dest_ok_u_13984")));
