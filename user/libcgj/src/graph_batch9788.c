/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9788: shim loader soft lamp stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_shim_ok_u_9788(void);
 *     - Return 0 (soft lamp: shim not claimed present for this
 *       freestanding continuum). Catalog only; not a shim binary probe.
 *   uint32_t __gj_shim_ok_u_9788  (alias)
 *   __libcgj_batch9788_marker = "libcgj-batch9788"
 *
 * Exclusive continuum CREATE-ONLY (9781-9790: secureboot soft id stubs —
 * sb_enabled_ok_u, sb_setupmode_ok_u, sb_pk_ok_u, sb_kek_ok_u, sb_db_ok_u,
 * sb_dbx_ok_u, mok_ok_u, shim_ok_u, secureboot_ready_u, batch_id_9790).
 * Unique gj_shim_ok_u_9788 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9788_marker[] = "libcgj-batch9788";

/* Soft shim loader lamp (not claimed / soft-off). */
#define B9788_SHIM_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9788_shim_ok(void)
{
	return B9788_SHIM_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shim_ok_u_9788 - soft shim presence lamp.
 *
 * Always returns 0. Soft pure-data status tag; does not probe shimx64
 * or boot chain. No parent wires.
 */
uint32_t
gj_shim_ok_u_9788(void)
{
	(void)NULL;
	return b9788_shim_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shim_ok_u_9788(void)
    __attribute__((alias("gj_shim_ok_u_9788")));
