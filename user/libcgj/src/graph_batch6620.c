/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6620: mmap/mprotect flags ready soft gate.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mmap_flags_ready_6620(uint32_t prot_mask_ok,
 *                                     uint32_t map_type_ok);
 *     - Soft ready gate: both prot_mask_ok and map_type_ok non-zero → 1;
 *       else 0.
 *   uint32_t __gj_mmap_flags_ready_6620  (alias)
 *   uint32_t gj_batch_id_6620(void);
 *   uint32_t __gj_batch_id_6620  (alias)
 *   __libcgj_batch6620_marker = "libcgj-batch6620"
 *
 * Exclusive continuum CREATE-ONLY (6611-6620: mmap/mprotect flag stubs —
 * prot_read, prot_write, prot_exec, prot_none_ok, prot_pack, prot_mask_ok,
 * map_shared, map_private, map_type_ok, flags_ready). Unique surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6620_marker[] = "libcgj-batch6620";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6620_flags_ready(uint32_t u32ProtOk, uint32_t u32MapOk)
{
	if (u32ProtOk == 0u || u32MapOk == 0u) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mmap_flags_ready_6620 - soft mmap/mprotect flags ready gate.
 *
 * prot_mask_ok: non-zero when prot mask soft-ok
 * map_type_ok:  non-zero when map type soft-ok
 *
 * Returns 1 when both gates pass, else 0. Pure integer. No parent wires.
 */
uint32_t
gj_mmap_flags_ready_6620(uint32_t prot_mask_ok, uint32_t map_type_ok)
{
	(void)NULL;
	return b6620_flags_ready(prot_mask_ok, map_type_ok);
}

/*
 * gj_batch_id_6620 - report this TU's graph batch number.
 *
 * Always returns 6620.
 */
uint32_t
gj_batch_id_6620(void)
{
	return 6620u;
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_mmap_flags_ready_6620(uint32_t prot_mask_ok, uint32_t map_type_ok)
    __attribute__((alias("gj_mmap_flags_ready_6620")));

uint32_t __gj_batch_id_6620(void)
    __attribute__((alias("gj_batch_id_6620")));
