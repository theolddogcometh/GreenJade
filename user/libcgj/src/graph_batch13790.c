/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13790: print soft continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_13790(void);
 *     - Returns the compile-time graph batch number for this TU (13790).
 *   uint32_t __gj_batch_id_13790  (alias)
 *   __libcgj_batch13790_marker = "libcgj-batch13790"
 *
 * Exclusive continuum CREATE-ONLY (13781-13790: print soft stubs —
 * cups_ok_u_13781, printer_ok_u_13782, job_ok_u_13783, dest_ok_u_13784,
 * ppd_ok_u_13785, ipp_ok_u_13786, queue_ok_u_13787, media_ok_u_13788,
 * print_soft_ready_u_13789, batch_id_13790). Unique surfaces only; no
 * multi-def. Does NOT redefine gj_batch_id / gj_graph_milestone / prior
 * batch_id_* symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13790_marker[] = "libcgj-batch13790";

#define B13790_BATCH_ID  13790u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13790_id(void)
{
	return B13790_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_13790 - report this TU's graph batch number.
 *
 * Always returns 13790. Soft continuum lamp for the 13781-13790 print
 * soft stubs wave. No parent wires.
 */
uint32_t
gj_batch_id_13790(void)
{
	(void)NULL;
	return b13790_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_13790(void)
    __attribute__((alias("gj_batch_id_13790")));
