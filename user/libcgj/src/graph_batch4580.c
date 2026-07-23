/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4580: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4580(void);
 *     - Returns the compile-time graph batch number for this TU (4580).
 *   uint32_t __gj_batch_id_4580  (alias)
 *   __libcgj_batch4580_marker = "libcgj-batch4580"
 *
 * Exclusive continuum CREATE-ONLY (4571-4580: ver_pack_u, ver_major_u,
 * ver_minor_u, ver_patch_u, ver_cmp_u, ver_compatible_u, ver_is_zero_u,
 * ver_bump_major_u, ver_bump_minor_u, batch_id_4580). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* symbols — avoid
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4580_marker[] = "libcgj-batch4580";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4580_id(void)
{
	return 4580u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4580 - report this TU's graph batch number.
 *
 * Always returns 4580. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_4580(void)
{
	(void)NULL;
	return b4580_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_4580(void)
    __attribute__((alias("gj_batch_id_4580")));
