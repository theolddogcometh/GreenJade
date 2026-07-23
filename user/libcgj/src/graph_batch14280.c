/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14280: print soft continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14280(void);
 *     - Returns the compile-time graph batch number for this TU (14280).
 *   uint32_t __gj_batch_id_14280  (alias)
 *   __libcgj_batch14280_marker = "libcgj-batch14280"
 *
 * Exclusive continuum CREATE-ONLY (14271-14280: cups/print soft stubs —
 * cups_ok_u_14271, printer_ok_u_14272, job_ok_u_14273, dest_ok_u_14274,
 * ppd_ok_u_14275, ipp_ok_u_14276, queue_ok_u_14277, media_ok_u_14278,
 * print_soft_ready_u_14279, batch_id_14280). Unique surfaces only; no
 * multi-def. Does NOT redefine gj_batch_id / gj_graph_milestone / prior
 * batch_id_* symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14280_marker[] = "libcgj-batch14280";

#define B14280_BATCH_ID  14280u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14280_id(void)
{
	return B14280_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14280 - report this TU's graph batch number.
 *
 * Always returns 14280. Soft continuum lamp for the 14271-14280
 * cups/print soft stubs wave. No parent wires.
 */
uint32_t
gj_batch_id_14280(void)
{
	(void)NULL;
	return b14280_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_14280(void)
    __attribute__((alias("gj_batch_id_14280")));
