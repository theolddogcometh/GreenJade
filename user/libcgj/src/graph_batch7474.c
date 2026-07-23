/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7474: mkdirat ALLPERMS mask id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mkdirat_allperms_id_7474(void);
 *     - Return soft ALLPERMS mask (07777) for mkdirat mode_t bits.
 *   uint32_t __gj_mkdirat_allperms_id_7474  (alias)
 *   __libcgj_batch7474_marker = "libcgj-batch7474"
 *
 * Exclusive continuum CREATE-ONLY (7471-7480: mkdirat mode stubs —
 * def_mode_id, priv_mode_id, world_mode_id, allperms_id,
 * has_irwxu, has_irwxo, mode_ok, mode_pack, mode_errorish,
 * batch_id_7480).
 * Unique gj_mkdirat_allperms_id_7474 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7474_marker[] = "libcgj-batch7474";

/*
 * Soft ALLPERMS: permission + setuid/setgid/sticky bits (07777).
 * Matches POSIX ALLPERMS without sys/stat.h.
 */
#define B7474_MKDIRAT_ALLPERMS ((uint32_t)07777u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7474_mkdirat_allperms_id(void)
{
	return B7474_MKDIRAT_ALLPERMS;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mkdirat_allperms_id_7474 - soft ALLPERMS mode mask constant.
 *
 * Always returns 07777. Catalog id only; does not call mkdirat.
 * No parent wires.
 */
uint32_t
gj_mkdirat_allperms_id_7474(void)
{
	(void)NULL;
	return b7474_mkdirat_allperms_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mkdirat_allperms_id_7474(void)
    __attribute__((alias("gj_mkdirat_allperms_id_7474")));
