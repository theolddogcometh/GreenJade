/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7442: umask soft default mask id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_umask_default_id_7442(void);
 *     - Return soft default umask constant (0022 / 0x12).
 *   uint32_t __gj_umask_default_id_7442  (alias)
 *   __libcgj_batch7442_marker = "libcgj-batch7442"
 *
 * Exclusive continuum CREATE-ONLY (7441-7450: umask soft mask stubs —
 * perm_mask_id, default_id, sbits_mask_id, full_mask_id, clamp_u,
 * apply_u, ok_u, blocks_u, pack_u, batch_id_7450).
 * Unique gj_umask_default_id_7442 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7442_marker[] = "libcgj-batch7442";

/* Soft default umask: strip group/other write (0022). */
#define B7442_DEFAULT_UMASK ((uint32_t)0x12u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7442_default_id(void)
{
	return B7442_DEFAULT_UMASK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_umask_default_id_7442 - soft default umask constant.
 *
 * Always returns 0x12 (0022). Catalog id only; does not call umask.
 * No parent wires.
 */
uint32_t
gj_umask_default_id_7442(void)
{
	(void)NULL;
	return b7442_default_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_umask_default_id_7442(void)
    __attribute__((alias("gj_umask_default_id_7442")));
