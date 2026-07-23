/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7050: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_7050(void);
 *     - Returns the compile-time graph batch number for this TU (7050).
 *   uint32_t __gj_batch_id_7050  (alias)
 *   __libcgj_batch7050_marker = "libcgj-batch7050"
 *
 * Exclusive continuum CREATE-ONLY (7041-7050: io_pgetevents stubs —
 * nr, min_ok, max_ok, range_ok, events_req, timeout_opt, sigmask_opt,
 * pack, args_ok, batch_id_7050). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* / io_pgetevents symbols —
 * avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7050_marker[] = "libcgj-batch7050";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7050_id(void)
{
	return 7050u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_7050 - report this TU's graph batch number.
 *
 * Always returns 7050. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_7050(void)
{
	(void)NULL;
	return b7050_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_7050(void)
    __attribute__((alias("gj_batch_id_7050")));
