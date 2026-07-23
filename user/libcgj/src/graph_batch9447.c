/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9447: CUPS print ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_cups_print_ok_u_9447(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       print submission / cupsPrintFile success probe.
 *   uint32_t __gj_cups_print_ok_u_9447  (alias)
 *   __libcgj_batch9447_marker = "libcgj-batch9447"
 *
 * Exclusive continuum CREATE-ONLY (9441-9450: cups soft id stubs —
 * cups_printer_ok_u_9441, cups_job_ok_u_9442, cups_dest_ok_u_9443,
 * cups_ppd_ok_u_9444, cups_ipp_ok_u_9445, cups_queue_ok_u_9446,
 * cups_print_ok_u_9447, cups_cancel_ok_u_9448, cups_ready_u_9449,
 * batch_id_9450). Unique surface only; no multi-def. No parent wires.
 * No __int128. No CUPS implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9447_marker[] = "libcgj-batch9447";

/* Soft CUPS print-ok lamp: always off (not a real print probe). */
#define B9447_CUPS_PRINT_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9447_cups_print_ok(void)
{
	return B9447_CUPS_PRINT_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cups_print_ok_u_9447 - CUPS print ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not submit print jobs or
 * call cupsd/libc. No parent wires.
 */
uint32_t
gj_cups_print_ok_u_9447(void)
{
	(void)NULL;
	return b9447_cups_print_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cups_print_ok_u_9447(void)
    __attribute__((alias("gj_cups_print_ok_u_9447")));
