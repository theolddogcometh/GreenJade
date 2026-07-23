/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7781: glob GLOB_ERR flag id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_glob_err_id_7781(void);
 *     - Return soft GLOB_ERR constant (0x1).
 *   uint32_t __gj_glob_err_id_7781  (alias)
 *   __libcgj_batch7781_marker = "libcgj-batch7781"
 *
 * Exclusive continuum CREATE-ONLY (7781-7790: glob flags stubs —
 * err_id, mark_id, nocheck_id, has_err, has_mark, has_nocheck,
 * flags_ok, flags_pack, flags_errorish, batch_id_7790).
 * Unique gj_glob_err_id_7781 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7781_marker[] = "libcgj-batch7781";

/* GLOB_ERR: return on read errors (1 << 0). */
#define B7781_GLOB_ERR ((uint32_t)0x1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7781_glob_err_id(void)
{
	return B7781_GLOB_ERR;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_glob_err_id_7781 - soft GLOB_ERR flag constant.
 *
 * Always returns 0x1 (POSIX/glibc GLOB_ERR). Catalog id only; does not
 * call glob(3). No parent wires.
 */
uint32_t
gj_glob_err_id_7781(void)
{
	(void)NULL;
	return b7781_glob_err_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_glob_err_id_7781(void)
    __attribute__((alias("gj_glob_err_id_7781")));
