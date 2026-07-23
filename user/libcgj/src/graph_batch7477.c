/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7477: mkdirat mode validity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mkdirat_mode_ok_u_7477(uint32_t mode);
 *     - Return 1 if mode uses only ALLPERMS bits (07777), else 0.
 *   uint32_t __gj_mkdirat_mode_ok_u_7477  (alias)
 *   __libcgj_batch7477_marker = "libcgj-batch7477"
 *
 * Exclusive continuum CREATE-ONLY (7471-7480: mkdirat mode stubs —
 * def_mode_id, priv_mode_id, world_mode_id, allperms_id,
 * has_irwxu, has_irwxo, mode_ok, mode_pack, mode_errorish,
 * batch_id_7480).
 * Unique gj_mkdirat_mode_ok_u_7477 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7477_marker[] = "libcgj-batch7477";

/* Known mkdirat permission bits: ALLPERMS (07777). */
#define B7477_MKDIRAT_ALLPERMS ((uint32_t)07777u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7477_mkdirat_mode_ok(uint32_t u32Mode)
{
	return ((u32Mode & ~B7477_MKDIRAT_ALLPERMS) == 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mkdirat_mode_ok_u_7477 - 1 if mode has no bits outside ALLPERMS.
 *
 * mode: mkdirat(2) mode argument (soft mode_t-like u32)
 *
 * Returns 1 when only bits in 07777 are set (including 0), else 0.
 * Soft catalog check; does not call mkdirat. No parent wires.
 */
uint32_t
gj_mkdirat_mode_ok_u_7477(uint32_t u32Mode)
{
	(void)NULL;
	return b7477_mkdirat_mode_ok(u32Mode);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mkdirat_mode_ok_u_7477(uint32_t u32Mode)
    __attribute__((alias("gj_mkdirat_mode_ok_u_7477")));
