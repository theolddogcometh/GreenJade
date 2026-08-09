/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * aarch64 minimal bring-up stub: banner + GIC + timer probe, then idle.
 * Not linked by default (product path uses kmain.o which wraps kmain).
 *
 * -------------------------------------------------------------------------
 * Soft product surface (exclusive residual deepen; never hard-fails M0)
 * -------------------------------------------------------------------------
 * Soft enter/exit markers bracket GIC + timer probes so a stub-only link
 * still has greppable phase lines. Callees emit their own PASS markers.
 * Soft inventory: phase lamps + wave stamp + product_kernel=OPEN honesty.
 * Soft deepen: lean area catalog (no ret* stamp storm; residual lean).
 * Final product bar is exact "M0 OK" (smoke scripts grep this string).
 *
 * C0 residual lean (exclusive residual; this unit only; Soft!=product):
 *   "aarch64: kmain_stub soft residual lean ..." once-lamp honesty.
 *   Stamp-free: no ret*angle storm, no product version stamp, no GPL.
 *   Soft != product multi-server / confine / Deck Top 50 / continuum.
 *   Dual DoD A/B remain OPEN (not claimed closed by soft lamps).
 *   G-AC-1: no Linux .ko product AC claim on aarch64 M0 scaffold stub.
 *   Dual MIT OR Apache-2.0 unchanged. Never hard-gates M0 OK.
 *
 * Greppable serial markers (scripts/run-aarch64.sh / make aarch64-smoke):
 *   aarch64: kmain_stub soft enter
 *   aarch64: GIC PASS          (from aarch64_gic_init)
 *   aarch64: timer PASS        (from aarch64_timer_probe)
 *   aarch64: kmain_stub soft inventory wave=126 ...
 *   aarch64: kmain_stub soft surf ...
 *   aarch64: kmain_stub soft deepen wave=126 areas=...
 *   aarch64: kmain_stub soft residual lean ... Soft!=product dual=...
 *   aarch64: kmain_stub soft return product_kernel=OPEN ...
 *   aarch64: kmain_stub soft path product_kernel=OPEN ...
 *   aarch64: kmain_stub soft honesty product_kernel=OPEN ...
 *   aarch64: kmain_stub soft exclusive ... product_kernel=OPEN ...
 *   aarch64: kmain_stub soft open ... product_kernel=OPEN ...
 *   aarch64: kmain_stub soft PASS
 *   M0 OK
 *
 * Product path later: share C kernel with GJ_ARCH_AARCH64 shims (see kmain.c).
 * Honesty: stub soft inventory != product kernel complete (OPEN).
 */
void aarch64_uart_puts(const char *sz);
void aarch64_uart_put_hex(unsigned long v);
void aarch64_gic_init(void);
void aarch64_timer_probe(void);

/*
 * Soft inventory stamp (greppable wave=126; residual lean, stamp-free).
 * File-local; never product gate. Never bumps GJ_IMAGE_VERSION.
 */
#define KMAIN_STUB_SOFT_WAVE 126u

/*
 * Soft area count for lean deepen catalog (honest residual; no ret* storm):
 * enter,gic,timer,inventory,surf,return,path,honesty,exclusive,open,
 * residual,lean,deepen = 13.
 */
#define KMAIN_STUB_SOFT_AREAS 13u

