/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5527: container / flatpak / steam layer tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_container_layer_tag_5527(uint32_t kind);
 *     - Soft classify a container layer kind into a tag nibble:
 *         0 -> 0  (unknown / none)
 *         1 -> 1  (flatpak runtime layer)
 *         2 -> 2  (steam runtime / pressure-vessel layer)
 *         3 -> 3  (generic container / bubblewrap-style layer)
 *       Other kind values return 0.
 *   uint32_t __gj_container_layer_tag_5527  (alias)
 *   __libcgj_batch5527_marker = "libcgj-batch5527"
 *
 * Exclusive continuum CREATE-ONLY (5521-5530: container/flatpak/steam
 * runtime layers, pressure-vessel stubs, scout/soldier/sniper runtime
 * IDs). Unique gj_container_layer_tag_5527 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5527_marker[] = "libcgj-batch5527";

#define B5527_KIND_NONE       0u
#define B5527_KIND_FLATPAK    1u
#define B5527_KIND_STEAM      2u
#define B5527_KIND_CONTAINER  3u

#define B5527_TAG_NONE       0u
#define B5527_TAG_FLATPAK    1u
#define B5527_TAG_STEAM      2u
#define B5527_TAG_CONTAINER  3u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5527_layer_tag(uint32_t u32Kind)
{
	if (u32Kind == B5527_KIND_NONE) {
		return B5527_TAG_NONE;
	}
	if (u32Kind == B5527_KIND_FLATPAK) {
		return B5527_TAG_FLATPAK;
	}
	if (u32Kind == B5527_KIND_STEAM) {
		return B5527_TAG_STEAM;
	}
	if (u32Kind == B5527_KIND_CONTAINER) {
		return B5527_TAG_CONTAINER;
	}
	return B5527_TAG_NONE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_container_layer_tag_5527 - soft container/flatpak/steam layer tag.
 *
 * kind: 0=none, 1=flatpak, 2=steam/pressure-vessel, 3=generic container.
 *
 * Returns matching tag 0..3, or 0 for unknown kinds. Integer-only;
 * does not inspect mounts or cgroups. No parent wires.
 */
uint32_t
gj_container_layer_tag_5527(uint32_t u32Kind)
{
	(void)NULL;
	return b5527_layer_tag(u32Kind);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_container_layer_tag_5527(uint32_t u32Kind)
    __attribute__((alias("gj_container_layer_tag_5527")));
