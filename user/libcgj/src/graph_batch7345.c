/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7345: posix_fadvise DONTNEED advice id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fadv_dontneed_id_7345(void);
 *     - Return soft POSIX_FADV_DONTNEED constant (4).
 *   uint32_t __gj_fadv_dontneed_id_7345  (alias)
 *   __libcgj_batch7345_marker = "libcgj-batch7345"
 *
 * Exclusive continuum CREATE-ONLY (7341-7350: fadvise sync flags stubs —
 * fadv_normal_id, fadv_random_id, fadv_sequential_id, fadv_willneed_id,
 * fadv_dontneed_id, fadv_advice_ok, o_sync_id, ms_sync_flags_pack,
 * ms_sync_flags_ok, batch_id_7350).
 * Unique gj_fadv_dontneed_id_7345 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7345_marker[] = "libcgj-batch7345";

/* POSIX_FADV_DONTNEED: data will not be accessed soon (4). */
#define B7345_POSIX_FADV_DONTNEED ((uint32_t)4u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7345_fadv_dontneed_id(void)
{
	return B7345_POSIX_FADV_DONTNEED;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fadv_dontneed_id_7345 - soft POSIX_FADV_DONTNEED advice constant.
 *
 * Always returns 4 (POSIX posix_fadvise DONTNEED). Catalog id only; does
 * not call posix_fadvise. No parent wires.
 */
uint32_t
gj_fadv_dontneed_id_7345(void)
{
	(void)NULL;
	return b7345_fadv_dontneed_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fadv_dontneed_id_7345(void)
    __attribute__((alias("gj_fadv_dontneed_id_7345")));
