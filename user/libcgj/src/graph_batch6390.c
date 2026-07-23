/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6390: exclusive family share gate batch
 * identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_6390(void);
 *     - Returns the compile-time graph batch number for this TU (6390).
 *   uint32_t __gj_batch_id_6390  (alias)
 *   __libcgj_batch6390_marker = "libcgj-batch6390"
 *
 * Exclusive continuum CREATE-ONLY (6381-6390: family share gate stubs
 * — fshare_enabled_p_6381, fshare_member_ok_6382, fshare_lender_ok_6383,
 * fshare_borrower_ok_6384, fshare_slot_ok_6385, fshare_title_ok_6386,
 * fshare_device_ok_6387, fshare_cooldown_clear_6388, fshare_gate_6389,
 * batch_id_6390). Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6390_marker[] = "libcgj-batch6390";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6390_id(void)
{
	return 6390u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_6390 - report this TU's graph batch number.
 *
 * Always returns 6390. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_6390(void)
{
	(void)NULL;
	return b6390_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_6390(void)
    __attribute__((alias("gj_batch_id_6390")));
