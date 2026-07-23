/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7141: shm_open O_RDWR flag id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_shm_o_rdwr_id_7141(void);
 *     - Return soft O_RDWR constant (0x2).
 *   uint32_t __gj_shm_o_rdwr_id_7141  (alias)
 *   __libcgj_batch7141_marker = "libcgj-batch7141"
 *
 * Exclusive continuum CREATE-ONLY (7141-7150: shm_open flag stubs —
 * o_rdwr_id, o_creat_id, o_excl_id, has_rdwr, has_creat, has_excl,
 * flags_ok, flags_pack, flags_errorish, batch_id_7150).
 * Unique gj_shm_o_rdwr_id_7141 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7141_marker[] = "libcgj-batch7141";

/* O_RDWR: open for reading and writing (0x2 / POSIX 2). */
#define B7141_O_RDWR ((uint32_t)0x2u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7141_shm_o_rdwr_id(void)
{
	return B7141_O_RDWR;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shm_o_rdwr_id_7141 - soft O_RDWR flag constant for shm_open.
 *
 * Always returns 0x2 (Linux/POSIX O_RDWR). Catalog id only; does not
 * call shm_open. No parent wires.
 */
uint32_t
gj_shm_o_rdwr_id_7141(void)
{
	(void)NULL;
	return b7141_shm_o_rdwr_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shm_o_rdwr_id_7141(void)
    __attribute__((alias("gj_shm_o_rdwr_id_7141")));
