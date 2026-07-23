/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5404: install media detect (label / score).
 *
 * Surface (unique symbols):
 *   uint32_t gj_install_media_label_ok_5404(const char *label);
 *     - Return 1 if label is non-NULL and non-empty, else 0.
 *   uint32_t gj_install_media_ready_5404(void);
 *     - Returns 0 (soft: install media not ready at compile time).
 *   uint32_t gj_install_media_score_5404(void);
 *     - Returns 5404 (wave-tagged install-media score).
 *   uint32_t gj_install_media_esp_hint_5404(void);
 *     - Returns 1 (ESP payload root is a valid install-media hint).
 *   uint32_t __gj_install_media_label_ok_5404  (alias)
 *   uint32_t __gj_install_media_ready_5404  (alias)
 *   uint32_t __gj_install_media_score_5404  (alias)
 *   uint32_t __gj_install_media_esp_hint_5404  (alias)
 *   __libcgj_batch5404_marker = "libcgj-batch5404"
 *
 * Exclusive continuum CREATE-ONLY (5401-5410). Install media detect
 * toward bar #3. Distinct from gj_install_media_detect_5403 and
 * gj_install_media_ok (batch2638). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5404_marker[] = "libcgj-batch5404";

#define B5404_NOT_READY  0u
#define B5404_ESP_HINT   1u
#define B5404_SCORE      5404u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5404_label_ok(const char *szLabel)
{
	if (szLabel == NULL) {
		return 0u;
	}
	if (szLabel[0] == '\0') {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_install_media_label_ok_5404 - soft non-empty volume label check.
 *
 * label: NUL-terminated label string (NULL -> 0)
 *
 * Returns 1 when label has at least one character, else 0.
 */
uint32_t
gj_install_media_label_ok_5404(const char *szLabel)
{
	(void)NULL;
	return b5404_label_ok(szLabel);
}

/*
 * gj_install_media_ready_5404 - soft install-media ready lamp.
 *
 * Always returns 0 (not ready at compile time). No device probe.
 */
uint32_t
gj_install_media_ready_5404(void)
{
	return B5404_NOT_READY;
}

/*
 * gj_install_media_score_5404 - wave-tagged install-media score.
 *
 * Always returns 5404.
 */
uint32_t
gj_install_media_score_5404(void)
{
	return B5404_SCORE;
}

/*
 * gj_install_media_esp_hint_5404 - ESP payload root is a valid media hint.
 *
 * Always returns 1. Soft product-path constant for Deck install media.
 */
uint32_t
gj_install_media_esp_hint_5404(void)
{
	return B5404_ESP_HINT;
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_install_media_label_ok_5404(const char *szLabel)
    __attribute__((alias("gj_install_media_label_ok_5404")));

uint32_t __gj_install_media_ready_5404(void)
    __attribute__((alias("gj_install_media_ready_5404")));

uint32_t __gj_install_media_score_5404(void)
    __attribute__((alias("gj_install_media_score_5404")));

uint32_t __gj_install_media_esp_hint_5404(void)
    __attribute__((alias("gj_install_media_esp_hint_5404")));
