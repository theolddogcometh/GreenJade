/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7454: linkat known flags soft baseline.
 *
 * Surface (unique symbols):
 *   uint32_t gj_linkat_known_flags_7454(void);
 *     - Returns soft known linkat flags mask (0x1400):
 *       AT_SYMLINK_FOLLOW | AT_EMPTY_PATH.
 *   uint32_t __gj_linkat_known_flags_7454  (alias)
 *   __libcgj_batch7454_marker = "libcgj-batch7454"
 *
 * Exclusive continuum CREATE-ONLY (7451-7460: linkat flags stubs —
 * symlink_follow, empty_path, both_mask, known_flags, known_flags_ok,
 * has_known, both_u, mask_u, any_u, continuum + batch_id_7460).
 * Unique gj_linkat_known_flags_7454 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7454_marker[] = "libcgj-batch7454";

/*
 * Soft known linkat flags baseline (compile-time tags only):
 *   AT_SYMLINK_FOLLOW (0x400)
 *   AT_EMPTY_PATH     (0x1000)
 * Soft value 0x1400. Not a kernel probe.
 */
#define B7454_LINKAT_KNOWN_FLAGS ((uint32_t)0x00001400u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7454_known_flags(void)
{
	return B7454_LINKAT_KNOWN_FLAGS;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_linkat_known_flags_7454 - soft known linkat flags mask.
 *
 * Always returns 0x1400. Soft pure-data product tag; does not call
 * linkat. No parent wires.
 */
uint32_t
gj_linkat_known_flags_7454(void)
{
	(void)NULL;
	return b7454_known_flags();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_linkat_known_flags_7454(void)
    __attribute__((alias("gj_linkat_known_flags_7454")));
