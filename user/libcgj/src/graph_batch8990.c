/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8990: proton/wine soft id continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_8990(void);
 *     - Returns the compile-time graph batch number for this TU (8990).
 *   uint32_t __gj_batch_id_8990  (alias)
 *   __libcgj_batch8990_marker = "libcgj-batch8990"
 *
 * Exclusive continuum CREATE-ONLY (8981-8990: proton/wine soft id
 * stubs — proton_version_tag_8981, wine_prefix_tag_8982,
 * wine_dll_ok_u_8983, wine_wow64_id_8984, dxvk_ready_u_8985,
 * vkd3d_ready_u_8986, proton_ready_u_8987, wine_ready_u_8988,
 * proton_soft_ready_u_8989, batch_id_8990). Unique surfaces only; no
 * multi-def. Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8990_marker[] = "libcgj-batch8990";

#define B8990_BATCH_ID  8990u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8990_id(void)
{
	return B8990_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_8990 - report this TU's graph batch number.
 *
 * Always returns 8990.
 */
uint32_t
gj_batch_id_8990(void)
{
	(void)NULL;
	return b8990_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_8990(void)
    __attribute__((alias("gj_batch_id_8990")));
