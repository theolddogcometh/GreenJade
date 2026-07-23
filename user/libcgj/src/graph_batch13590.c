/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13590: cups/print soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_13590(void);
 *     - Returns the compile-time graph batch number for this TU (13590).
 *   uint32_t __gj_batch_id_13590  (alias)
 *   __libcgj_batch13590_marker = "libcgj-batch13590"
 *
 * Exclusive continuum CREATE-ONLY (13581-13590: cups/print soft id
 * stubs — cups_ok_u_13581, ipp_ok_u_13582, print_job_ok_u_13583,
 * printer_discover_ok_u_13584, ppd_ok_u_13585, filter_ok_u_13586,
 * spool_ok_u_13587, usb_print_ok_u_13588, print_soft_ready_u_13589,
 * batch_id_13590→13590).
 * Unique surfaces only; no multi-def. Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* symbols
 * — avoid multi-def. Distinct from gj_batch_id_13390 (batch13390),
 * gj_batch_id_13190 (batch13190), gj_batch_id_12990 (batch12990),
 * gj_batch_id_12790 (batch12790), gj_batch_id_12590 (batch12590) and
 * gj_batch_id_12390 (batch12390). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13590_marker[] = "libcgj-batch13590";

#define B13590_BATCH_ID  13590u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13590_id(void)
{
	return B13590_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_13590 - report this TU's graph batch number.
 *
 * Always returns 13590. Closes the 13581-13590 cups/print soft wave.
 */
uint32_t
gj_batch_id_13590(void)
{
	(void)NULL;
	return b13590_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_13590(void)
    __attribute__((alias("gj_batch_id_13590")));