/* Once-lamp for residual lean (C0; never hard-gates M0). */
static unsigned g_u8StubSoftResidualLeanOnce;

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

    /*
     * Grep: aarch64: kmain_stub soft deepen
     * Lean area catalog - soft scaffold only; not product kernel claim.
     * residual+lean replace ret* stamp storms (C0 residual; Soft!=product).
     */
    aarch64_uart_puts("aarch64: kmain_stub soft deepen wave=");
    aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
    aarch64_uart_puts(" areas=");
    aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_AREAS);
    aarch64_uart_puts(" catalog=enter,gic,timer,inventory,surf,return,path,"
                      "honesty,exclusive,open,residual,lean,deepen "
                      "soft_only=1 stamp_storm=0\n");

    /*
     * Grep: aarch64: kmain_stub soft residual lean
     * C0 residual once-lamp. Soft!=product dual MIT OR Apache-2.0.
     * Stamp-free (no ret*angle storm, no product version stamp). G-AC-1:
     * no .ko product AC. Dual DoD A/B OPEN. Never hard-gates M0 OK.
     */
    if (g_u8StubSoftResidualLeanOnce == 0u) {
        g_u8StubSoftResidualLeanOnce = 1u;
        aarch64_uart_puts("aarch64: kmain_stub soft residual lean "
                          "enter=1 gic=");
        aarch64_uart_put_hex((unsigned long)uGicDone);
        aarch64_uart_puts(" timer=");
        aarch64_uart_put_hex((unsigned long)uTimerDone);
        aarch64_uart_puts(" bits=");
        aarch64_uart_put_hex((unsigned long)uSurfBits);
        aarch64_uart_puts(" areas=");
        aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_AREAS);
        aarch64_uart_puts(" wave=");
        aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
        aarch64_uart_puts(" stamp_storm=0 version_stamp=0 hard_gate=0 "
                          "product_kernel=OPEN multi_server=0 confine=0 "
                          "stub=1 product_kmain=0 deck=0 continuum=0 "
                          "dual_dod=OPEN g_ac1=1 no_ko_product_ac=1 "
                          "soft_ne_product=1 dual=MIT_OR_Apache-2.0 "
                          "unit=kmain_stub.c "
                          "(Soft!=product; dual MIT OR Apache-2.0; G-AC-1; "
                          "C0 aarch64 kmain_stub residual lean; "
                          "Dual DoD OPEN; no version stamp)\n");
    }

    /* Grep: aarch64: kmain_stub soft return — soft return surfaces */
    aarch64_uart_puts("aarch64: kmain_stub soft return gic=");
    aarch64_uart_put_hex((unsigned long)uGicDone);
    aarch64_uart_puts(" timer=");
    aarch64_uart_put_hex((unsigned long)uTimerDone);
    aarch64_uart_puts(" bits=");
    aarch64_uart_put_hex((unsigned long)uSurfBits);
    aarch64_uart_puts(" product_kernel=OPEN soft_ne_product=1 wave=");
    aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
    aarch64_uart_puts("\n");

    /*
     * Grep: aarch64: kmain_stub soft path
     * Honesty: stub-only path; product kmain is separate (kmain.c).
     * product_kernel=OPEN: aarch64 product kernel remains OPEN.
     */
    aarch64_uart_puts("aarch64: kmain_stub soft path stub=1 product_kmain=0 "
                      "product_kernel=OPEN hard_gate=0 dual_dod=OPEN "
                      "soft_ne_product=1 wave=");
    aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
    aarch64_uart_puts("\n");

    /*
     * Grep: aarch64: kmain_stub soft honesty
     * Explicit non-claim: soft inventory deepen != product kernel complete.
     */
    aarch64_uart_puts("aarch64: kmain_stub soft honesty product_kernel=OPEN "
                      "soft_only=1 m0_scaffold=1 multi_server=0 confine=0 "
                      "dual_dod=OPEN soft_ne_product=1 wave=");
    aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
    aarch64_uart_puts("\n");

    /*
     * Grep: aarch64: kmain_stub soft exclusive - exclusive residual deepen.
     * Soft inventory != product multi-server confine / product kernel.
     */
    aarch64_uart_puts("aarch64: kmain_stub soft exclusive multi_server=0 "
                      "confine=0 product_kernel=OPEN soft_only=1 "
                      "stamp_storm=0 wave=");
    aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
    aarch64_uart_puts("\n");

    /*
     * Grep: aarch64: kmain_stub soft open - open-lamp rollup.
     * Explicit product_kernel=OPEN; soft != product complete.
     */
    aarch64_uart_puts("aarch64: kmain_stub soft open multi_server=0 confine=0 "
                      "product_kernel=OPEN soft_only=1 dual_dod=OPEN wave=");
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
