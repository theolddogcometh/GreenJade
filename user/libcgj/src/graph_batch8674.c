/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8674: inotify IN_MOVE event-mask id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_inotify_move_id_8674(void);
 *     - Returns 0x000000c0 (soft IN_MOVE = MOVED_FROM|MOVED_TO id).
 *   uint32_t __gj_inotify_move_id_8674  (alias)
 *   __libcgj_batch8674_marker = "libcgj-batch8674"
 *
 * Exclusive continuum CREATE-ONLY (8671-8680: inotify/fanotify id stubs —
 * create_id_8671, modify_id_8672, delete_id_8673, move_id_8674,
 * access_id_8675, fanotify_ready_u_8676, watch_ok_u_8677,
 * watch_mask_ok_u_8678, fsnotify_ready_u_8679, batch_id_8680).
 * Unique gj_inotify_move_id_8674 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8674_marker[] = "libcgj-batch8674";

/*
 * Soft IN_MOVE interest mask: IN_MOVED_FROM (0x40) | IN_MOVED_TO (0x80).
 * Catalog id only; not a live inotify_add_watch call.
 */
#define B8674_INOTIFY_MOVE_ID  0x000000c0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8674_move_id(void)
{
	return B8674_INOTIFY_MOVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_inotify_move_id_8674 - soft IN_MOVE event-mask catalog id.
 *
 * Always returns 0x000000c0 (MOVED_FROM|MOVED_TO). Pure-data layout
 * tag; does not open an inotify fd or add a watch. No parent wires.
 */
uint32_t
gj_inotify_move_id_8674(void)
{
	(void)NULL;
	return b8674_move_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_inotify_move_id_8674(void)
    __attribute__((alias("gj_inotify_move_id_8674")));
