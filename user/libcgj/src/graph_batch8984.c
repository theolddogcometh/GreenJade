/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8984: soft Wine WoW64 architecture id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wine_wow64_id_8984(void);
 *     - Returns 0x574f3634 (fourCC "WO64") as the soft Wine WoW64
 *       architecture catalog id for the bar3-related continuum.
 *       Integer product tag only; does not probe PE machine types.
 *   uint32_t __gj_wine_wow64_id_8984  (alias)
 *   __libcgj_batch8984_marker = "libcgj-batch8984"
 *
 * Exclusive continuum CREATE-ONLY (8981-8990: proton/wine soft id
 * stubs — proton_version_tag_8981, wine_prefix_tag_8982,
 * wine_dll_ok_u_8983, wine_wow64_id_8984, dxvk_ready_u_8985,
 * vkd3d_ready_u_8986, proton_ready_u_8987, wine_ready_u_8988,
 * proton_soft_ready_u_8989, batch_id_8990). Unique surface only; no
 * multi-def. Distinct from gj_wine_dll_ok_u_8983 and
 * gj_wine_prefix_tag_8982. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8984_marker[] = "libcgj-batch8984";

/* Soft "WO64" fourCC-style Wine WoW64 architecture id. */
#define B8984_WINE_WOW64_ID  0x574f3634u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8984_wow64_id(void)
{
	return B8984_WINE_WOW64_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wine_wow64_id_8984 - soft Wine WoW64 architecture catalog id.
 *
 * Always returns 0x574f3634 ("WO64"). Soft pure-data only; does not
 * inspect PE headers or call libc. No parent wires.
 */
uint32_t
gj_wine_wow64_id_8984(void)
{
	(void)NULL;
	return b8984_wow64_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wine_wow64_id_8984(void)
    __attribute__((alias("gj_wine_wow64_id_8984")));
