/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6490: exclusive friend presence state batch
 * identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_6490(void);
 *     - Returns the compile-time graph batch number for this TU (6490).
 *   uint32_t __gj_batch_id_6490  (alias)
 *   __libcgj_batch6490_marker = "libcgj-batch6490"
 *
 * Exclusive continuum CREATE-ONLY (6481-6490: friend presence state stubs
 * — fpres_uid_ok_6481, fpres_state_ok_6482, fpres_online_p_6483,
 * fpres_away_p_6484, fpres_busy_p_6485, fpres_ingame_p_6486,
 * fpres_last_seen_ok_6487, fpres_appid_ok_6488, fpres_gate_6489,
 * batch_id_6490). Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6490_marker[] = "libcgj-batch6490";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6490_id(void)
{
	return 6490u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_6490 - report this TU's graph batch number.
 *
 * Always returns 6490. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_6490(void)
{
	(void)NULL;
	return b6490_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_6490(void)
    __attribute__((alias("gj_batch_id_6490")));
