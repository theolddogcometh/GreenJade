/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5509: ESP minimum size (MiB).
 *
 * Surface (unique symbols):
 *   uint32_t gj_esp_size_min_mib_5509(void);
 *     - Returns 512 (soft recommended minimum EFI System Partition
 *       size in mebibytes for Deck install continuum).
 *   uint32_t __gj_esp_size_min_mib_5509  (alias)
 *   __libcgj_batch5509_marker = "libcgj-batch5509"
 *
 * Post-milestone 5500 exclusive continuum CREATE-ONLY (5501-5510: Deck
 * install media verification / checksum / volume labels / ESP size).
 * Unique gj_esp_size_min_mib_5509 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5509_marker[] = "libcgj-batch5509";

/* Soft recommended ESP minimum (MiB). */
#define B5509_ESP_MIN_MIB  512u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5509_esp_min_mib(void)
{
	return B5509_ESP_MIN_MIB;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_esp_size_min_mib_5509 - report soft minimum ESP size in MiB.
 *
 * Always returns 512. Compile-time product-path constant for Deck
 * install media / partitioning verification. Does not probe disks.
 * No parent wires.
 */
uint32_t
gj_esp_size_min_mib_5509(void)
{
	(void)NULL;
	return b5509_esp_min_mib();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_esp_size_min_mib_5509(void)
    __attribute__((alias("gj_esp_size_min_mib_5509")));
