/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * EL1 exception vector table. On PSCI probe fault recovery, overwrite
 * saved x0 with g_psci_probe_result so eret returns NOT_SUPPORTED.
 */
.section .text
.align 11
.global aarch64_vectors
.global aarch64_exception_dispatch
.global g_psci_probe_fault
.global g_psci_probe_result

.macro VEC n
    .align 7
    stp x0, x1, [sp, #-16]!
    mov x0, #\n
    bl  aarch64_exception_dispatch
    /* If PSCI probe recovered a trap, force return value into x0. */
    adrp x2, g_psci_probe_fault
    ldr  w2, [x2, :lo12:g_psci_probe_fault]
    cbz  w2, 1f
    adrp x0, g_psci_probe_result
    ldr  x0, [x0, :lo12:g_psci_probe_result]
    str  x0, [sp]
1:
    ldp x0, x1, [sp], #16
    eret
.endm

aarch64_vectors:
    VEC 0
    VEC 1
    VEC 2
    VEC 3
    VEC 4
    VEC 5
    VEC 6
    VEC 7
    VEC 8
    VEC 9
    VEC 10
    VEC 11
    VEC 12
    VEC 13
    VEC 14
    VEC 15

.section .note.GNU-stack,"",@progbits
