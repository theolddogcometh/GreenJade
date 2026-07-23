/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10690: print soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_10690(void);
 *     - Returns the compile-time graph batch number for this TU (10690).
 *   uint32_t __gj_batch_id_10690  (alias)
 *   __libcgj_batch10690_marker = "libcgj-batch10690"
 *
 * Exclusive continuum CREATE-ONLY (10681-10690: print soft id
 * stubs — cups_ok_u_10681, ipp_ok_u_10682, print_job_ok_u_10683,
 * printer_discover_ok_u_10684, ppd_ok_u_10685, filter_ok_u_10686,
 * spool_ok_u_10687, usb_print_ok_u_10688, print_soft_ready_u_10689,
 * batch_id_10690). Unique surfaces only; no multi-def. Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols — avoid multi-def. Closes the 10681-10690 print soft wave.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10690_marker[] = "libcgj-batch10690";

#define B10690_BATCH_ID  10690u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10690_id(void)
{
	return B10690_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_10690 - report this TU's graph batch number.
 *
 * Always returns 10690. Does not call libc. No parent wires.
 */
uint32_t
gj_batch_id_10690(void)
{
	(void)NULL;
	return b10690_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_10690(void)
    __attribute__((alias("gj_batch_id_10690")));
