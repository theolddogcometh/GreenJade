/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7153: sem_open known oflag mask id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sem_oflag_known_mask_id_7153(void);
 *     - Return soft known-oflag mask O_CREAT|O_EXCL (0xC0).
 *   uint32_t __gj_sem_oflag_known_mask_id_7153  (alias)
 *   __libcgj_batch7153_marker = "libcgj-batch7153"
 *
 * Exclusive continuum CREATE-ONLY (7151-7160: sem_open flag stubs —
 * creat_id, excl_id, known_mask_id, has_creat, has_excl, needs_mode,
 * flags_ok, flags_pack, flags_errorish, batch_id_7160).
 * Unique gj_sem_oflag_known_mask_id_7153 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7153_marker[] = "libcgj-batch7153";

/*
 * Known sem_open oflag bits:
 *   O_CREAT (0x40) | O_EXCL (0x80) = 0xC0
 * Zero oflag (open existing) is also valid and has no bits set.
 */
#define B7153_SEM_OFLAG_KNOWN_MASK ((uint32_t)0xC0u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7153_sem_oflag_known_mask_id(void)
{
	return B7153_SEM_OFLAG_KNOWN_MASK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sem_oflag_known_mask_id_7153 - soft known sem_open oflag mask.
 *
 * Always returns 0xC0 (O_CREAT|O_EXCL). Catalog mask only; does not
 * call sem_open. No parent wires.
 */
uint32_t
gj_sem_oflag_known_mask_id_7153(void)
{
	(void)NULL;
	return b7153_sem_oflag_known_mask_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sem_oflag_known_mask_id_7153(void)
    __attribute__((alias("gj_sem_oflag_known_mask_id_7153")));
