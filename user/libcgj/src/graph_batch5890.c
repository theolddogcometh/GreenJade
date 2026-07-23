/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5890: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_5890(void);
 *     - Returns the compile-time graph batch number for this TU (5890).
 *   uint32_t __gj_batch_id_5890  (alias)
 *   __libcgj_batch5890_marker = "libcgj-batch5890"
 *
 * Exclusive continuum CREATE-ONLY (5881-5890: power profile TDP steps
 * for Deck — tdp_step_min_mw_5881, tdp_step_max_mw_5882,
 * tdp_step_default_mw_5883, tdp_step_gran_mw_5884,
 * power_profile_count_5885, profile_powersave_tdp_mw_5886,
 * profile_balanced_tdp_mw_5887, profile_perf_tdp_mw_5888,
 * tdp_step_clamp_mw_5889, batch_id_5890). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* symbols — avoid
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5890_marker[] = "libcgj-batch5890";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5890_id(void)
{
	return 5890u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_5890 - report this TU's graph batch number.
 *
 * Always returns 5890. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_5890(void)
{
	(void)NULL;
	return b5890_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_5890(void)
    __attribute__((alias("gj_batch_id_5890")));
