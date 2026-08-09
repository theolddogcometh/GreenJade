/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Product platform attention / power:
 *   Ctrl+Alt+Del  → reboot (KBC 0xFE, fallback 0xCF9)
 *   ACPI power btn → shutdown (PM1 S5) then reboot fallback
 *
 * Poll-mode on scheduler thr stack (same H1 class as net_eth_poll).
 * Soft!=product residual lamps; Dual MIT OR Apache-2.0. G-AC-1.
 *
 * greppable: platform_power: | Ctrl+Alt+Del | power button | S5
 */
#pragma once

#include <gj/types.h>

/** Discover FADT PM1 + enable power-button events. Safe to re-call. */
void platform_power_init(void);

/**
 * Drain PS/2 scancodes + sample ACPI PWRBTN.
 * On CAD: reboot. On power button: ACPI poweroff (then reboot fallback).
 * Call from BSP scheduler thr stack only (never IRQ).
 */
void platform_power_poll(void);

/** Immediate reboot (KBC then CF9 then HLT). Does not return. */
void platform_power_reboot(void) __attribute__((noreturn));

/** ACPI S5 poweroff if PM1 known; else reboot. Does not return. */
void platform_power_shutdown(void) __attribute__((noreturn));

/** Soft tallies (diagnostics). */
u32 platform_power_cad_hits(void);
u32 platform_power_pwrbtn_hits(void);
int platform_power_acpi_ok(void);
