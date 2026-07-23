/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7342: posix_fadvise RANDOM advice id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fadv_random_id_7342(void);
 *     - Return soft POSIX_FADV_RANDOM constant (1).
 *   uint32_t __gj_fadv_random_id_7342  (alias)
 *   __libcgj_batch7342_marker = "libcgj-batch7342"
 *
 * Exclusive continuum CREATE-ONLY (7341-7350: fadvise sync flags stubs —
 * fadv_normal_id, fadv_random_id, fadv_sequential_id, fadv_willneed_id,
 * fadv_dontneed_id, fadv_advice_ok, o_sync_id, ms_sync_flags_pack,
 * ms_sync_flags_ok, batch_id_7350).
 * Unique gj_fadv_random_id_7342 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7342_marker[] = "libcgj-batch7342";

/* POSIX_FADV_RANDOM: expect random page references (1). */
#define B7342_POSIX_FADV_RANDOM ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7342_fadv_random_id(void)
{
	return B7342_POSIX_FADV_RANDOM;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fadv_random_id_7342 - soft POSIX_FADV_RANDOM advice constant.
 *
 * Always returns 1 (POSIX posix_fadvise RANDOM). Catalog id only; does
 * not call posix_fadvise. No parent wires.
 */
uint32_t
gj_fadv_random_id_7342(void)
{
	(void)NULL;
	return b7342_fadv_random_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fadv_random_id_7342(void)
    __attribute__((alias("gj_fadv_random_id_7342")));
