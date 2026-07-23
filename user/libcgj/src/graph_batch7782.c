/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7782: glob GLOB_MARK flag id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_glob_mark_id_7782(void);
 *     - Return soft GLOB_MARK constant (0x2).
 *   uint32_t __gj_glob_mark_id_7782  (alias)
 *   __libcgj_batch7782_marker = "libcgj-batch7782"
 *
 * Exclusive continuum CREATE-ONLY (7781-7790: glob flags stubs —
 * err_id, mark_id, nocheck_id, has_err, has_mark, has_nocheck,
 * flags_ok, flags_pack, flags_errorish, batch_id_7790).
 * Unique gj_glob_mark_id_7782 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7782_marker[] = "libcgj-batch7782";

/* GLOB_MARK: append slash to directories (1 << 1). */
#define B7782_GLOB_MARK ((uint32_t)0x2u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7782_glob_mark_id(void)
{
	return B7782_GLOB_MARK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_glob_mark_id_7782 - soft GLOB_MARK flag constant.
 *
 * Always returns 0x2 (POSIX/glibc GLOB_MARK). Catalog id only; does not
 * call glob(3). No parent wires.
 */
uint32_t
gj_glob_mark_id_7782(void)
{
	(void)NULL;
	return b7782_glob_mark_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_glob_mark_id_7782(void)
    __attribute__((alias("gj_glob_mark_id_7782")));
