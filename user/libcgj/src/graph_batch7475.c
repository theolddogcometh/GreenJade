/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7475: mkdirat S_IRWXU presence stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mkdirat_has_irwxu_u_7475(uint32_t mode);
 *     - Return 1 if mode has full owner rwx (S_IRWXU / 0700), else 0.
 *   uint32_t __gj_mkdirat_has_irwxu_u_7475  (alias)
 *   __libcgj_batch7475_marker = "libcgj-batch7475"
 *
 * Exclusive continuum CREATE-ONLY (7471-7480: mkdirat mode stubs —
 * def_mode_id, priv_mode_id, world_mode_id, allperms_id,
 * has_irwxu, has_irwxo, mode_ok, mode_pack, mode_errorish,
 * batch_id_7480).
 * Unique gj_mkdirat_has_irwxu_u_7475 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7475_marker[] = "libcgj-batch7475";

/* S_IRWXU: owner read/write/execute (0700). */
#define B7475_S_IRWXU ((uint32_t)0700u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7475_mkdirat_has_irwxu(uint32_t u32Mode)
{
	return ((u32Mode & B7475_S_IRWXU) == B7475_S_IRWXU) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mkdirat_has_irwxu_u_7475 - 1 if mode includes full owner rwx.
 *
 * mode: mkdirat(2) mode argument (soft mode_t-like u32)
 *
 * Returns 1 when bits 0700 are all set, else 0. Soft bit test only;
 * does not call mkdirat. No parent wires.
 */
uint32_t
gj_mkdirat_has_irwxu_u_7475(uint32_t u32Mode)
{
	(void)NULL;
	return b7475_mkdirat_has_irwxu(u32Mode);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mkdirat_has_irwxu_u_7475(uint32_t u32Mode)
    __attribute__((alias("gj_mkdirat_has_irwxu_u_7475")));
