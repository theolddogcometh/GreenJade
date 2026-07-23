/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7142: shm_open O_CREAT flag id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_shm_o_creat_id_7142(void);
 *     - Return soft O_CREAT constant (0x40 / octal 0100).
 *   uint32_t __gj_shm_o_creat_id_7142  (alias)
 *   __libcgj_batch7142_marker = "libcgj-batch7142"
 *
 * Exclusive continuum CREATE-ONLY (7141-7150: shm_open flag stubs —
 * o_rdwr_id, o_creat_id, o_excl_id, has_rdwr, has_creat, has_excl,
 * flags_ok, flags_pack, flags_errorish, batch_id_7150).
 * Unique gj_shm_o_creat_id_7142 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7142_marker[] = "libcgj-batch7142";

/* O_CREAT: create object if it does not exist (0x40 / octal 0100). */
#define B7142_O_CREAT ((uint32_t)0x40u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7142_shm_o_creat_id(void)
{
	return B7142_O_CREAT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shm_o_creat_id_7142 - soft O_CREAT flag constant for shm_open.
 *
 * Always returns 0x40 (Linux/POSIX O_CREAT). Catalog id only; does not
 * call shm_open. No parent wires.
 */
uint32_t
gj_shm_o_creat_id_7142(void)
{
	(void)NULL;
	return b7142_shm_o_creat_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shm_o_creat_id_7142(void)
    __attribute__((alias("gj_shm_o_creat_id_7142")));
