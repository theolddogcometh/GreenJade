/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8677: fsnotify watch soft-ok unit lamp stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_watch_ok_u_8677(void);
 *     - Returns 1 (soft watch-descriptor path ok).
 *   uint32_t __gj_watch_ok_u_8677  (alias)
 *   __libcgj_batch8677_marker = "libcgj-batch8677"
 *
 * Exclusive continuum CREATE-ONLY (8671-8680: inotify/fanotify id stubs —
 * create_id_8671, modify_id_8672, delete_id_8673, move_id_8674,
 * access_id_8675, fanotify_ready_u_8676, watch_ok_u_8677,
 * watch_mask_ok_u_8678, fsnotify_ready_u_8679, batch_id_8680).
 * Unique gj_watch_ok_u_8677 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8677_marker[] = "libcgj-batch8677";

/* Soft watch-path ok lamp (inotify/fanotify watch surface present). */
#define B8677_WATCH_OK  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8677_watch_ok(void)
{
	return B8677_WATCH_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_watch_ok_u_8677 - soft fsnotify watch ok unit lamp.
 *
 * Always returns 1. Pure-data product tag; does not allocate watch
 * descriptors or call the kernel. No parent wires.
 */
uint32_t
gj_watch_ok_u_8677(void)
{
	(void)NULL;
	return b8677_watch_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_watch_ok_u_8677(void)
    __attribute__((alias("gj_watch_ok_u_8677")));
