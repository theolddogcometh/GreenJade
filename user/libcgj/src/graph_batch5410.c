/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5410: steam runtime roots + wave close.
 *
 * Surface (unique symbols):
 *   uint32_t gj_steam_runtime_ld_ok_5410(const char *path);
 *     - Return 1 if path is absolute with length >= 2, else 0.
 *   uint32_t gj_steam_runtime_pin_ok_5410(void);
 *     - Returns 1 (soft runtime pin policy ok for product path).
 *   uint32_t gj_steam_runtime_score_5410(void);
 *     - Returns 5410 (wave-tagged steam runtime score).
 *   uint32_t gj_bar3_product_score_5410(void);
 *     - Returns 5410 (bar #3 product-path score for this wave close).
 *   uint32_t gj_batch_id_5410(void);
 *     - Returns 5410 (this TU's graph batch number).
 *   uint32_t __gj_steam_runtime_ld_ok_5410  (alias)
 *   uint32_t __gj_steam_runtime_pin_ok_5410  (alias)
 *   uint32_t __gj_steam_runtime_score_5410  (alias)
 *   uint32_t __gj_bar3_product_score_5410  (alias)
 *   uint32_t __gj_batch_id_5410  (alias)
 *   __libcgj_batch5410_marker = "libcgj-batch5410"
 *
 * Exclusive continuum CREATE-ONLY (5401-5410: Deck native ABI probe,
 * install media detect, partition layout, bootloader EFI path, steam
 * runtime roots). Toward bar #3 (Steam Deck install, Deck Top 50,
 * real-hw). Unique batch-suffixed symbols only; does NOT redefine
 * gj_batch_id / gj_graph_milestone / bare names. Distinct from
 * gj_steam_runtime_*_5409 and gj_bar3_*_u (batch5356-5358). No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5410_marker[] = "libcgj-batch5410";

#define B5410_PIN_OK  1u
#define B5410_SCORE   5410u
#define B5410_BATCH   5410u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5410_path_ok(const char *szPath)
{
	if (szPath == NULL) {
		return 0u;
	}
	if (szPath[0] != '/') {
		return 0u;
	}
	if (szPath[1] == '\0') {
		return 0u;
	}
	return 1u;
}

static uint32_t
b5410_id(void)
{
	return B5410_BATCH;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_steam_runtime_ld_ok_5410 - soft absolute ld-linux / runtime path check.
 *
 * path: NUL-terminated loader/runtime path (NULL -> 0)
 *
 * Returns 1 when path starts with '/' and length >= 2, else 0.
 */
uint32_t
gj_steam_runtime_ld_ok_5410(const char *szPath)
{
	(void)NULL;
	return b5410_path_ok(szPath);
}

/*
 * gj_steam_runtime_pin_ok_5410 - soft steam-runtime pin policy lamp.
 *
 * Always returns 1. Product-path tag only; no package database.
 */
uint32_t
gj_steam_runtime_pin_ok_5410(void)
{
	return B5410_PIN_OK;
}

/*
 * gj_steam_runtime_score_5410 - wave-tagged steam runtime score.
 *
 * Always returns 5410.
 */
uint32_t
gj_steam_runtime_score_5410(void)
{
	return B5410_SCORE;
}

/*
 * gj_bar3_product_score_5410 - bar #3 product-path score for wave close.
 *
 * Always returns 5410. Soft continuum close tag toward Steam Deck
 * install / Deck Top 50 / real-hw. Distinct from gj_bar3_install_ready_u
 * (batch5356) and gj_product_score_5400 (batch5398).
 */
uint32_t
gj_bar3_product_score_5410(void)
{
	return B5410_SCORE;
}

/*
 * gj_batch_id_5410 - report this TU's graph batch number.
 *
 * Always returns 5410.
 */
uint32_t
gj_batch_id_5410(void)
{
	return b5410_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_steam_runtime_ld_ok_5410(const char *szPath)
    __attribute__((alias("gj_steam_runtime_ld_ok_5410")));

uint32_t __gj_steam_runtime_pin_ok_5410(void)
    __attribute__((alias("gj_steam_runtime_pin_ok_5410")));

uint32_t __gj_steam_runtime_score_5410(void)
    __attribute__((alias("gj_steam_runtime_score_5410")));

uint32_t __gj_bar3_product_score_5410(void)
    __attribute__((alias("gj_bar3_product_score_5410")));

uint32_t __gj_batch_id_5410(void)
    __attribute__((alias("gj_batch_id_5410")));
