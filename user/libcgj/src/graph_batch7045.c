/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7045: io_pgetevents events-buffer req stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_io_pgetevents_events_req_u_7045(uint32_t max_nr,
 *                                               uint32_t events_present);
 *     - Return 1 if max_nr==0 or events_present!=0, else 0.
 *   uint32_t __gj_io_pgetevents_events_req_u_7045  (alias)
 *   __libcgj_batch7045_marker = "libcgj-batch7045"
 *
 * Exclusive continuum CREATE-ONLY (7041-7050: io_pgetevents stubs —
 * nr, min_ok, max_ok, range_ok, events_req, timeout_opt, sigmask_opt,
 * pack, args_ok, batch_id_7050). Unique
 * gj_io_pgetevents_events_req_u_7045 surface only; no multi-def. Does
 * NOT redefine io_pgetevents. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7045_marker[] = "libcgj-batch7045";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7045_io_pgetevents_events_req(uint32_t u32MaxNr, uint32_t u32EventsPresent)
{
	if (u32MaxNr == 0u)
		return 1u;
	return (u32EventsPresent != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_io_pgetevents_events_req_u_7045 - soft events buffer presence check.
 *
 * max_nr:          soft max events capacity
 * events_present:  non-zero if events pointer is non-NULL (soft flag)
 *
 * Returns 1 when no events are requested (max_nr==0) or the events
 * buffer is present. Soft pointer-presence flag only; no parent wires.
 */
uint32_t
gj_io_pgetevents_events_req_u_7045(uint32_t u32MaxNr, uint32_t u32EventsPresent)
{
	(void)NULL;
	return b7045_io_pgetevents_events_req(u32MaxNr, u32EventsPresent);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_io_pgetevents_events_req_u_7045(uint32_t u32MaxNr,
                                              uint32_t u32EventsPresent)
    __attribute__((alias("gj_io_pgetevents_events_req_u_7045")));
