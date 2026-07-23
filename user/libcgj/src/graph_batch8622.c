/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8622: soft dynlink R_*_64 reloc type id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rel_64_id_8622(void);
 *     - Return soft reloc-type catalog id for absolute 64-bit (1).
 *   uint32_t __gj_rel_64_id_8622  (alias)
 *   __libcgj_batch8622_marker = "libcgj-batch8622"
 *
 * Exclusive continuum CREATE-ONLY (8621-8630: dynlink reloc id stubs —
 * rel_none_id, rel_64_id, rel_pc32_id, rel_gotpc_id, rel_plt_id,
 * rel_copy_id, rel_glob_dat_id, rel_jump_slot_id, rel_relative_id,
 * batch_id_8630). Unique gj_rel_64_id_8622 surface only; no multi-def.
 * Soft pure-data catalog only; does not apply relocations. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8622_marker[] = "libcgj-batch8622";

/* Soft dynlink reloc catalog: absolute 64-bit word. */
#define B8622_REL_64  ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8622_64_id(void)
{
	return B8622_REL_64;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rel_64_id_8622 - soft R_*_64 reloc type catalog id.
 *
 * Always returns 1 (absolute 64-bit). Pure data tag; does not write an
 * absolute address into a reloc site. No parent wires.
 */
uint32_t
gj_rel_64_id_8622(void)
{
	(void)NULL;
	return b8622_64_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rel_64_id_8622(void)
    __attribute__((alias("gj_rel_64_id_8622")));
