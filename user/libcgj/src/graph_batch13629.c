/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13629: mesa soft continuum ready lamp (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_mesa_soft_ready_u_13629(void);
 *     - Always returns 0. Soft stub continuum ready lamp held off;
 *       pure-data product tag for exclusive wave 13621-13630; not a
 *       hard mesa runtime probe.
 *   uint32_t __gj_mesa_soft_ready_u_13629  (alias)
 *   __libcgj_batch13629_marker = "libcgj-batch13629"
 *
 * Exclusive continuum CREATE-ONLY (13621-13630: mesa soft stubs —
 * all soft lamps →0; batch_id→13630 —
 * mesa_ok_u_13621, vulkan_ok_u_13622, radeonsi_ok_u_13623,
 * radv_ok_u_13624, amdgpu_ok_u_13625, aco_ok_u_13626, zink_ok_u_13627,
 * lavapipe_ok_u_13628, mesa_soft_ready_u_13629, batch_id_13630).
 * Unique surface only; no multi-def. Distinct from
 * gj_mesa_soft_ready_u_13429 (all→0), gj_mesa_soft_ready_u_13229
 * (all→0), gj_mesa_soft_ready_u_13029 (all→0),
 * gj_mesa_soft_ready_u_12829 (all→0), gj_mesa_soft_ready_u_12629
 * (all→0), gj_mesa_soft_ready_u_12429 (all→0),
 * gj_mesa_soft_ready_u_12229 (all→0), gj_mesa_soft_ready_u_12029
 * (all→0), gj_mesa_soft_ready_u_11929, gj_mesa_soft_ready_u_11729,
 * gj_mesa_soft_ready_u_11529, gj_mesa_soft_ready_u_11329,
 * gj_mesa_soft_ready_u_11129, gj_mesa_soft_ready_u_10929,
 * gj_mesa_soft_ready_u_10729, gj_mesa_soft_ready_u_10529,
 * gj_mesa_soft_ready_u_10329, gj_mesa_soft_ready_u_10229,
 * gj_mesa_soft_ready_u_10029 and per-slot ok_u soft stubs
 * (13621-13628, all→0). Ok units remain 0. No parent wires. No
 * __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13629_marker[] = "libcgj-batch13629";

/* Soft continuum-ready lamp for mesa id stubs wave (held off). */
#define B13629_MESA_SOFT_READY  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13629_soft_ready(void)
{
	return B13629_MESA_SOFT_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mesa_soft_ready_u_13629 - mesa soft id continuum ready.
 *
 * Always returns 0. Soft pure-data product tag; continuum ready lamp
 * held off per mesa soft all→0 theme. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_mesa_soft_ready_u_13629(void)
{
	(void)NULL;
	return b13629_soft_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mesa_soft_ready_u_13629(void)
    __attribute__((alias("gj_mesa_soft_ready_u_13629")));
