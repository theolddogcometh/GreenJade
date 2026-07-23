/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7344: posix_fadvise WILLNEED advice id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fadv_willneed_id_7344(void);
 *     - Return soft POSIX_FADV_WILLNEED constant (3).
 *   uint32_t __gj_fadv_willneed_id_7344  (alias)
 *   __libcgj_batch7344_marker = "libcgj-batch7344"
 *
 * Exclusive continuum CREATE-ONLY (7341-7350: fadvise sync flags stubs —
 * fadv_normal_id, fadv_random_id, fadv_sequential_id, fadv_willneed_id,
 * fadv_dontneed_id, fadv_advice_ok, o_sync_id, ms_sync_flags_pack,
 * ms_sync_flags_ok, batch_id_7350).
 * Unique gj_fadv_willneed_id_7344 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7344_marker[] = "libcgj-batch7344";

/* POSIX_FADV_WILLNEED: will need these pages soon (3). */
#define B7344_POSIX_FADV_WILLNEED ((uint32_t)3u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7344_fadv_willneed_id(void)
{
	return B7344_POSIX_FADV_WILLNEED;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fadv_willneed_id_7344 - soft POSIX_FADV_WILLNEED advice constant.
 *
 * Always returns 3 (POSIX posix_fadvise WILLNEED). Catalog id only; does
 * not call posix_fadvise. No parent wires.
 */
uint32_t
gj_fadv_willneed_id_7344(void)
{
	(void)NULL;
	return b7344_fadv_willneed_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fadv_willneed_id_7344(void)
    __attribute__((alias("gj_fadv_willneed_id_7344")));
