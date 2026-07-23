/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Exception stubs + IRQ stubs + common trap path.
 *
 * Soft deepen (ABI-stable frames — trap.h / IDT must match):
 *
 *   Exception path (isr_stub_N → isr_common → trap_dispatch):
 *     CPU pushes SS/RSP/RFLAGS/CS/RIP [+ error] for ring transitions.
 *     Stub pushes error (0 if none) then vector, then isr_common saves
 *     all GPRs. Resulting layout is struct gj_trap_frame (gj/trap.h):
 *       r15 r14 r13 r12 r11 r10 r9 r8 rbp rdi rsi rdx rcx rbx rax
 *       vector error rip cs rflags rsp ss
 *     isr_common: cld + call trap_dispatch(rdi = frame*). Rare return
 *     restores GPRs, drops vector+error, iretq.
 *
 *   IRQ path (irq_stub_*): save only SysV caller-saved GPRs, cld, call
 *     C handler (EOI inside C), restore, iretq. No trap_frame — handlers
 *     are short notify/EOI paths (P-IRQ-4 spirit).
 *
 *   Vectors of note:
 *     0..31  CPU exceptions (error-code vectors: 8,10-14,17,30)
 *     48     Local APIC timer
 *     49     reschedule / wake IPI
 *     50     TLB shootdown IPI
 *     0x41   MSI-X device → Notification
 *     128    i386 Linux int 0x80 (PE32/WoW64 compat)
 *
 * Grep: isr_common, irq_stub, trap: soft stats
 */
.section .text

.macro ISR_NOERR n
.global isr_stub_\n
.type isr_stub_\n, @function
isr_stub_\n:
    pushq $0
    pushq $\n
    jmp isr_common
.endm

.macro ISR_ERR n
.global isr_stub_\n
.type isr_stub_\n, @function
isr_stub_\n:
    pushq $\n
    jmp isr_common
.endm

/*
 * Shared IRQ prologue/epilogue: caller-saved only (SysV).
 * Handler symbol is an extern C function; must EOI as needed.
 */
.macro IRQ_STUB name, handler
.global \name
.type \name, @function
.extern \handler
\name:
    push %rax
    push %rcx
    push %rdx
    push %rsi
    push %rdi
    push %r8
    push %r9
    push %r10
    push %r11
    cld
    call \handler
    pop %r11
    pop %r10
    pop %r9
    pop %r8
    pop %rdi
    pop %rsi
    pop %rdx
    pop %rcx
    pop %rax
    iretq
.endm

ISR_NOERR 0
ISR_NOERR 1
ISR_NOERR 2
ISR_NOERR 3
ISR_NOERR 4
ISR_NOERR 5
ISR_NOERR 6
ISR_NOERR 7
ISR_ERR   8
ISR_NOERR 9
ISR_ERR   10
ISR_ERR   11
ISR_ERR   12
ISR_ERR   13
ISR_ERR   14
ISR_NOERR 15
ISR_NOERR 16
ISR_ERR   17
ISR_NOERR 18
ISR_NOERR 19
ISR_NOERR 20
ISR_NOERR 21
ISR_NOERR 22
ISR_NOERR 23
ISR_NOERR 24
ISR_NOERR 25
ISR_NOERR 26
ISR_NOERR 27
ISR_NOERR 28
ISR_NOERR 29
ISR_ERR   30
ISR_NOERR 31

/* i386 Linux int 0x80 (vector 128) — PE32/WoW64 compat syscall */
.global isr_stub_128
.type isr_stub_128, @function
isr_stub_128:
    pushq $0
    pushq $128
    jmp isr_common

/* IRQ0 timer (legacy PIT path) — no error code; EOI in C handler */
IRQ_STUB irq_stub_0, irq_timer_handler

/* Local APIC timer (vector 48) */
IRQ_STUB irq_stub_apic_timer, apic_timer_irq

/* Reschedule / wake IPI (vector 49) — EOI only, wakes HLT */
IRQ_STUB irq_stub_ipi_resched, apic_ipi_resched_irq

/* TLB shootdown IPI (vector 50) */
IRQ_STUB irq_stub_ipi_tlb, apic_ipi_tlb_irq

/* MSI-X device IRQ (vector 0x41) → Notification */
IRQ_STUB irq_stub_msix, irq_msix_handler

.extern trap_dispatch
.global isr_common
.type isr_common, @function
isr_common:
    /* Full GPR save → struct gj_trap_frame (see trap.h). */
    push %rax
    push %rbx
    push %rcx
    push %rdx
    push %rsi
    push %rdi
    push %rbp
    push %r8
    push %r9
    push %r10
    push %r11
    push %r12
    push %r13
    push %r14
    push %r15
    mov %rsp, %rdi
    cld                       /* SysV: DF clear before C */
    call trap_dispatch
    /* If returns: restore and iretq (rare — user kill may return to idle) */
    pop %r15
    pop %r14
    pop %r13
    pop %r12
    pop %r11
    pop %r10
    pop %r9
    pop %r8
    pop %rbp
    pop %rdi
    pop %rsi
    pop %rdx
    pop %rcx
    pop %rbx
    pop %rax
    add $16, %rsp   /* vector + error */
    iretq

.size isr_common, . - isr_common

.section .note.GNU-stack,"",@progbits
