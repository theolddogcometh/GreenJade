/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * aarch64 bring-up: banner + GIC + timer probe, then idle.
 *
 * Greppable serial markers (scripts/run-aarch64.sh / make aarch64-smoke):
 *   aarch64: GIC PASS
 *   aarch64: timer PASS
 *   M0 OK
 *
 * Product path later: share C kernel with GJ_ARCH_AARCH64 shims.
 */
void aarch64_uart_puts(const char *sz);
void aarch64_gic_init(void);
void aarch64_timer_probe(void);

void
aarch64_kmain_stub(void)
{
    aarch64_uart_puts("GreenJade aarch64 M0+ (GIC+timer)\n");

    /* Prints "aarch64: GIC PASS" on success. */
    aarch64_gic_init();

    /* Prints "aarch64: timer PASS" on success (counter + soft vtimer). */
    aarch64_timer_probe();

    /* Final greppable M0 line (must contain exact "M0 OK"). */
    aarch64_uart_puts("M0 OK\n");

    for (;;) {
        __asm__ volatile("wfe");
    }
}
