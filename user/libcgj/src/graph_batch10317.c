/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10317: dxvk ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dxvk_ok_u_10317(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       DXVK runtime probe for the proton/wine soft continuum.
 *   uint32_t __gj_dxvk_ok_u_10317  (alias)
 *   __libcgj_batch10317_marker = "libcgj-batch10317"
 *
 * Exclusive continuum CREATE-ONLY (10311-10320: proton/wine soft
 * id stubs — proton_ok_u_10311, wine_ok_u_10312,
 * proton_ge_ok_u_10313, steam_runtime_ok_u_10314,
 * pressure_vessel_ok_u_10315, compat_tool_ok_u_10316,
 * dxvk_ok_u_10317, vkd3d_ok_u_10318,
 * proton_soft_ready_u_10319, batch_id_10320). Unique surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10317_marker[] = "libcgj-batch10317";

/* Soft dxvk-ok lamp: always off (not a real runtime probe). */
#define B10317_DXVK_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10317_ok(void)
{
	return B10317_DXVK_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dxvk_ok_u_10317 - dxvk ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe DXVK
 * presence or call libc. No parent wires.
 */
uint32_t
gj_dxvk_ok_u_10317(void)
{
	(void)NULL;
	return b10317_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dxvk_ok_u_10317(void)
    __attribute__((alias("gj_dxvk_ok_u_10317")));
