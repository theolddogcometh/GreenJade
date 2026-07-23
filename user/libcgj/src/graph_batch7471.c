/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7471: mkdirat default mode id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mkdirat_def_mode_id_7471(void);
 *     - Return soft default mkdirat mode (0755 / rwxr-xr-x).
 *   uint32_t __gj_mkdirat_def_mode_id_7471  (alias)
 *   __libcgj_batch7471_marker = "libcgj-batch7471"
 *
 * Exclusive continuum CREATE-ONLY (7471-7480: mkdirat mode stubs —
 * def_mode_id, priv_mode_id, world_mode_id, allperms_id,
 * has_irwxu, has_irwxo, mode_ok, mode_pack, mode_errorish,
 * batch_id_7480).
 * Unique gj_mkdirat_def_mode_id_7471 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7471_marker[] = "libcgj-batch7471";

/* Soft default mkdirat mode: 0755 (rwxr-xr-x). */
#define B7471_MKDIRAT_DEF_MODE ((uint32_t)0755u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7471_mkdirat_def_mode_id(void)
{
	return B7471_MKDIRAT_DEF_MODE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mkdirat_def_mode_id_7471 - soft default mkdirat mode constant.
 *
 * Always returns 0755. Catalog id only; does not call mkdirat.
 * No parent wires.
 */
uint32_t
gj_mkdirat_def_mode_id_7471(void)
{
	(void)NULL;
	return b7471_mkdirat_def_mode_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mkdirat_def_mode_id_7471(void)
    __attribute__((alias("gj_mkdirat_def_mode_id_7471")));
