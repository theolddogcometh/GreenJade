/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6789: inotify IN_ALL_EVENTS soft baseline.
 *
 * Surface (unique symbols):
 *   uint32_t gj_inotify_all_events_6789(void);
 *     - Returns soft IN_ALL_EVENTS baseline mask (0x00000FFF):
 *       ACCESS|MODIFY|ATTRIB|CLOSE_WRITE|CLOSE_NOWRITE|OPEN|
 *       MOVED_FROM|MOVED_TO|CREATE|DELETE|DELETE_SELF|MOVE_SELF.
 *   uint32_t gj_inotify_all_events_ok_6789(uint32_t mask);
 *     - Return 1 if mask covers the all-events baseline (has-all).
 *   uint32_t __gj_inotify_all_events_6789  (alias)
 *   uint32_t __gj_inotify_all_events_ok_6789  (alias)
 *   __libcgj_batch6789_marker = "libcgj-batch6789"
 *
 * Exclusive continuum CREATE-ONLY (6781-6790: inotify event mask stubs).
 * Unique gj_inotify_*_6789 surfaces only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6789_marker[] = "libcgj-batch6789";

/*
 * Soft IN_ALL_EVENTS baseline (watch interest bits only):
 *   bit 0  ACCESS, 1 MODIFY, 2 ATTRIB, 3 CLOSE_WRITE,
 *   4 CLOSE_NOWRITE, 5 OPEN, 6 MOVED_FROM, 7 MOVED_TO,
 *   8 CREATE, 9 DELETE, 10 DELETE_SELF, 11 MOVE_SELF
 * Compile-time tag only; not a kernel probe. Soft value 0x00000FFF.
 */
#define B6789_ALL_EVENTS  0x00000FFFu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6789_all_events(void)
{
	return B6789_ALL_EVENTS;
}

static uint32_t
b6789_all_events_ok(uint32_t u32Mask)
{
	uint32_t u32Need = B6789_ALL_EVENTS;

	return ((u32Mask & u32Need) == u32Need) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_inotify_all_events_6789 - soft IN_ALL_EVENTS baseline mask.
 *
 * Always returns 0x00000FFF. Soft pure-data product tag; does not call
 * inotify_init / inotify_add_watch. No parent wires.
 */
uint32_t
gj_inotify_all_events_6789(void)
{
	(void)NULL;
	return b6789_all_events();
}

/*
 * gj_inotify_all_events_ok_6789 - mask covers IN_ALL_EVENTS baseline bits.
 *
 * mask: soft inotify event bitmask
 *
 * Returns 1 if all baseline interest bits present; else 0.
 */
uint32_t
gj_inotify_all_events_ok_6789(uint32_t u32Mask)
{
	return b6789_all_events_ok(u32Mask);
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_inotify_all_events_6789(void)
    __attribute__((alias("gj_inotify_all_events_6789")));

uint32_t __gj_inotify_all_events_ok_6789(uint32_t u32Mask)
    __attribute__((alias("gj_inotify_all_events_ok_6789")));
