/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8982: soft Wine prefix product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wine_prefix_tag_8982(void);
 *     - Returns 0x57494e45 (fourCC "WINE") as the soft Wine prefix
 *       product tag for the bar3-related continuum. Integer catalog
 *       constant only; does not inspect WINEPREFIX paths.
 *   uint32_t __gj_wine_prefix_tag_8982  (alias)
 *   __libcgj_batch8982_marker = "libcgj-batch8982"
 *
 * Exclusive continuum CREATE-ONLY (8981-8990: proton/wine soft id
 * stubs — proton_version_tag_8981, wine_prefix_tag_8982,
 * wine_dll_ok_u_8983, wine_wow64_id_8984, dxvk_ready_u_8985,
 * vkd3d_ready_u_8986, proton_ready_u_8987, wine_ready_u_8988,
 * proton_soft_ready_u_8989, batch_id_8990). Unique surface only; no
 * multi-def. Distinct from gj_wine_prefix_magic_5614 (batch5614) and
 * gj_wine_prefix_ok_u (batch2273). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8982_marker[] = "libcgj-batch8982";

/* Soft "WINE" fourCC-style Wine prefix product tag. */
#define B8982_WINE_PREFIX_TAG  0x57494e45u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8982_tag(void)
{
	return B8982_WINE_PREFIX_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wine_prefix_tag_8982 - soft Wine prefix product tag.
 *
 * Always returns 0x57494e45 ("WINE"). Soft pure-data only; does not
 * inspect bottle paths or call libc. No parent wires.
 */
uint32_t
gj_wine_prefix_tag_8982(void)
{
	(void)NULL;
	return b8982_tag();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wine_prefix_tag_8982(void)
    __attribute__((alias("gj_wine_prefix_tag_8982")));
