/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6146: BT audio codec select tag stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bt_audio_codec_tag_6146(uint32_t codec_id);
 *     - Soft BT audio codec fourCC-style tag for a soft codec_id:
 *       0 → 0x53424320 ("SBC "), 1 → 0x41414320 ("AAC "),
 *       2 → 0x41505458 ("APTX"), 3 → 0x4C444143 ("LDAC"),
 *       else 0. Pure data; does not negotiate codecs with BlueZ.
 *   uint32_t __gj_bt_audio_codec_tag_6146  (alias)
 *   __libcgj_batch6146_marker = "libcgj-batch6146"
 *
 * Exclusive continuum CREATE-ONLY (6141-6150: bluez/bt audio stubs).
 * Unique gj_bt_audio_codec_tag_6146 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6146_marker[] = "libcgj-batch6146";

#define B6146_TAG_SBC   0x53424320u /* "SBC " */
#define B6146_TAG_AAC   0x41414320u /* "AAC " */
#define B6146_TAG_APTX  0x41505458u /* "APTX" */
#define B6146_TAG_LDAC  0x4C444143u /* "LDAC" */

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6146_codec_tag(uint32_t u32Id)
{
	if (u32Id == 0u)
		return B6146_TAG_SBC;
	if (u32Id == 1u)
		return B6146_TAG_AAC;
	if (u32Id == 2u)
		return B6146_TAG_APTX;
	if (u32Id == 3u)
		return B6146_TAG_LDAC;
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bt_audio_codec_tag_6146 - BT audio codec select tag stub.
 *
 * codec_id: soft codec index (0=SBC, 1=AAC, 2=APTX, 3=LDAC)
 *
 * Returns the corresponding fourCC tag, or 0 if unknown. Soft pure-data
 * codec lamp. No parent wires.
 */
uint32_t
gj_bt_audio_codec_tag_6146(uint32_t codec_id)
{
	(void)NULL;
	return b6146_codec_tag(codec_id);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bt_audio_codec_tag_6146(uint32_t codec_id)
    __attribute__((alias("gj_bt_audio_codec_tag_6146")));
