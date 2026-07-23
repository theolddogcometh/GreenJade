/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7151: sem_open O_CREAT flag id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sem_o_creat_id_7151(void);
 *     - Return soft O_CREAT constant (0x40 / 0100).
 *   uint32_t __gj_sem_o_creat_id_7151  (alias)
 *   __libcgj_batch7151_marker = "libcgj-batch7151"
 *
 * Exclusive continuum CREATE-ONLY (7151-7160: sem_open flag stubs —
 * creat_id, excl_id, known_mask_id, has_creat, has_excl, needs_mode,
 * flags_ok, flags_pack, flags_errorish, batch_id_7160).
 * Unique gj_sem_o_creat_id_7151 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7151_marker[] = "libcgj-batch7151";

/* O_CREAT: create named semaphore if absent (0100 / 0x40). */
#define B7151_O_CREAT ((uint32_t)0x40u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7151_sem_o_creat_id(void)
{
	return B7151_O_CREAT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sem_o_creat_id_7151 - soft O_CREAT flag constant for sem_open.
 *
 * Always returns 0x40 (Linux/POSIX open-style O_CREAT). Catalog id
 * only; does not call sem_open. No parent wires.
 */
uint32_t
gj_sem_o_creat_id_7151(void)
{
	(void)NULL;
	return b7151_sem_o_creat_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sem_o_creat_id_7151(void)
    __attribute__((alias("gj_sem_o_creat_id_7151")));
