/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5520: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_5520(void);
 *     - Returns the compile-time graph batch number for this TU (5520).
 *   uint32_t __gj_batch_id_5520  (alias)
 *   __libcgj_batch5520_marker = "libcgj-batch5520"
 *
 * Exclusive CREATE-ONLY (5511-5520: kmod_path_probe_ok_5511,
 * kmod_modules_dep_probe_5512, kmod_modules_alias_probe_5513,
 * fw_path_probe_ok_5514, fw_amdgpu_blob_count_5515,
 * fw_deck_apu_blob_list_ok_5516, amdgpu_mod_ready_5517,
 * mesa_radv_deck_ready_5518, mesa_amdgpu_stack_score_5519,
 * batch_id_5520). Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5520_marker[] = "libcgj-batch5520";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5520_id(void)
{
	return 5520u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_5520 - report this TU's graph batch number.
 *
 * Always returns 5520. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_5520(void)
{
	(void)NULL;
	return b5520_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_5520(void)
    __attribute__((alias("gj_batch_id_5520")));
