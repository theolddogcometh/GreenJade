/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5733: PCI host-bridge class tag (APU root).
 *
 * Surface (unique symbols):
 *   uint32_t gj_pci_class_host_bridge_5733(void);
 *     - Returns packed PCI class code for host bridge: 0x060000
 *       (base 0x06 / sub 0x00 / prog-if 0x00). Soft product tag for
 *       Deck APU root complex enum.
 *   uint32_t gj_pci_class_is_host_bridge_5733(uint32_t class_code);
 *     - Return 1 if (class_code & 0xFFFF00) == 0x060000, else 0.
 *   uint32_t __gj_pci_class_host_bridge_5733  (alias)
 *   uint32_t __gj_pci_class_is_host_bridge_5733  (alias)
 *   __libcgj_batch5733_marker = "libcgj-batch5733"
 *
 * Exclusive continuum CREATE-ONLY (5731-5740: real-hw PCI enum stubs
 * for Deck APU / USB / NVMe). Unique *_5733 surfaces only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5733_marker[] = "libcgj-batch5733";

/* PCI class: base=bridge(0x06), sub=host(0x00), prog-if=0. */
#define B5733_CLASS_HOST_BRIDGE  0x060000u
#define B5733_CLASS_MASK         0xFFFF00u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5733_host_bridge_class(void)
{
	return B5733_CLASS_HOST_BRIDGE;
}

static uint32_t
b5733_is_host_bridge(uint32_t u32Class)
{
	if ((u32Class & B5733_CLASS_MASK) == B5733_CLASS_HOST_BRIDGE) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pci_class_host_bridge_5733 - packed host-bridge class code.
 *
 * Always returns 0x060000. Soft pure-data only; no config read.
 */
uint32_t
gj_pci_class_host_bridge_5733(void)
{
	(void)NULL;
	return b5733_host_bridge_class();
}

/*
 * gj_pci_class_is_host_bridge_5733 - match host-bridge base/sub class.
 *
 * class_code: 24-bit class|subclass|progif packed as 0xBBSSPP
 * Returns 1 on host-bridge match (prog-if ignored via mask), else 0.
 */
uint32_t
gj_pci_class_is_host_bridge_5733(uint32_t u32Class)
{
	return b5733_is_host_bridge(u32Class);
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_pci_class_host_bridge_5733(void)
    __attribute__((alias("gj_pci_class_host_bridge_5733")));

uint32_t __gj_pci_class_is_host_bridge_5733(uint32_t u32Class)
    __attribute__((alias("gj_pci_class_is_host_bridge_5733")));
