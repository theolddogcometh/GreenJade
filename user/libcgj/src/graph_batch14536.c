/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14536: hda mixer spk soft name tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_hda_mixer_spk_name_14536(void);
 *     - Returns the soft HDA mixer spk control name tag for this
 *       continuum (always 0x53504B20, fourCC "SPK "). Not an ALSA
 *       control walk.
 *   uint32_t __gj_hda_mixer_spk_name_14536  (alias)
 *   __libcgj_batch14536_marker = "libcgj-batch14536"
 *
 * Exclusive continuum CREATE-ONLY (14531-14540: hda mixer soft
 * name tags — name fourCCs; soft_ready→0; batch_id→14540). Unique surface
 * only; no multi-def. Distinct from audio soft stubs (pipewire/alsa ok_u) and sibling
 * hda mixer soft name tags in prior waves. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14536_marker[] = "libcgj-batch14536";

/* HDA mixer spk soft name fourCC "SPK ". */
#define B14536_SPK  0x53504B20u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14536_name(void)
{
	return B14536_SPK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hda_mixer_spk_name_14536 - soft continuum surface
 *
 * Always returns 0x53504B20. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_hda_mixer_spk_name_14536(void)
{
	(void)NULL;
	return b14536_name();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hda_mixer_spk_name_14536(void)
    __attribute__((alias("gj_hda_mixer_spk_name_14536")));
