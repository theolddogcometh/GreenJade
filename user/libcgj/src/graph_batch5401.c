/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5401: Deck native ABI probe (core).
 *
 * Surface (unique symbols):
 *   uint32_t gj_deck_native_abi_probe_5401(void);
 *     - Returns 1 (soft native-ABI probe lamp for Steam Deck install).
 *   uint32_t gj_deck_abi_x64_ok_5401(void);
 *     - Returns 1 (x86_64 ABI considered present for Deck product path).
 *   uint32_t gj_deck_abi_sse2_ok_5401(void);
 *     - Returns 1 (SSE2 required by freestanding Deck build path).
 *   uint32_t gj_batch_id_5401(void);
 *     - Returns 5401 (this TU's graph batch number).
 *   uint32_t __gj_deck_native_abi_probe_5401  (alias)
 *   uint32_t __gj_deck_abi_x64_ok_5401  (alias)
 *   uint32_t __gj_deck_abi_sse2_ok_5401  (alias)
 *   uint32_t __gj_batch_id_5401  (alias)
 *   __libcgj_batch5401_marker = "libcgj-batch5401"
 *
 * Exclusive continuum CREATE-ONLY (5401-5410: Deck native ABI probe,
 * install media detect, partition layout, bootloader EFI path, steam
 * runtime roots). Toward bar #3 (Steam Deck install / Deck Top 50 /
 * real-hw). Unique batch-suffixed symbols only; no multi-def of
 * gj_batch_id / gj_graph_milestone / bare names. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5401_marker[] = "libcgj-batch5401";

#define B5401_OK     1u
#define B5401_BATCH  5401u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5401_ok(void)
{
	return B5401_OK;
}

static uint32_t
b5401_id(void)
{
	return B5401_BATCH;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deck_native_abi_probe_5401 - soft Deck native ABI probe lamp.
 *
 * Always returns 1. Compile-time product-path tag for bar #3 Deck
 * install; does not probe hardware or call libc. No parent wires.
 */
uint32_t
gj_deck_native_abi_probe_5401(void)
{
	(void)NULL;
	return b5401_ok();
}

/*
 * gj_deck_abi_x64_ok_5401 - soft x86_64 ABI readiness for Deck path.
 *
 * Always returns 1 (x64 surface assumed for freestanding Deck builds).
 */
uint32_t
gj_deck_abi_x64_ok_5401(void)
{
	return b5401_ok();
}

/*
 * gj_deck_abi_sse2_ok_5401 - soft SSE2 readiness for Deck path.
 *
 * Always returns 1 (build requires -msse2). No CPUID at runtime.
 */
uint32_t
gj_deck_abi_sse2_ok_5401(void)
{
	return b5401_ok();
}

/*
 * gj_batch_id_5401 - report this TU's graph batch number.
 *
 * Always returns 5401.
 */
uint32_t
gj_batch_id_5401(void)
{
	return b5401_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_deck_native_abi_probe_5401(void)
    __attribute__((alias("gj_deck_native_abi_probe_5401")));

uint32_t __gj_deck_abi_x64_ok_5401(void)
    __attribute__((alias("gj_deck_abi_x64_ok_5401")));

uint32_t __gj_deck_abi_sse2_ok_5401(void)
    __attribute__((alias("gj_deck_abi_sse2_ok_5401")));

uint32_t __gj_batch_id_5401(void)
    __attribute__((alias("gj_batch_id_5401")));
