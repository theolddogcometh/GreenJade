/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * EL1 exception vector table (VBAR_EL1). Soft deepen keeps the public
 * vector ABI fixed: 16 slots × 0x80 bytes, table .align 11, vec index
 * 0..15 passed in x0 to aarch64_exception_dispatch.
 *
 * VBAR layout (ARM DDI, fixed offsets — do not reorder):
 *   +0x000  Current EL SP0:  Sync IRQ FIQ SError   (vec 0..3)
 *   +0x200  Current EL SPx:  Sync IRQ FIQ SError   (vec 4..7)
 *   +0x400  Lower EL AArch64: Sync IRQ FIQ SError  (vec 8..11)
 *   +0x600  Lower EL AArch32: Sync IRQ FIQ SError  (vec 12..15)
 * Class = vec & 3: 0=Sync 1=IRQ 2=FIQ 3=SError (see exception.c soft paths).
 *
 * Thin frame (soft; not full AAPCS64 yet — product next widens x0–x18):
 *   [sp+0]  = saved x0
 *   [sp+8]  = saved x1
 *   [sp+16] = saved x2   (stub temp; restored)
 *   [sp+24] = saved x3   (stub temp; restored)
 * PSCI probe fault recovery: if g_psci_probe_fault, overwrite saved x0
 * with g_psci_probe_result so eret returns NOT_SUPPORTED into the HVC/SMC
 * site; then one-shot clear fault so sticky state cannot poison later erets
 * (SVC selftest, etc.). g_psci_probe_result remains for C-side readers.
 *
 * SPSR_EL1 / ELR_EL1 left to hardware + C (ELR advance on PSCI recover).
 * Full GP/FP save is not done here — DAIF.I still held in product soft path.
 */
.section .text
.align 11
.global aarch64_vectors
.global aarch64_exception_dispatch
.global g_psci_probe_fault
.global g_psci_probe_result
.type aarch64_vectors, %function

.macro VEC n
    .align 7
    /* Preserve arg/result + stub temps (soft; keeps slot under 0x80). */
    stp  x0, x1, [sp, #-16]!
    stp  x2, x3, [sp, #-16]!
    mov  x0, #\n
    bl   aarch64_exception_dispatch
    /*
     * PSCI soft recover: force x0 result for the trapping HVC/SMC.
     * Saved x0 lives at [sp, #16] under the x2/x3 pair.
     */
    adrp x2, g_psci_probe_fault
    ldr  x3, [x2, :lo12:g_psci_probe_fault]
    cbz  x3, 1f
    adrp x0, g_psci_probe_result
    ldr  x0, [x0, :lo12:g_psci_probe_result]
    str  x0, [sp, #16]
    /* Soft one-shot: clear sticky fault after consume. */
    str  xzr, [x2, :lo12:g_psci_probe_fault]
1:
    ldp  x2, x3, [sp], #16
    ldp  x0, x1, [sp], #16
    eret
.endm

aarch64_vectors:
    /* Current EL, SP_EL0 */
    VEC 0
    VEC 1
    VEC 2
    VEC 3
    /* Current EL, SP_ELx (typical EL1 kernel) */
    VEC 4
    VEC 5
    VEC 6
    VEC 7
    /* Lower EL, AArch64 */
    VEC 8
    VEC 9
    VEC 10
    VEC 11
    /* Lower EL, AArch32 */
    VEC 12
    VEC 13
    VEC 14
    VEC 15

.size aarch64_vectors, .-aarch64_vectors

.section .note.GNU-stack,"",@progbits
