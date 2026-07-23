/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7042: io_pgetevents min_nr soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_io_pgetevents_min_ok_u_7042(uint32_t min_nr);
 *     - Return 1 if min_nr is soft-acceptable (any value; always 1).
 *   uint32_t __gj_io_pgetevents_min_ok_u_7042  (alias)
 *   __libcgj_batch7042_marker = "libcgj-batch7042"
 *
 * Exclusive continuum CREATE-ONLY (7041-7050: io_pgetevents stubs —
 * nr, min_ok, max_ok, range_ok, events_req, timeout_opt, sigmask_opt,
 * pack, args_ok, batch_id_7050). Unique gj_io_pgetevents_min_ok_u_7042
 * surface only; no multi-def. Does NOT redefine io_pgetevents.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7042_marker[] = "libcgj-batch7042";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7042_io_pgetevents_min_ok(uint32_t u32MinNr)
{
	/* min_nr may be zero (wait for any available); soft always-ok. */
	(void)u32MinNr;
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_io_pgetevents_min_ok_u_7042 - soft min_nr pre-check.
 *
 * min_nr: soft io_pgetevents min_nr (events to wait for at least)
 *
 * Always returns 1. Soft acceptance for zero-or-more min_nr; does not
 * call io_pgetevents. No parent wires.
 */
uint32_t
gj_io_pgetevents_min_ok_u_7042(uint32_t u32MinNr)
{
	(void)NULL;
	return b7042_io_pgetevents_min_ok(u32MinNr);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_io_pgetevents_min_ok_u_7042(uint32_t u32MinNr)
    __attribute__((alias("gj_io_pgetevents_min_ok_u_7042")));
