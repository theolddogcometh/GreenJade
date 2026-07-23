/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5664: steam.sh launch mode codes.
 *
 * Surface (unique symbols):
 *   uint32_t gj_steam_sh_launch_mode_5664(uint32_t mode_id);
 *     - Map steam.sh launch mode id (0..7) to a packed mode code.
 *       Unknown mode_id → 0.
 *   uint32_t gj_steam_sh_launch_mode_count_5664(void);
 *     - Number of known launch modes (always 8).
 *   uint32_t gj_batch_id_5664(void);
 *   uint32_t __gj_steam_sh_launch_mode_5664
 *   uint32_t __gj_steam_sh_launch_mode_count_5664
 *   uint32_t __gj_batch_id_5664  (aliases)
 *   __libcgj_batch5664_marker = "libcgj-batch5664"
 *
 * Exclusive continuum CREATE-ONLY (5661-5670: Steam client bootstrap,
 * steam.sh launch codes, steamui ready integers). Unique *_5664
 * surfaces only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5664_marker[] = "libcgj-batch5664";

#define B5664_BATCH_ID   5664u
#define B5664_MODE_MAX   8u

/*
 * steam.sh launch mode codes (stable product tags, not argv strings):
 *   0 default, 1 silent, 2 bigpicture, 3 tenfoot, 4 developer,
 *   5 login-only, 6 shutdown, 7 verify.
 */
static const uint16_t s_b5664_mode[B5664_MODE_MAX] = {
	0x4446u, /* 0 DF  default   */
	0x534cu, /* 1 SL  silent    */
	0x4250u, /* 2 BP  bigpicture */
	0x5446u, /* 3 TF  tenfoot   */
	0x4456u, /* 4 DV  developer */
	0x4c47u, /* 5 LG  login     */
	0x5344u, /* 6 SD  shutdown  */
	0x5652u  /* 7 VR  verify    */
};

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5664_mode(uint32_t u32Mode)
{
	if (u32Mode >= B5664_MODE_MAX) {
		return 0u;
	}
	return (uint32_t)s_b5664_mode[u32Mode];
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_steam_sh_launch_mode_5664 - map steam.sh launch mode id to code.
 *
 * mode_id: 0..7 known; else 0. Soft table only; does not exec steam.sh.
 */
uint32_t
gj_steam_sh_launch_mode_5664(uint32_t mode_id)
{
	(void)NULL;
	return b5664_mode(mode_id);
}

uint32_t
gj_steam_sh_launch_mode_count_5664(void)
{
	return B5664_MODE_MAX;
}

uint32_t
gj_batch_id_5664(void)
{
	return B5664_BATCH_ID;
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_steam_sh_launch_mode_5664(uint32_t mode_id)
    __attribute__((alias("gj_steam_sh_launch_mode_5664")));

uint32_t __gj_steam_sh_launch_mode_count_5664(void)
    __attribute__((alias("gj_steam_sh_launch_mode_count_5664")));

uint32_t __gj_batch_id_5664(void)
    __attribute__((alias("gj_batch_id_5664")));
