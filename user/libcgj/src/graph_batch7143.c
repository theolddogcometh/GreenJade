/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7143: shm_open O_EXCL flag id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_shm_o_excl_id_7143(void);
 *     - Return soft O_EXCL constant (0x80 / octal 0200).
 *   uint32_t __gj_shm_o_excl_id_7143  (alias)
 *   __libcgj_batch7143_marker = "libcgj-batch7143"
 *
 * Exclusive continuum CREATE-ONLY (7141-7150: shm_open flag stubs —
 * o_rdwr_id, o_creat_id, o_excl_id, has_rdwr, has_creat, has_excl,
 * flags_ok, flags_pack, flags_errorish, batch_id_7150).
 * Unique gj_shm_o_excl_id_7143 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7143_marker[] = "libcgj-batch7143";

/* O_EXCL: fail if object already exists with O_CREAT (0x80 / octal 0200). */
#define B7143_O_EXCL ((uint32_t)0x80u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7143_shm_o_excl_id(void)
{
	return B7143_O_EXCL;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shm_o_excl_id_7143 - soft O_EXCL flag constant for shm_open.
 *
 * Always returns 0x80 (Linux/POSIX O_EXCL). Catalog id only; does not
 * call shm_open. No parent wires.
 */
uint32_t
gj_shm_o_excl_id_7143(void)
{
	(void)NULL;
	return b7143_shm_o_excl_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shm_o_excl_id_7143(void)
    __attribute__((alias("gj_shm_o_excl_id_7143")));
