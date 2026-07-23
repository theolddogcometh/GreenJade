/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8983: soft Wine DLL-ok unit flag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wine_dll_ok_u_8983(void);
 *     - Returns 1 (Wine DLL soft-id continuum ok for bar3-related
 *       product path). Pure-data product tag; does not load PE/DLL
 *       modules or probe wine paths.
 *   uint32_t __gj_wine_dll_ok_u_8983  (alias)
 *   __libcgj_batch8983_marker = "libcgj-batch8983"
 *
 * Exclusive continuum CREATE-ONLY (8981-8990: proton/wine soft id
 * stubs — proton_version_tag_8981, wine_prefix_tag_8982,
 * wine_dll_ok_u_8983, wine_wow64_id_8984, dxvk_ready_u_8985,
 * vkd3d_ready_u_8986, proton_ready_u_8987, wine_ready_u_8988,
 * proton_soft_ready_u_8989, batch_id_8990). Unique surface only; no
 * multi-def. Distinct from gj_wine_prefix_ready_5615 (batch5615).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8983_marker[] = "libcgj-batch8983";

/* Wine DLL soft-ok lamp for continuum 8981-8990. */
#define B8983_WINE_DLL_OK  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8983_dll_ok(void)
{
	return B8983_WINE_DLL_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wine_dll_ok_u_8983 - soft Wine DLL-ok unit flag.
 *
 * Always returns 1. Soft pure-data only; does not LoadLibrary or probe
 * system32 / syswow64. No parent wires.
 */
uint32_t
gj_wine_dll_ok_u_8983(void)
{
	(void)NULL;
	return b8983_dll_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wine_dll_ok_u_8983(void)
    __attribute__((alias("gj_wine_dll_ok_u_8983")));
