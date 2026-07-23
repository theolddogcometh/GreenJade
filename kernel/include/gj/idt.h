/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Design: DESIGN_SPEC_COMPLETE G-IDT-*
 */
#pragma once

#include <gj/types.h>

void idt_init(void);

/** Non-zero after idt_init; required before untrusted ring-3. */
int idt_ready(void);

/** lidt the shared IDT (call on each AP after gdt_load_ap). */
void idt_load_ap(void);

/**
 * Install a 64-bit interrupt gate (type typically 0x8E).
 * Used for IRQ vectors after PIC remap.
 */
void idt_set_gate(u32 u32Vec, void *pHandler, u8 u8Type);
