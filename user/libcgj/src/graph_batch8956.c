/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8956: xdg_toplevel id ready stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_xdg_toplevel_ready_u_8956(void);
 *     - Return 1 (soft ready lamp for xdg_toplevel role id path).
 *   uint32_t __gj_xdg_toplevel_ready_u_8956  (alias)
 *   __libcgj_batch8956_marker = "libcgj-batch8956"
 *
 * Exclusive continuum CREATE-ONLY (8951-8960: wayland/xdg id stubs —
 * wl_display_ready, wl_compositor_ready, wl_surface_ready, wl_seat_ready,
 * xdg_wm_base_ready, xdg_toplevel_ready, xdg_popup_ready, wl_shm_ready,
 * wayland_ready, batch_id_8960).
 * Unique gj_xdg_toplevel_ready_u_8956 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8956_marker[] = "libcgj-batch8956";

/* Soft ready lamp for xdg_toplevel id path. */
#define B8956_XDG_TOPLEVEL_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8956_xdg_toplevel_ready(void)
{
	return B8956_XDG_TOPLEVEL_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xdg_toplevel_ready_u_8956 - soft xdg_toplevel id ready lamp.
 *
 * Always returns 1. Link-time / catalog presence tag only; does not
 * get_toplevel or configure windows. No parent wires.
 */
uint32_t
gj_xdg_toplevel_ready_u_8956(void)
{
	(void)NULL;
	return b8956_xdg_toplevel_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_xdg_toplevel_ready_u_8956(void)
    __attribute__((alias("gj_xdg_toplevel_ready_u_8956")));
