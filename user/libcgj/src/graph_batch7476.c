/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7476: mkdirat S_IRWXO presence stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mkdirat_has_irwxo_u_7476(uint32_t mode);
 *     - Return 1 if mode has full other rwx (S_IRWXO / 0007), else 0.
 *   uint32_t __gj_mkdirat_has_irwxo_u_7476  (alias)
 *   __libcgj_batch7476_marker = "libcgj-batch7476"
 *
 * Exclusive continuum CREATE-ONLY (7471-7480: mkdirat mode stubs —
 * def_mode_id, priv_mode_id, world_mode_id, allperms_id,
 * has_irwxu, has_irwxo, mode_ok, mode_pack, mode_errorish,
 * batch_id_7480).
 * Unique gj_mkdirat_has_irwxo_u_7476 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7476_marker[] = "libcgj-batch7476";

/* S_IRWXO: other read/write/execute (0007). */
#define B7476_S_IRWXO ((uint32_t)0007u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7476_mkdirat_has_irwxo(uint32_t u32Mode)
{
	return ((u32Mode & B7476_S_IRWXO) == B7476_S_IRWXO) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mkdirat_has_irwxo_u_7476 - 1 if mode includes full other rwx.
 *
 * mode: mkdirat(2) mode argument (soft mode_t-like u32)
 *
 * Returns 1 when bits 0007 are all set, else 0. Soft bit test only;
 * does not call mkdirat. No parent wires.
 */
uint32_t
gj_mkdirat_has_irwxo_u_7476(uint32_t u32Mode)
{
	(void)NULL;
	return b7476_mkdirat_has_irwxo(u32Mode);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mkdirat_has_irwxo_u_7476(uint32_t u32Mode)
    __attribute__((alias("gj_mkdirat_has_irwxo_u_7476")));
