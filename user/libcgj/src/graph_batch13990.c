/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13990: print soft continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_13990(void);
 *     - Returns the compile-time graph batch number for this TU (13990).
 *   uint32_t __gj_batch_id_13990  (alias)
 *   __libcgj_batch13990_marker = "libcgj-batch13990"
 *
 * Exclusive continuum CREATE-ONLY (13981-13990: print soft stubs —
 * cups_ok_u_13981, printer_ok_u_13982, job_ok_u_13983, dest_ok_u_13984,
 * ppd_ok_u_13985, ipp_ok_u_13986, queue_ok_u_13987, media_ok_u_13988,
 * print_soft_ready_u_13989, batch_id_13990). Unique surfaces only; no
 * multi-def. Does NOT redefine gj_batch_id / gj_graph_milestone / prior
 * batch_id_* symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13990_marker[] = "libcgj-batch13990";

#define B13990_BATCH_ID  13990u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13990_id(void)
{
	return B13990_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_13990 - report this TU's graph batch number.
 *
 * Always returns 13990. Soft continuum lamp for the 13981-13990 print
 * soft stubs wave. No parent wires.
 */
uint32_t
gj_batch_id_13990(void)
{
	(void)NULL;
	return b13990_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_13990(void)
    __attribute__((alias("gj_batch_id_13990")));
