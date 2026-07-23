/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7347: open O_SYNC flag id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_o_sync_id_7347(void);
 *     - Return soft Linux O_SYNC open-flag constant (0x101000).
 *   uint32_t __gj_o_sync_id_7347  (alias)
 *   __libcgj_batch7347_marker = "libcgj-batch7347"
 *
 * Exclusive continuum CREATE-ONLY (7341-7350: fadvise sync flags stubs —
 * fadv_normal_id, fadv_random_id, fadv_sequential_id, fadv_willneed_id,
 * fadv_dontneed_id, fadv_advice_ok, o_sync_id, ms_sync_flags_pack,
 * ms_sync_flags_ok, batch_id_7350).
 * Unique gj_o_sync_id_7347 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7347_marker[] = "libcgj-batch7347";

/*
 * Soft Linux x86_64 O_SYNC (octal 04010000 = O_DSYNC|__O_SYNC).
 * Catalog id only; not an open(2) call.
 */
#define B7347_O_SYNC ((uint32_t)0x101000u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7347_o_sync_id(void)
{
	return B7347_O_SYNC;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_o_sync_id_7347 - soft O_SYNC open-flag constant.
 *
 * Always returns 0x101000 (Linux soft O_SYNC). Catalog id only; does not
 * call open. No parent wires.
 */
uint32_t
gj_o_sync_id_7347(void)
{
	(void)NULL;
	return b7347_o_sync_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_o_sync_id_7347(void)
    __attribute__((alias("gj_o_sync_id_7347")));
