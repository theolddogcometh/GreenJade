/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14537: hda mixer beep soft name tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_hda_mixer_beep_name_14537(void);
 *     - Returns the soft HDA mixer beep control name tag for this
 *       continuum (always 0x42454550, fourCC "BEEP"). Not an ALSA
 *       control walk.
 *   uint32_t __gj_hda_mixer_beep_name_14537  (alias)
 *   __libcgj_batch14537_marker = "libcgj-batch14537"
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

const char __libcgj_batch14537_marker[] = "libcgj-batch14537";

/* HDA mixer beep soft name fourCC "BEEP". */
#define B14537_BEEP  0x42454550u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14537_name(void)
{
	return B14537_BEEP;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hda_mixer_beep_name_14537 - soft continuum surface
 *
 * Always returns 0x42454550. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_hda_mixer_beep_name_14537(void)
{
	(void)NULL;
	return b14537_name();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hda_mixer_beep_name_14537(void)
    __attribute__((alias("gj_hda_mixer_beep_name_14537")));
