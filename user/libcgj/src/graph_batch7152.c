/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7152: sem_open O_EXCL flag id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sem_o_excl_id_7152(void);
 *     - Return soft O_EXCL constant (0x80 / 0200).
 *   uint32_t __gj_sem_o_excl_id_7152  (alias)
 *   __libcgj_batch7152_marker = "libcgj-batch7152"
 *
 * Exclusive continuum CREATE-ONLY (7151-7160: sem_open flag stubs —
 * creat_id, excl_id, known_mask_id, has_creat, has_excl, needs_mode,
 * flags_ok, flags_pack, flags_errorish, batch_id_7160).
 * Unique gj_sem_o_excl_id_7152 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7152_marker[] = "libcgj-batch7152";

/* O_EXCL: fail if named semaphore already exists (with O_CREAT) (0200). */
#define B7152_O_EXCL ((uint32_t)0x80u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7152_sem_o_excl_id(void)
{
	return B7152_O_EXCL;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sem_o_excl_id_7152 - soft O_EXCL flag constant for sem_open.
 *
 * Always returns 0x80 (Linux/POSIX open-style O_EXCL). Catalog id
 * only; does not call sem_open. No parent wires.
 */
uint32_t
gj_sem_o_excl_id_7152(void)
{
	(void)NULL;
	return b7152_sem_o_excl_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sem_o_excl_id_7152(void)
    __attribute__((alias("gj_sem_o_excl_id_7152")));
