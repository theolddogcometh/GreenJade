/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8955: xdg_wm_base id ready stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_xdg_wm_base_ready_u_8955(void);
 *     - Return 1 (soft ready lamp for xdg_wm_base global id path).
 *   uint32_t __gj_xdg_wm_base_ready_u_8955  (alias)
 *   __libcgj_batch8955_marker = "libcgj-batch8955"
 *
 * Exclusive continuum CREATE-ONLY (8951-8960: wayland/xdg id stubs —
 * wl_display_ready, wl_compositor_ready, wl_surface_ready, wl_seat_ready,
 * xdg_wm_base_ready, xdg_toplevel_ready, xdg_popup_ready, wl_shm_ready,
 * wayland_ready, batch_id_8960).
 * Unique gj_xdg_wm_base_ready_u_8955 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8955_marker[] = "libcgj-batch8955";

/* Soft ready lamp for xdg_wm_base id path. */
#define B8955_XDG_WM_BASE_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8955_xdg_wm_base_ready(void)
{
	return B8955_XDG_WM_BASE_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xdg_wm_base_ready_u_8955 - soft xdg_wm_base id ready lamp.
 *
 * Always returns 1. Link-time / catalog presence tag only; does not
 * bind xdg_wm_base or create shell roles. No parent wires.
 */
uint32_t
gj_xdg_wm_base_ready_u_8955(void)
{
	(void)NULL;
	return b8955_xdg_wm_base_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_xdg_wm_base_ready_u_8955(void)
    __attribute__((alias("gj_xdg_wm_base_ready_u_8955")));
