/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8627: soft dynlink R_*_GLOB_DAT reloc type id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rel_glob_dat_id_8627(void);
 *     - Return soft reloc-type catalog id for GLOB_DAT (6).
 *   uint32_t __gj_rel_glob_dat_id_8627  (alias)
 *   __libcgj_batch8627_marker = "libcgj-batch8627"
 *
 * Exclusive continuum CREATE-ONLY (8621-8630: dynlink reloc id stubs —
 * rel_none_id, rel_64_id, rel_pc32_id, rel_gotpc_id, rel_plt_id,
 * rel_copy_id, rel_glob_dat_id, rel_jump_slot_id, rel_relative_id,
 * batch_id_8630). Unique gj_rel_glob_dat_id_8627 surface only; no multi-def.
 * Soft pure-data catalog only; does not apply relocations. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8627_marker[] = "libcgj-batch8627";

/* Soft dynlink reloc catalog: GLOB_DAT (set GOT entry to symbol). */
#define B8627_REL_GLOB_DAT  ((uint32_t)6u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8627_glob_dat_id(void)
{
	return B8627_REL_GLOB_DAT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rel_glob_dat_id_8627 - soft R_*_GLOB_DAT reloc type catalog id.
 *
 * Always returns 6 (GLOB_DAT). Pure data tag; does not write a resolved
 * symbol address into a GOT slot. No parent wires.
 */
uint32_t
gj_rel_glob_dat_id_8627(void)
{
	(void)NULL;
	return b8627_glob_dat_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rel_glob_dat_id_8627(void)
    __attribute__((alias("gj_rel_glob_dat_id_8627")));
