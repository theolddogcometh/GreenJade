/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9450: cups soft continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_9450(void);
 *     - Returns the compile-time graph batch number for this TU (9450).
 *   uint32_t __gj_batch_id_9450  (alias)
 *   __libcgj_batch9450_marker = "libcgj-batch9450"
 *
 * Exclusive continuum CREATE-ONLY (9441-9450: cups soft id stubs —
 * cups_printer_ok_u_9441, cups_job_ok_u_9442, cups_dest_ok_u_9443,
 * cups_ppd_ok_u_9444, cups_ipp_ok_u_9445, cups_queue_ok_u_9446,
 * cups_print_ok_u_9447, cups_cancel_ok_u_9448, cups_ready_u_9449,
 * batch_id_9450). Unique surfaces only; no multi-def. Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* symbols — avoid
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9450_marker[] = "libcgj-batch9450";

#define B9450_BATCH_ID  9450u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9450_id(void)
{
	return B9450_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_9450 - report this TU's graph batch number.
 *
 * Always returns 9450.
 */
uint32_t
gj_batch_id_9450(void)
{
	(void)NULL;
	return b9450_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_9450(void)
    __attribute__((alias("gj_batch_id_9450")));
