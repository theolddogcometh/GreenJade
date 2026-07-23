/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7341: posix_fadvise NORMAL advice id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fadv_normal_id_7341(void);
 *     - Return soft POSIX_FADV_NORMAL constant (0).
 *   uint32_t __gj_fadv_normal_id_7341  (alias)
 *   __libcgj_batch7341_marker = "libcgj-batch7341"
 *
 * Exclusive continuum CREATE-ONLY (7341-7350: fadvise sync flags stubs —
 * fadv_normal_id, fadv_random_id, fadv_sequential_id, fadv_willneed_id,
 * fadv_dontneed_id, fadv_advice_ok, o_sync_id, ms_sync_flags_pack,
 * ms_sync_flags_ok, batch_id_7350).
 * Unique gj_fadv_normal_id_7341 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7341_marker[] = "libcgj-batch7341";

/* POSIX_FADV_NORMAL: no special treatment (0). */
#define B7341_POSIX_FADV_NORMAL ((uint32_t)0u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7341_fadv_normal_id(void)
{
	return B7341_POSIX_FADV_NORMAL;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fadv_normal_id_7341 - soft POSIX_FADV_NORMAL advice constant.
 *
 * Always returns 0 (POSIX posix_fadvise NORMAL). Catalog id only; does
 * not call posix_fadvise. No parent wires.
 */
uint32_t
gj_fadv_normal_id_7341(void)
{
	(void)NULL;
	return b7341_fadv_normal_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fadv_normal_id_7341(void)
    __attribute__((alias("gj_fadv_normal_id_7341")));
