/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6180: MILESTONE 6180 batch identity + wave
 * fingerprint for the flatpak app id hash stubs continuum.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_6180(void);
 *     - Returns the compile-time graph batch number for this TU (6180).
 *   uint32_t gj_graph_milestone_6180(void);
 *     - Returns milestone revision 6180 (end of 6171-6180 wave).
 *   uint32_t gj_flatpak_appid_hash_wave_6180(void);
 *     - Wave fingerprint: 0x1817181C (6171<<16 | 6180).
 *   uint32_t __gj_batch_id_6180  (alias)
 *   uint32_t __gj_graph_milestone_6180  (alias)
 *   uint32_t __gj_flatpak_appid_hash_wave_6180  (alias)
 *   __libcgj_batch6180_marker = "libcgj-batch6180"
 *
 * MILESTONE 6180 for the exclusive continuum CREATE-ONLY wave
 * (batches 6171-6179: flatpak_appid_fnv1a32_6171, djb2_6172,
 * char_ok_6173, seg_count_6174, shape_ok_6175, len_ok/len_6176,
 * mix32_6177, prefix_hash_6178, hash_ready_6179). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* or milestone_*
 * symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6180_marker[] = "libcgj-batch6180";

#define B6180_BATCH_ID  6180u
/* Wave fingerprint: start<<16 | end  →  6171<<16 | 6180 */
#define B6180_WAVE_FP   ((6171u << 16) | 6180u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6180_id(void)
{
	return B6180_BATCH_ID;
}

static uint32_t
b6180_wave(void)
{
	return B6180_WAVE_FP;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_6180 - report this TU's graph batch number.
 *
 * Always returns 6180.
 */
uint32_t
gj_batch_id_6180(void)
{
	(void)NULL;
	return b6180_id();
}

/*
 * gj_graph_milestone_6180 - report this TU's graph milestone revision.
 *
 * Always returns 6180 (MILESTONE 6180). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_6180(void)
{
	return b6180_id();
}

/*
 * gj_flatpak_appid_hash_wave_6180 - flatpak app-id hash wave fingerprint.
 *
 * Always returns (6171 << 16) | 6180. Soft continuum identity tag for
 * the flatpak app id hash stubs wave. No parent wires.
 */
uint32_t
gj_flatpak_appid_hash_wave_6180(void)
{
	return b6180_wave();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_6180(void)
    __attribute__((alias("gj_batch_id_6180")));

uint32_t __gj_graph_milestone_6180(void)
    __attribute__((alias("gj_graph_milestone_6180")));

uint32_t __gj_flatpak_appid_hash_wave_6180(void)
    __attribute__((alias("gj_flatpak_appid_hash_wave_6180")));
