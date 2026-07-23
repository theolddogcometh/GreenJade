/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8672: inotify IN_MODIFY event-mask id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_inotify_modify_id_8672(void);
 *     - Returns 0x00000002 (soft IN_MODIFY event-mask catalog id).
 *   uint32_t __gj_inotify_modify_id_8672  (alias)
 *   __libcgj_batch8672_marker = "libcgj-batch8672"
 *
 * Exclusive continuum CREATE-ONLY (8671-8680: inotify/fanotify id stubs —
 * create_id_8671, modify_id_8672, delete_id_8673, move_id_8674,
 * access_id_8675, fanotify_ready_u_8676, watch_ok_u_8677,
 * watch_mask_ok_u_8678, fsnotify_ready_u_8679, batch_id_8680).
 * Unique gj_inotify_modify_id_8672 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8672_marker[] = "libcgj-batch8672";

/* Soft IN_MODIFY (file was modified). */
#define B8672_INOTIFY_MODIFY_ID  0x00000002u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8672_modify_id(void)
{
	return B8672_INOTIFY_MODIFY_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_inotify_modify_id_8672 - soft IN_MODIFY event-mask catalog id.
 *
 * Always returns 0x00000002. Pure-data layout tag; does not open an
 * inotify fd or add a watch. No parent wires.
 */
uint32_t
gj_inotify_modify_id_8672(void)
{
	(void)NULL;
	return b8672_modify_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_inotify_modify_id_8672(void)
    __attribute__((alias("gj_inotify_modify_id_8672")));
