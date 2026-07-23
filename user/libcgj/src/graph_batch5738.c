/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5738: NVMe PCI class / prog-if match.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pci_nvme_class_code_5738(void);
 *     - Returns packed NVMe class: 0x010802 (mass storage 0x01,
 *       non-volatile memory 0x08, NVMe 0x02) per public PCI codes.
 *   uint32_t gj_pci_nvme_class_ok_5738(uint32_t class_code);
 *     - Return 1 if class_code matches 0x010802 exactly, else 0.
 *   uint32_t __gj_pci_nvme_class_code_5738  (alias)
 *   uint32_t __gj_pci_nvme_class_ok_5738  (alias)
 *   __libcgj_batch5738_marker = "libcgj-batch5738"
 *
 * Exclusive continuum CREATE-ONLY (5731-5740: real-hw PCI enum stubs
 * for Deck APU / USB / NVMe). Unique *_5738 surfaces only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5738_marker[] = "libcgj-batch5738";

/* PCI: mass-storage / NVM / NVMe I/O controller prog-if. */
#define B5738_NVME_CLASS  0x010802u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5738_nvme_class(void)
{
	return B5738_NVME_CLASS;
}

static uint32_t
b5738_nvme_ok(uint32_t u32Class)
{
	if (u32Class == B5738_NVME_CLASS) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pci_nvme_class_code_5738 - packed NVMe class constant.
 *
 * Always returns 0x010802. Soft tag for Deck onboard NVMe enum.
 */
uint32_t
gj_pci_nvme_class_code_5738(void)
{
	(void)NULL;
	return b5738_nvme_class();
}

/*
 * gj_pci_nvme_class_ok_5738 - exact NVMe class match.
 *
 * class_code: 24-bit packed class from config header
 * Returns 1 on 0x010802, else 0. No BAR map / no parent wires.
 */
uint32_t
gj_pci_nvme_class_ok_5738(uint32_t u32Class)
{
	return b5738_nvme_ok(u32Class);
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_pci_nvme_class_code_5738(void)
    __attribute__((alias("gj_pci_nvme_class_code_5738")));

uint32_t __gj_pci_nvme_class_ok_5738(uint32_t u32Class)
    __attribute__((alias("gj_pci_nvme_class_ok_5738")));
