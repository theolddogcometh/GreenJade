/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8981: soft Proton version product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_proton_version_tag_8981(void);
 *     - Returns 0x50524f54 (fourCC "PROT") as the soft Proton version
 *       product tag for the bar3-related continuum. Integer catalog
 *       constant only; does not parse Proton version strings.
 *   uint32_t __gj_proton_version_tag_8981  (alias)
 *   __libcgj_batch8981_marker = "libcgj-batch8981"
 *
 * Exclusive continuum CREATE-ONLY (8981-8990: proton/wine soft id
 * stubs — proton_version_tag_8981, wine_prefix_tag_8982,
 * wine_dll_ok_u_8983, wine_wow64_id_8984, dxvk_ready_u_8985,
 * vkd3d_ready_u_8986, proton_ready_u_8987, wine_ready_u_8988,
 * proton_soft_ready_u_8989, batch_id_8990). Unique surface only; no
 * multi-def. Distinct from gj_proton_major_from_ver (batch1864),
 * gj_proton_ge_ver_ok (batch2366), and gj_proton_ge_ver_pack_5613
 * (batch5613). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8981_marker[] = "libcgj-batch8981";

/* Soft "PROT" fourCC-style Proton version product tag. */
#define B8981_PROTON_VERSION_TAG  0x50524f54u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8981_tag(void)
{
	return B8981_PROTON_VERSION_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_proton_version_tag_8981 - soft Proton version product tag.
 *
 * Always returns 0x50524f54 ("PROT"). Soft pure-data only; does not
 * parse version strings or probe Proton installs. No parent wires.
 */
uint32_t
gj_proton_version_tag_8981(void)
{
	(void)NULL;
	return b8981_tag();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_proton_version_tag_8981(void)
    __attribute__((alias("gj_proton_version_tag_8981")));
