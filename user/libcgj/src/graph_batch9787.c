/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9787: Machine Owner Key soft lamp stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mok_ok_u_9787(void);
 *     - Return 0 (soft lamp: MOK not claimed present / enrolled for this
 *       freestanding continuum). Catalog only; not a MokList probe.
 *   uint32_t __gj_mok_ok_u_9787  (alias)
 *   __libcgj_batch9787_marker = "libcgj-batch9787"
 *
 * Exclusive continuum CREATE-ONLY (9781-9790: secureboot soft id stubs —
 * sb_enabled_ok_u, sb_setupmode_ok_u, sb_pk_ok_u, sb_kek_ok_u, sb_db_ok_u,
 * sb_dbx_ok_u, mok_ok_u, shim_ok_u, secureboot_ready_u, batch_id_9790).
 * Unique gj_mok_ok_u_9787 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9787_marker[] = "libcgj-batch9787";

/* Soft Machine Owner Key (MOK) lamp (not claimed / soft-off). */
#define B9787_MOK_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9787_mok_ok(void)
{
	return B9787_MOK_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mok_ok_u_9787 - soft MOK presence lamp.
 *
 * Always returns 0. Soft pure-data status tag; does not read MokList or
 * call mokutil. No parent wires.
 */
uint32_t
gj_mok_ok_u_9787(void)
{
	(void)NULL;
	return b9787_mok_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mok_ok_u_9787(void)
    __attribute__((alias("gj_mok_ok_u_9787")));
