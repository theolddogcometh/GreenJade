/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10014: soft Wine prefix ok unit (open).
 *
 * Surface (unique symbols):
 *   uint32_t gj_wine_prefix_ok_u_10014(void);
 *     - Returns 0 (Wine prefix soft ok not complete for this
 *       bar3-related continuum; still open). Soft pure-data readiness
 *       lamp; does not inspect WINEPREFIX paths or bottles.
 *   uint32_t __gj_wine_prefix_ok_u_10014  (alias)
 *   __libcgj_batch10014_marker = "libcgj-batch10014"
 *
 * Exclusive continuum CREATE-ONLY (10011-10020: proton/dxvk soft id
 * stubs — proton_ge_ok_u_10011, dxvk_ok_u_10012, vkd3d_ok_u_10013,
 * wine_prefix_ok_u_10014, steamrt_ok_u_10015, gamescope_ok_u_10016,
 * mangohud_ok_u_10017, proton_launch_ok_u_10018,
 * proton_soft_ready_u_10019, batch_id_10020). Unique surface only; no
 * multi-def. Distinct from gj_wine_prefix_ok_u (batch2273),
 * gj_wine_prefix_tag_8982, and gj_wine_prefix_magic_5614. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10014_marker[] = "libcgj-batch10014";

/* Wine prefix soft-ok lamp for continuum 10011-10020 (still open). */
#define B10014_WINE_PREFIX_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10014_ok(void)
{
	return B10014_WINE_PREFIX_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wine_prefix_ok_u_10014 - report Wine prefix soft-ok lamp (open).
 *
 * Always returns 0 (not complete / still open). Soft pure-data only;
 * does not inspect bottle paths or call libc. No parent wires.
 */
uint32_t
gj_wine_prefix_ok_u_10014(void)
{
	(void)NULL;
	return b10014_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wine_prefix_ok_u_10014(void)
    __attribute__((alias("gj_wine_prefix_ok_u_10014")));
