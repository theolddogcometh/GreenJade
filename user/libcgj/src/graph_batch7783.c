/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7783: glob GLOB_NOCHECK flag id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_glob_nocheck_id_7783(void);
 *     - Return soft GLOB_NOCHECK constant (0x10).
 *   uint32_t __gj_glob_nocheck_id_7783  (alias)
 *   __libcgj_batch7783_marker = "libcgj-batch7783"
 *
 * Exclusive continuum CREATE-ONLY (7781-7790: glob flags stubs —
 * err_id, mark_id, nocheck_id, has_err, has_mark, has_nocheck,
 * flags_ok, flags_pack, flags_errorish, batch_id_7790).
 * Unique gj_glob_nocheck_id_7783 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7783_marker[] = "libcgj-batch7783";

/* GLOB_NOCHECK: return pattern if no matches (1 << 4). */
#define B7783_GLOB_NOCHECK ((uint32_t)0x10u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7783_glob_nocheck_id(void)
{
	return B7783_GLOB_NOCHECK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_glob_nocheck_id_7783 - soft GLOB_NOCHECK flag constant.
 *
 * Always returns 0x10 (POSIX/glibc GLOB_NOCHECK). Catalog id only; does
 * not call glob(3). No parent wires.
 */
uint32_t
gj_glob_nocheck_id_7783(void)
{
	(void)NULL;
	return b7783_glob_nocheck_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_glob_nocheck_id_7783(void)
    __attribute__((alias("gj_glob_nocheck_id_7783")));
