/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14080: print soft continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14080(void);
 *     - Returns the compile-time graph batch number for this TU (14080).
 *   uint32_t __gj_batch_id_14080  (alias)
 *   __libcgj_batch14080_marker = "libcgj-batch14080"
 *
 * Exclusive continuum CREATE-ONLY (14071-14080: cups/print soft stubs —
 * cups_ok_u_14071, printer_ok_u_14072, job_ok_u_14073, dest_ok_u_14074,
 * ppd_ok_u_14075, ipp_ok_u_14076, queue_ok_u_14077, media_ok_u_14078,
 * print_soft_ready_u_14079, batch_id_14080). Unique surfaces only; no
 * multi-def. Does NOT redefine gj_batch_id / gj_graph_milestone / prior
 * batch_id_* symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14080_marker[] = "libcgj-batch14080";

#define B14080_BATCH_ID  14080u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14080_id(void)
{
	return B14080_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14080 - report this TU's graph batch number.
 *
 * Always returns 14080. Soft continuum lamp for the 14071-14080
 * cups/print soft stubs wave. No parent wires.
 */
uint32_t
gj_batch_id_14080(void)
{
	(void)NULL;
	return b14080_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_14080(void)
    __attribute__((alias("gj_batch_id_14080")));
