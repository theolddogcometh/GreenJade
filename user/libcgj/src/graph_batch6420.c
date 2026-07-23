/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6420: OpenXR runtime ready soft stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_oxr_runtime_ready_6420(uint32_t api_ok,
 *                                      uint32_t instance_ok,
 *                                      uint32_t system_ok,
 *                                      uint32_t view_ok);
 *     - Soft runtime-ready gate: all of api_ok, instance_ok, system_ok,
 *       and view_ok must be non-zero → 1; else 0.
 *   uint32_t __gj_oxr_runtime_ready_6420  (alias)
 *   uint32_t gj_batch_id_6420(void);
 *   uint32_t __gj_batch_id_6420  (alias)
 *   __libcgj_batch6420_marker = "libcgj-batch6420"
 *
 * Exclusive continuum CREATE-ONLY (6411-6420: OpenXR runtime stubs —
 * api_version_ok_6411, instance_create_ok_6412, system_id_ok_6413,
 * session_state_pack_6414, view_config_ok_6415, swapchain_extent_ok_6416,
 * space_loc_flags_6417, action_set_prio_6418, frame_budget_ms_6419,
 * runtime_ready_6420). Unique surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6420_marker[] = "libcgj-batch6420";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6420_ready(uint32_t u32Api, uint32_t u32Inst, uint32_t u32Sys,
    uint32_t u32View)
{
	if (u32Api == 0u || u32Inst == 0u || u32Sys == 0u ||
	    u32View == 0u) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_oxr_runtime_ready_6420 - soft OpenXR runtime ready gate.
 *
 * api_ok:      non-zero when API version soft-ok
 * instance_ok: non-zero when instance create soft-ok
 * system_ok:   non-zero when system id soft-ok
 * view_ok:     non-zero when view config soft-ok
 *
 * Returns 1 when all gates pass, else 0. Pure integer. No parent wires.
 */
uint32_t
gj_oxr_runtime_ready_6420(uint32_t api_ok, uint32_t instance_ok,
    uint32_t system_ok, uint32_t view_ok)
{
	(void)NULL;
	return b6420_ready(api_ok, instance_ok, system_ok, view_ok);
}

/*
 * gj_batch_id_6420 - report this TU's graph batch number.
 *
 * Always returns 6420.
 */
uint32_t
gj_batch_id_6420(void)
{
	return 6420u;
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_oxr_runtime_ready_6420(uint32_t api_ok, uint32_t instance_ok,
    uint32_t system_ok, uint32_t view_ok)
    __attribute__((alias("gj_oxr_runtime_ready_6420")));

uint32_t __gj_batch_id_6420(void)
    __attribute__((alias("gj_batch_id_6420")));
