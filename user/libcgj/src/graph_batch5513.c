/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5513: kernel modules.alias path probe tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_kmod_modules_alias_probe_5513(void);
 *     - Returns 1 (modules.alias probe surface ready). Soft compile-time
 *       stub for alias/modalias tables.
 *   uint32_t __gj_kmod_modules_alias_probe_5513  (alias)
 *   __libcgj_batch5513_marker = "libcgj-batch5513"
 *
 * Exclusive CREATE-ONLY (5511-5520: kernel modules path probes, firmware
 * blobs list stubs, amdgpu/mesa readiness for Deck APU). Unique
 * gj_kmod_modules_alias_probe_5513 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5513_marker[] = "libcgj-batch5513";

/* Soft lamp: modules.alias path probe surface ready. */
#define B5513_MODULES_ALIAS_PROBE  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5513_modules_alias_probe(void)
{
	return B5513_MODULES_ALIAS_PROBE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_kmod_modules_alias_probe_5513 - modules.alias path probe readiness.
 *
 * Always returns 1. Soft pure-data tag for modules.alias / modalias
 * tables; does not parse alias maps. Does not call libc. No parent wires.
 */
uint32_t
gj_kmod_modules_alias_probe_5513(void)
{
	(void)NULL;
	return b5513_modules_alias_probe();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_kmod_modules_alias_probe_5513(void)
    __attribute__((alias("gj_kmod_modules_alias_probe_5513")));
