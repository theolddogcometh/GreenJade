/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7043: io_pgetevents max_nr soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_io_pgetevents_max_ok_u_7043(uint32_t max_nr);
 *     - Return 1 if max_nr is non-zero, else 0.
 *   uint32_t __gj_io_pgetevents_max_ok_u_7043  (alias)
 *   __libcgj_batch7043_marker = "libcgj-batch7043"
 *
 * Exclusive continuum CREATE-ONLY (7041-7050: io_pgetevents stubs —
 * nr, min_ok, max_ok, range_ok, events_req, timeout_opt, sigmask_opt,
 * pack, args_ok, batch_id_7050). Unique gj_io_pgetevents_max_ok_u_7043
 * surface only; no multi-def. Does NOT redefine io_pgetevents.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7043_marker[] = "libcgj-batch7043";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7043_io_pgetevents_max_ok(uint32_t u32MaxNr)
{
	return (u32MaxNr != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_io_pgetevents_max_ok_u_7043 - soft max_nr pre-check.
 *
 * max_nr: soft io_pgetevents nr / max events capacity
 *
 * Returns 1 when max_nr is non-zero (room for at least one event), else
 * 0. Soft pre-check; does not call io_pgetevents. No parent wires.
 */
uint32_t
gj_io_pgetevents_max_ok_u_7043(uint32_t u32MaxNr)
{
	(void)NULL;
	return b7043_io_pgetevents_max_ok(u32MaxNr);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_io_pgetevents_max_ok_u_7043(uint32_t u32MaxNr)
    __attribute__((alias("gj_io_pgetevents_max_ok_u_7043")));
