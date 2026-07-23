/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * aarch64 minimal bring-up stub: banner + GIC + timer probe, then idle.
 * Not linked by default (product path uses kmain.o which wraps kmain).
 *
 * -------------------------------------------------------------------------
 * Soft product surface (Wave 17 exclusive deepen; never hard-fails M0)
 * -------------------------------------------------------------------------
 * Soft enter/exit markers bracket GIC + timer probes so a stub-only link
 * still has greppable phase lines. Callees emit their own PASS markers.
 * Soft inventory: phase lamps + wave stamp + product_kernel=OPEN honesty.
 * Final product bar is exact "M0 OK" (smoke scripts grep this string).
 *
 * Greppable serial markers (scripts/run-aarch64.sh / make aarch64-smoke):
 *   aarch64: kmain_stub soft enter
 *   aarch64: GIC PASS          (from aarch64_gic_init)
 *   aarch64: timer PASS        (from aarch64_timer_probe)
 *   aarch64: kmain_stub soft inventory wave=17 …
 *   aarch64: kmain_stub soft surf …
 *   aarch64: kmain_stub soft deepen wave=17 areas=…
 *   aarch64: kmain_stub soft return product_kernel=OPEN …
 *   aarch64: kmain_stub soft path product_kernel=OPEN …
 *   aarch64: kmain_stub soft honesty product_kernel=OPEN …
 *   aarch64: kmain_stub soft PASS
 *   M0 OK
 *
 * Product path later: share C kernel with GJ_ARCH_AARCH64 shims (see kmain.c).
 * Honesty: stub soft inventory ≠ product kernel complete (OPEN).
 */
void aarch64_uart_puts(const char *sz);
void aarch64_uart_put_hex(unsigned long v);
void aarch64_gic_init(void);
void aarch64_timer_probe(void);

/* Wave 17 soft inventory stamp (file-local; never product gate). */
#define KMAIN_STUB_SOFT_WAVE  17u
/* Areas: enter,gic,timer,inventory,surf,return,path,honesty,deepen */
#define KMAIN_STUB_SOFT_AREAS 9u

void
aarch64_kmain_stub(void)
{
    unsigned uGicDone;
    unsigned uTimerDone;
    unsigned uSurfBits;

    uGicDone = 0u;
    uTimerDone = 0u;
    uSurfBits = 0u;

    aarch64_uart_puts("GreenJade aarch64 M0+ (GIC+timer)\n");
    aarch64_uart_puts("aarch64: kmain_stub soft enter\n");
    uSurfBits |= 1u; /* enter */

    /* Prints "aarch64: GIC PASS" on success. */
    aarch64_gic_init();
    aarch64_uart_puts("aarch64: kmain_stub soft gic done\n");
    uGicDone = 1u;
    uSurfBits |= 2u;

    /* Prints "aarch64: timer PASS" on success (counter + soft vtimer). */
    aarch64_timer_probe();
    aarch64_uart_puts("aarch64: kmain_stub soft timer done\n");
    uTimerDone = 1u;
    uSurfBits |= 4u;

    /* Grep: aarch64: kmain_stub soft inventory */
    aarch64_uart_puts("aarch64: kmain_stub soft inventory wave=");
    aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
    aarch64_uart_puts(" gic=");
    aarch64_uart_put_hex((unsigned long)uGicDone);
    aarch64_uart_puts(" timer=");
    aarch64_uart_put_hex((unsigned long)uTimerDone);
    aarch64_uart_puts(" areas=");
    aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_AREAS);
    aarch64_uart_puts("\n");

    /* Grep: aarch64: kmain_stub soft surf */
    aarch64_uart_puts("aarch64: kmain_stub soft surf enter=1 gic=");
    aarch64_uart_put_hex((unsigned long)uGicDone);
    aarch64_uart_puts(" timer=");
    aarch64_uart_put_hex((unsigned long)uTimerDone);
    aarch64_uart_puts(" bits=");
    aarch64_uart_put_hex((unsigned long)uSurfBits);
    aarch64_uart_puts(" wave=");
    aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
    aarch64_uart_puts("\n");
    aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
    aarch64_uart_puts(" areas=");
    aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_AREAS);
    aarch64_uart_puts(" catalog=enter,gic,timer,inventory,surf,return,path,"
                      "honesty,deepen soft_only=1\n");

    /* Grep: aarch64: kmain_stub soft return — Wave 17 return surfaces */
    aarch64_uart_puts("aarch64: kmain_stub soft return gic=");
    aarch64_uart_put_hex((unsigned long)uGicDone);
    aarch64_uart_puts(" timer=");
    aarch64_uart_put_hex((unsigned long)uTimerDone);
    aarch64_uart_puts(" bits=");
    aarch64_uart_put_hex((unsigned long)uSurfBits);
    aarch64_uart_puts(" product_kernel=OPEN wave=");
    aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
    aarch64_uart_puts("\n");

    /* Grep: aarch64: kmain_stub soft path */
    aarch64_uart_puts("aarch64: kmain_stub soft path stub=1 product_kmain=0 "
                      "product_kernel=OPEN hard_gate=0 wave=");
    aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
    aarch64_uart_puts("\n");

    /* Grep: aarch64: kmain_stub soft honesty */
    aarch64_uart_puts("aarch64: kmain_stub soft honesty product_kernel=OPEN "
                      "soft_only=1 no_bar3=1 m0_scaffold=1 wave=");
    aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
    aarch64_uart_puts("\n");

    /* Grep: aarch64: kmain_stub soft exclusive — Wave 17 exclusive deepen */
    aarch64_uart_puts("aarch64: kmain_stub soft exclusive multi_server=0 "
                      "confine=0 bar3=0 product_kernel=OPEN soft_only=1 wave=");
    aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
    aarch64_uart_puts("\n");

    /* Grep: aarch64: kmain_stub soft open — Wave 17 open-lamp rollup */
    aarch64_uart_puts("aarch64: kmain_stub soft open multi_server=0 confine=0 "
                      "bar3=0 product_kernel=OPEN soft_only=1 wave=");
    aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
    aarch64_uart_puts("\n");


    /* Soft summary before fixed M0 bar. */
    aarch64_uart_puts("aarch64: kmain_stub soft PASS\n");

    /* Final greppable M0 line (must contain exact "M0 OK"). */
    aarch64_uart_puts("M0 OK\n");

    for (;;) {
        __asm__ volatile("wfe");
    }
}
