/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5736: USB xHCI PCI class / prog-if match.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pci_usb_xhci_class_code_5736(void);
 *     - Returns packed USB xHCI class: 0x0C0330 (base 0x0C serial bus,
 *       sub 0x03 USB, prog-if 0x30 xHCI) per public PCI class codes.
 *   uint32_t gj_pci_usb_xhci_class_ok_5736(uint32_t class_code);
 *     - Return 1 if class_code matches 0x0C0330 exactly, else 0.
 *   uint32_t __gj_pci_usb_xhci_class_code_5736  (alias)
 *   uint32_t __gj_pci_usb_xhci_class_ok_5736  (alias)
 *   __libcgj_batch5736_marker = "libcgj-batch5736"
 *
 * Exclusive continuum CREATE-ONLY (5731-5740: real-hw PCI enum stubs
 * for Deck APU / USB / NVMe). Unique *_5736 surfaces only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5736_marker[] = "libcgj-batch5736";

/* PCI: serial-bus / USB / xHCI prog-if. */
#define B5736_XHCI_CLASS  0x0C0330u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5736_xhci_class(void)
{
	return B5736_XHCI_CLASS;
}

static uint32_t
b5736_xhci_ok(uint32_t u32Class)
{
	if (u32Class == B5736_XHCI_CLASS) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pci_usb_xhci_class_code_5736 - packed xHCI class constant.
 *
 * Always returns 0x0C0330. Soft tag for Deck USB host enum.
 */
uint32_t
gj_pci_usb_xhci_class_code_5736(void)
{
	(void)NULL;
	return b5736_xhci_class();
}

/*
 * gj_pci_usb_xhci_class_ok_5736 - exact xHCI class match.
 *
 * class_code: 24-bit packed class from config header
 * Returns 1 on 0x0C0330, else 0. No MMIO / no parent wires.
 */
uint32_t
gj_pci_usb_xhci_class_ok_5736(uint32_t u32Class)
{
	return b5736_xhci_ok(u32Class);
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_pci_usb_xhci_class_code_5736(void)
    __attribute__((alias("gj_pci_usb_xhci_class_code_5736")));

uint32_t __gj_pci_usb_xhci_class_ok_5736(uint32_t u32Class)
    __attribute__((alias("gj_pci_usb_xhci_class_ok_5736")));
