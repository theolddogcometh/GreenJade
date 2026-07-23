/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13390: cups/print soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_13390(void);
 *     - Returns the compile-time graph batch number for this TU (13390).
 *   uint32_t __gj_batch_id_13390  (alias)
 *   __libcgj_batch13390_marker = "libcgj-batch13390"
 *
 * Exclusive continuum CREATE-ONLY (13381-13390: cups/print soft id
 * stubs — cups_ok_u_13381, ipp_ok_u_13382, print_job_ok_u_13383,
 * printer_discover_ok_u_13384, ppd_ok_u_13385, filter_ok_u_13386,
 * spool_ok_u_13387, usb_print_ok_u_13388, print_soft_ready_u_13389,
 * batch_id_13390).
 * Unique surfaces only; no multi-def. Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* symbols
 * — avoid multi-def. Distinct from gj_batch_id_13190 (batch13190),
 * gj_batch_id_12990 (batch12990), gj_batch_id_12790 (batch12790),
 * gj_batch_id_12590 (batch12590) and gj_batch_id_12390 (batch12390).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13390_marker[] = "libcgj-batch13390";

#define B13390_BATCH_ID  13390u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13390_id(void)
{
	return B13390_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_13390 - report this TU's graph batch number.
 *
 * Always returns 13390.
 */
uint32_t
gj_batch_id_13390(void)
{
	(void)NULL;
	return b13390_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_13390(void)
    __attribute__((alias("gj_batch_id_13390")));
