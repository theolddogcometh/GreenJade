/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6134: PipeWire media-class tag stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pw_media_class_tag_6134(uint32_t class_kind);
 *     - Map a soft media-class kind to a fourCC tag:
 *         0 → 0x41756E6B ("Aunk" unknown)
 *         1 → 0x41736E6B ("Asnk" Audio/Sink)
 *         2 → 0x41737263 ("Asrc" Audio/Source)
 *         3 → 0x44756E6B ("Dunk" Duplex / other)
 *       Any other value → unknown tag.
 *   uint32_t __gj_pw_media_class_tag_6134  (alias)
 *   __libcgj_batch6134_marker = "libcgj-batch6134"
 *
 * Exclusive continuum CREATE-ONLY (6131-6140: pipewire node graph
 * stubs). Unique gj_pw_media_class_tag_6134 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6134_marker[] = "libcgj-batch6134";

/* Soft media-class fourCC tags (ASCII mnemonics, not on-wire SPA). */
#define B6134_TAG_UNK   0x41756E6Bu /* "Aunk" */
#define B6134_TAG_SINK  0x41736E6Bu /* "Asnk" */
#define B6134_TAG_SRC   0x41737263u /* "Asrc" */
#define B6134_TAG_DUP   0x44756E6Bu /* "Dunk" */

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6134_media_class_tag(uint32_t uKind)
{
	if (uKind == 1u) {
		return B6134_TAG_SINK;
	}
	if (uKind == 2u) {
		return B6134_TAG_SRC;
	}
	if (uKind == 3u) {
		return B6134_TAG_DUP;
	}
	return B6134_TAG_UNK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pw_media_class_tag_6134 - map soft media-class kind to fourCC.
 *
 * class_kind: 0 unknown, 1 sink, 2 source, 3 duplex/other.
 * Returns fixed fourCC tag. Soft pure-data stub. No parent wires.
 */
uint32_t
gj_pw_media_class_tag_6134(uint32_t uClassKind)
{
	(void)NULL;
	return b6134_media_class_tag(uClassKind);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pw_media_class_tag_6134(uint32_t uClassKind)
    __attribute__((alias("gj_pw_media_class_tag_6134")));
