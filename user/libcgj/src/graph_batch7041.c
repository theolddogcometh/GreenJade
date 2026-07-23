/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7041: io_pgetevents soft syscall-nr stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_io_pgetevents_nr_7041(void);
 *     - Return soft x86_64 io_pgetevents syscall number (333).
 *   uint32_t __gj_io_pgetevents_nr_7041  (alias)
 *   __libcgj_batch7041_marker = "libcgj-batch7041"
 *
 * Exclusive continuum CREATE-ONLY (7041-7050: io_pgetevents stubs —
 * nr, min_ok, max_ok, range_ok, events_req, timeout_opt, sigmask_opt,
 * pack, args_ok, batch_id_7050). Unique gj_io_pgetevents_nr_7041
 * surface only; no multi-def. Does NOT redefine io_pgetevents /
 * __io_pgetevents. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7041_marker[] = "libcgj-batch7041";

/* Soft x86_64 NR_io_pgetevents (not a live syscall invoke). */
#define B7041_IO_PGETEVENTS_NR ((uint32_t)333u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7041_io_pgetevents_nr(void)
{
	return B7041_IO_PGETEVENTS_NR;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_io_pgetevents_nr_7041 - soft io_pgetevents syscall catalog number.
 *
 * Always returns 333 (x86_64 NR_io_pgetevents). Pure-data stub for
 * product AIO/event catalogs; does not invoke io_pgetevents. No parent
 * wires.
 */
uint32_t
gj_io_pgetevents_nr_7041(void)
{
	(void)NULL;
	return b7041_io_pgetevents_nr();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_io_pgetevents_nr_7041(void)
    __attribute__((alias("gj_io_pgetevents_nr_7041")));
