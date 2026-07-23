/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5557: Steam Input action-set handle stubs.
 *
 * Surface (unique symbols):
 *   uint64_t gj_steam_action_set_handle_5557(uint32_t set_id,
 *                                            uint32_t slot);
 *     - Pure local handle fabric (not a Steamworks IPC call):
 *       (batch_tag << 48) | (slot << 32) | set_id. slot clamped 0..15.
 *   uint32_t gj_steam_action_set_id_5557(uint64_t handle);
 *   uint32_t gj_steam_action_set_slot_5557(uint64_t handle);
 *   int gj_steam_action_set_valid_5557(uint64_t handle);
 *     - 1 if high tag nibble matches this batch fabric, else 0.
 *   uint32_t gj_batch_id_5557(void);
 *   __ aliases
 *   __libcgj_batch5557_marker = "libcgj-batch5557"
 *
 * Exclusive continuum CREATE-ONLY (5551-5560: controller layouts —
 * glyphs, gyro calib, trackpad; steam input stubs). Unique *_5557
 * surfaces only; no multi-def. No parent wires. No __int128.
 * Not affiliated with Valve; clean-room integer handle packing only.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5557_marker[] = "libcgj-batch5557";

#define B5557_BATCH_ID  5557u
/* 16-bit fabric tag derived from batch id (fits high of u64). */
#define B5557_TAG       ((uint64_t)0x1557u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b5557_make(uint32_t u32Set, uint32_t u32Slot)
{
	if (u32Slot > 15u) {
		u32Slot = 15u;
	}
	return (B5557_TAG << 48)
	    | (((uint64_t)u32Slot & 0xfull) << 32)
	    | (uint64_t)u32Set;
}

static uint32_t
b5557_set_id(uint64_t u64H)
{
	return (uint32_t)(u64H & 0xffffffffull);
}

static uint32_t
b5557_slot(uint64_t u64H)
{
	return (uint32_t)((u64H >> 32) & 0xfull);
}

static int
b5557_valid(uint64_t u64H)
{
	return (((u64H >> 48) & 0xffffull) == B5557_TAG) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

uint64_t
gj_steam_action_set_handle_5557(uint32_t set_id, uint32_t slot)
{
	(void)NULL;
	return b5557_make(set_id, slot);
}

uint32_t
gj_steam_action_set_id_5557(uint64_t handle)
{
	return b5557_set_id(handle);
}

uint32_t
gj_steam_action_set_slot_5557(uint64_t handle)
{
	return b5557_slot(handle);
}

int
gj_steam_action_set_valid_5557(uint64_t handle)
{
	return b5557_valid(handle);
}

uint32_t
gj_batch_id_5557(void)
{
	return B5557_BATCH_ID;
}

/* ---- underscored aliases ----------------------------------------------- */

uint64_t __gj_steam_action_set_handle_5557(uint32_t set_id, uint32_t slot)
    __attribute__((alias("gj_steam_action_set_handle_5557")));

uint32_t __gj_steam_action_set_id_5557(uint64_t handle)
    __attribute__((alias("gj_steam_action_set_id_5557")));

uint32_t __gj_steam_action_set_slot_5557(uint64_t handle)
    __attribute__((alias("gj_steam_action_set_slot_5557")));

int __gj_steam_action_set_valid_5557(uint64_t handle)
    __attribute__((alias("gj_steam_action_set_valid_5557")));

uint32_t __gj_batch_id_5557(void)
    __attribute__((alias("gj_batch_id_5557")));
