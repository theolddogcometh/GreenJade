/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8624: soft dynlink GOT-PC reloc type id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rel_gotpc_id_8624(void);
 *     - Return soft reloc-type catalog id for GOT-PC (9 / GOTPCREL).
 *   uint32_t __gj_rel_gotpc_id_8624  (alias)
 *   __libcgj_batch8624_marker = "libcgj-batch8624"
 *
 * Exclusive continuum CREATE-ONLY (8621-8630: dynlink reloc id stubs —
 * rel_none_id, rel_64_id, rel_pc32_id, rel_gotpc_id, rel_plt_id,
 * rel_copy_id, rel_glob_dat_id, rel_jump_slot_id, rel_relative_id,
 * batch_id_8630). Unique gj_rel_gotpc_id_8624 surface only; no multi-def.
 * Soft pure-data catalog only; does not apply relocations. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8624_marker[] = "libcgj-batch8624";

/* Soft dynlink reloc catalog: GOT entry PC-relative (GOTPCREL). */
#define B8624_REL_GOTPC  ((uint32_t)9u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8624_gotpc_id(void)
{
	return B8624_REL_GOTPC;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rel_gotpc_id_8624 - soft R_*_GOTPCREL reloc type catalog id.
 *
 * Always returns 9 (GOT-PC relative). Pure data tag; does not form a
 * GOT-relative displacement or load a GOT slot. No parent wires.
 */
uint32_t
gj_rel_gotpc_id_8624(void)
{
	(void)NULL;
	return b8624_gotpc_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rel_gotpc_id_8624(void)
    __attribute__((alias("gj_rel_gotpc_id_8624")));
