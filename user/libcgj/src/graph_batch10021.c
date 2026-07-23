/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10021: Mesa soft id ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mesa_ok_u_10021(void);
 *     - Returns 0 (Mesa soft id stub not asserted ready). Soft
 *       compile-time product tag; not a live Mesa/libGL probe.
 *   uint32_t __gj_mesa_ok_u_10021  (alias)
 *   __libcgj_batch10021_marker = "libcgj-batch10021"
 *
 * Exclusive CREATE-ONLY (10021-10030: mesa/radv soft id stubs). Unique
 * gj_mesa_ok_u_10021 surface only; no multi-def. Distinct from
 * gj_mesa_radv_deck_ready_5518 and gj_mesa_amdgpu_stack_score_5519.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10021_marker[] = "libcgj-batch10021";

/* Soft lamp: Mesa ok stub (not asserted). */
#define B10021_MESA_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10021_mesa_ok(void)
{
	return B10021_MESA_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mesa_ok_u_10021 - Mesa soft id ok stub.
 *
 * Always returns 0. Soft pure-data lamp; does not dlopen Mesa, query
 * GL/Vulkan ICDs, or probe driver paths. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_mesa_ok_u_10021(void)
{
	(void)NULL;
	return b10021_mesa_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mesa_ok_u_10021(void)
    __attribute__((alias("gj_mesa_ok_u_10021")));
