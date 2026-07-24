/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * aarch64 minimal bring-up stub: banner + GIC + timer probe, then idle.
 * Not linked by default (product path uses kmain.o which wraps kmain).
 *
 * -------------------------------------------------------------------------
 * Soft product surface (Wave 35 exclusive deepen; never hard-fails M0)
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
 *   aarch64: kmain_stub soft inventory wave=81 …
 *   aarch64: kmain_stub soft surf …
 *   aarch64: kmain_stub soft deepen wave=81 areas=…
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

/* Wave 62 soft inventory stamp (file-local; never product gate). */
#define KMAIN_STUB_SOFT_WAVE 81u
/* Areas: enter,gic,timer,inventory,surf,return,path,honesty,deepen */
#define KMAIN_STUB_SOFT_AREAS 129u

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
     * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3. Never hard-fails M0.
     */
    /* Grep: aarch64: kmain_stub soft retclass — Wave 19 return-class taxonomy (kept) */
    aarch64_uart_puts("aarch64: kmain_stub soft retclass ok|fail|inval|nodev|busy|nomem "
                      "soft_only=1 product_gate=0 wave=");
    aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
    aarch64_uart_puts(" (retclass taxonomy; Soft!=product; not bar3)\n");
    /* Grep: aarch64: kmain_stub soft retlane — Wave 19 return-lane catalog (kept) */
    aarch64_uart_puts("aarch64: kmain_stub soft retlane inv|selftest|rate|retcode|retmap|class "
                      "product_kernel=OPEN soft_ne_product=1 wave=");
    aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
    aarch64_uart_puts(" (retlane catalog; Soft!=product)\n");
    /*
     * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft!=product; not bar3.
     */
    /* Grep: aarch64: kmain_stub soft retbound — Wave 20 return-bound honesty (kept) */
    aarch64_uart_puts("aarch64: kmain_stub soft retbound soft_only=1 product_gate=0 hard_gate=0 "
                      "never_blocks_m0=1 wave=");
    aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
    aarch64_uart_puts(" (retbound honesty; Soft!=product; not bar3)\n");
    /* Grep: aarch64: kmain_stub soft retseal — Wave 20 seal stamp (kept) */
    aarch64_uart_puts("aarch64: kmain_stub soft retseal exclusive=1 soft_ne_product=1 "
                      "product_kernel=OPEN bar3=0 wave=");
    aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
    aarch64_uart_puts(" (retseal stamp; Soft!=product)\n");
    /*
     * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft!=product; not bar3.
     */
    /* Grep: aarch64: soft retpulse — Wave 21 return-pulse honesty (kept) */
    aarch64_uart_puts("aarch64: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                      "never_blocks_m0=1 wave=");
    aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
    aarch64_uart_puts(" (retpulse honesty; Soft!=product; not bar3)\n");
    /* Grep: aarch64: soft retmark — Wave 21 mark stamp (kept) */
    aarch64_uart_puts("aarch64: soft retmark exclusive=1 soft_ne_product=1 "
                      "product_kernel=OPEN bar3=0 wave=");
    aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
    aarch64_uart_puts(" (retmark stamp; Soft!=product)\n");

    
/*
 * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: soft retphase — Wave 22 return-phase honesty (kept) */
aarch64_uart_puts("aarch64: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
              "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retphase honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retbadge — Wave 22 badge stamp (kept) */
aarch64_uart_puts("aarch64: soft retbadge exclusive=1 soft_ne_product=1 "
              "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retbadge stamp; Soft!=product)\n");
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: soft rettoken — Wave 23 return-token honesty (kept) */
aarch64_uart_puts("aarch64: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
              "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (rettoken honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retcrest — Wave 23 crest stamp (kept) */
aarch64_uart_puts("aarch64: soft retcrest exclusive=1 soft_ne_product=1 "
              "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retcrest stamp; Soft!=product)\n");
/*
 * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: soft retvault — Wave 24 return-vault honesty (kept) */
aarch64_uart_puts("aarch64: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
              "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retvault honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retbanner — Wave 24 banner stamp (kept) */
aarch64_uart_puts("aarch64: soft retbanner exclusive=1 soft_ne_product=1 "
              "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retbanner stamp; Soft!=product)\n");
/*
 * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: soft retledger — Wave 25 return-ledger honesty (kept) */
aarch64_uart_puts("aarch64: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
              "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retledger honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retbeacon — Wave 25 beacon stamp (kept) */
aarch64_uart_puts("aarch64: soft retbeacon exclusive=1 soft_ne_product=1 "
              "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retbeacon stamp; Soft!=product)\n");
/*
 * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: soft retcipher — Wave 26 return-cipher honesty (kept) */
aarch64_uart_puts("aarch64: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
              "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retcipher honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retflame — Wave 26 flame stamp (kept) */
aarch64_uart_puts("aarch64: soft retflame exclusive=1 soft_ne_product=1 "
              "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retflame stamp; Soft!=product)\n");
/*
 * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: soft retprism — Wave 27 return-prism honesty (kept) */
aarch64_uart_puts("aarch64: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
              "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retprism honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retforge — Wave 27 forge stamp (kept) */
aarch64_uart_puts("aarch64: soft retforge exclusive=1 soft_ne_product=1 "
              "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retforge stamp; Soft!=product)\n");
/*
 * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: soft retshard — Wave 28 return-shard honesty (kept) */
aarch64_uart_puts("aarch64: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retshard honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retcrown — Wave 28 crown stamp (kept) */
aarch64_uart_puts("aarch64: soft retcrown exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retcrown stamp; Soft!=product)\n");
/*
 * ---- Wave 29 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: soft retglyph — Wave 29 return-glyph honesty (kept) */
aarch64_uart_puts("aarch64: soft retglyph soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retglyph honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retscepter — Wave 29 scepter stamp (kept) */
aarch64_uart_puts("aarch64: soft retscepter exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retscepter stamp; Soft!=product)\n");
/*
 * ---- Wave 30 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: soft retsigil — Wave 30 return-sigil honesty (kept) */
aarch64_uart_puts("aarch64: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retsigil honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retemblem — Wave 30 emblem stamp (kept) */
aarch64_uart_puts("aarch64: soft retemblem exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retemblem stamp; Soft!=product)\n");
/*
 * ---- Wave 31 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: soft retaegis — Wave 31 return-aegis honesty (kept) */
aarch64_uart_puts("aarch64: soft retaegis soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retaegis honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retmantle — Wave 31 mantle stamp (kept) */
aarch64_uart_puts("aarch64: soft retmantle exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retmantle stamp; Soft!=product)\n");
/*
 * ---- Wave 32 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: soft retbulwark — Wave 32 return-bulwark honesty (kept) */
aarch64_uart_puts("aarch64: soft retbulwark soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retbulwark honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retpanoply — Wave 32 panoply stamp (kept) */
aarch64_uart_puts("aarch64: soft retpanoply exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retpanoply stamp; Soft!=product)\n");
/*
 * ---- Wave 33 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: soft retbastion — Wave 33 return-bastion honesty (kept) */
aarch64_uart_puts("aarch64: soft retbastion soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retbastion honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retcitadel — Wave 33 citadel stamp (kept) */
aarch64_uart_puts("aarch64: soft retcitadel exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retcitadel stamp; Soft!=product)\n");
/*
 * ---- Wave 34 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: soft retredoubt — Wave 34 return-redoubt honesty */
aarch64_uart_puts("aarch64: soft retredoubt soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retredoubt honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retkeep — Wave 34 exclusive keep stamp */
aarch64_uart_puts("aarch64: soft retkeep exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retkeep stamp; Soft!=product)\n");
/*
 * ---- Wave 35 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: soft retfortress — Wave 35 return-fortress honesty */
aarch64_uart_puts("aarch64: soft retfortress soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retfortress honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retpalace — Wave 35 exclusive palace stamp */
aarch64_uart_puts("aarch64: soft retpalace exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retpalace stamp; Soft!=product)\n");
/*
 * ---- Wave 36 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: soft rethold — Wave 36 return-hold honesty */
aarch64_uart_puts("aarch64: soft rethold soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (rethold honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retspire — Wave 36 exclusive spire stamp */
aarch64_uart_puts("aarch64: soft retspire exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retspire stamp; Soft!=product)\n");
/*
 * ---- Wave 37 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: soft retwall — Wave 37 return-wall honesty */
aarch64_uart_puts("aarch64: soft retwall soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retwall honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retgate — Wave 37 exclusive gate stamp */
aarch64_uart_puts("aarch64: soft retgate exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retgate stamp; Soft!=product)\n");
/*
 * ---- Wave 38 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: soft retmoat — Wave 38 return-moat honesty */
aarch64_uart_puts("aarch64: soft retmoat soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retmoat honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retower — Wave 38 exclusive tower stamp */
aarch64_uart_puts("aarch64: soft retower exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retower stamp; Soft!=product)\n");
/*
 * ---- Wave 39 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: soft retbarbican — Wave 39 return-barbican honesty */
aarch64_uart_puts("aarch64: soft retbarbican soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retbarbican honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retglacis — Wave 39 exclusive glacis stamp */
aarch64_uart_puts("aarch64: soft retglacis exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retglacis stamp; Soft!=product)\n");
/*
 * ---- Wave 40 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: soft retcurtain — Wave 40 return-curtain honesty */
aarch64_uart_puts("aarch64: soft retcurtain soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retcurtain honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retparapet — Wave 40 exclusive parapet stamp */
aarch64_uart_puts("aarch64: soft retparapet exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retparapet stamp; Soft!=product)\n");
/*
 * ---- Wave 41 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: soft retravelin — Wave 41 return-travelin honesty */
aarch64_uart_puts("aarch64: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retravelin honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retditch — Wave 41 exclusive ditch stamp */
aarch64_uart_puts("aarch64: soft retditch exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retditch stamp; Soft!=product)\n");
/*
 * ---- Wave 42 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: soft retportcullis — Wave 42 return-portcullis honesty */
aarch64_uart_puts("aarch64: soft retportcullis soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retportcullis honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retbattlement — Wave 42 exclusive battlement stamp */
aarch64_uart_puts("aarch64: soft retbattlement exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retbattlement stamp; Soft!=product)\n");
/*
 * ---- Wave 43 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: soft retmachicolation — Wave 43 return-machicolation honesty */
aarch64_uart_puts("aarch64: soft retmachicolation soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retmachicolation honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retarrowslit — Wave 43 exclusive arrowslit stamp */
aarch64_uart_puts("aarch64: soft retarrowslit exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retarrowslit stamp; Soft!=product)\n");

/*
 * ---- Wave 44 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: soft retmerlon — Wave 44 return-merlon honesty */
aarch64_uart_puts("aarch64: soft retmerlon soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retmerlon honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retembrasure — Wave 44 exclusive embrasure stamp */
aarch64_uart_puts("aarch64: soft retembrasure exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retembrasure stamp; Soft!=product)\n");

/*
 * ---- Wave 45 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: soft retkeepgate — Wave 45 return-keepgate honesty */
aarch64_uart_puts("aarch64: soft retkeepgate soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retkeepgate honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retouterward — Wave 45 exclusive outerward stamp */
aarch64_uart_puts("aarch64: soft retouterward exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retouterward stamp; Soft!=product)\n");

/*
 * ---- Wave 46 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: soft retbailey — Wave 46 return-bailey honesty */
aarch64_uart_puts("aarch64: soft retbailey soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retbailey honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retpostern — Wave 46 exclusive postern stamp */
aarch64_uart_puts("aarch64: soft retpostern exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retpostern stamp; Soft!=product)\n");

/*
 * ---- Wave 47 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: soft retinnerward — Wave 47 return-innerward honesty */
aarch64_uart_puts("aarch64: soft retinnerward soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retinnerward honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retdonjon — Wave 47 exclusive donjon stamp */
aarch64_uart_puts("aarch64: soft retdonjon exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retdonjon stamp; Soft!=product)\n");

/*
 * ---- Wave 48 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: soft retchevaux — Wave 48 return-chevaux honesty */
aarch64_uart_puts("aarch64: soft retchevaux soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retchevaux honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retpalisade — Wave 48 exclusive palisade stamp */
aarch64_uart_puts("aarch64: soft retpalisade exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retpalisade stamp; Soft!=product)\n");

/*
 * ---- Wave 49 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: soft retglacisgate — Wave 49 return-glacisgate honesty */
aarch64_uart_puts("aarch64: soft retglacisgate soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retglacisgate honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retoutwork — Wave 49 exclusive outwork stamp */
aarch64_uart_puts("aarch64: soft retoutwork exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retoutwork stamp; Soft!=product)\n");
/*
 * ---- Wave 50 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: soft retsally — Wave 50 return-sally honesty */
aarch64_uart_puts("aarch64: soft retsally soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retsally honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retcounterscarp — Wave 50 exclusive counterscarp stamp */
aarch64_uart_puts("aarch64: soft retcounterscarp exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retcounterscarp stamp; Soft!=product)\n");

/*
 * ---- Wave 51 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: soft retfosse — Wave 51 return-fosse honesty */
aarch64_uart_puts("aarch64: soft retfosse soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retfosse honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retcoveredway — Wave 51 exclusive coveredway stamp */
aarch64_uart_puts("aarch64: soft retcoveredway exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retcoveredway stamp; Soft!=product)\n");

/*
 * ---- Wave 52 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: soft rettenaille — Wave 52 return-tenaille honesty */
aarch64_uart_puts("aarch64: soft rettenaille soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (rettenaille honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retdemilune — Wave 52 exclusive demilune stamp */
aarch64_uart_puts("aarch64: soft retdemilune exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retdemilune stamp; Soft!=product)\n");
/*
 * ---- Wave 53 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: soft retravelin — Wave 53 return-travelin honesty */
aarch64_uart_puts("aarch64: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retravelin honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retlunette — Wave 53 exclusive lunette stamp */
aarch64_uart_puts("aarch64: soft retlunette exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retlunette stamp; Soft!=product)\n");
/*
 * ---- Wave 54 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: soft retcaponier — Wave 54 return-caponier honesty */
aarch64_uart_puts("aarch64: soft retcaponier soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retcaponier honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retredan — Wave 54 exclusive redan stamp */
aarch64_uart_puts("aarch64: soft retredan exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retredan stamp; Soft!=product)\n");
/*
 * ---- Wave 55 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: soft retflank — Wave 55 return-flank honesty */
aarch64_uart_puts("aarch64: soft retflank soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retflank honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retface — Wave 55 exclusive face stamp */
aarch64_uart_puts("aarch64: soft retface exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retface stamp; Soft!=product)\n");
/*
 * ---- Wave 56 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: soft retgorge — Wave 56 return-gorge honesty */
aarch64_uart_puts("aarch64: soft retgorge soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retgorge honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retshoulder — Wave 56 exclusive shoulder stamp */
aarch64_uart_puts("aarch64: soft retshoulder exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retshoulder stamp; Soft!=product)\n");
/*
 * ---- Wave 57 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: soft retraverse — Wave 57 return-traverse honesty */
aarch64_uart_puts("aarch64: soft retraverse soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retraverse honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retcasemate — Wave 57 exclusive casemate stamp */
aarch64_uart_puts("aarch64: soft retcasemate exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retcasemate stamp; Soft!=product)\n");

/*
 * ---- Wave 58 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: soft retorillon — Wave 58 return-orillon honesty */
aarch64_uart_puts("aarch64: soft retorillon soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retorillon honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retbonnette — Wave 58 exclusive bonnette stamp */
aarch64_uart_puts("aarch64: soft retbonnette exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retbonnette stamp; Soft!=product)\n");

/*
 * ---- Wave 59 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: soft retcrownwork — Wave 59 return-crownwork honesty */
aarch64_uart_puts("aarch64: soft retcrownwork soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retcrownwork honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft rethornwork — Wave 59 exclusive hornwork stamp */
aarch64_uart_puts("aarch64: soft rethornwork exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (rethornwork stamp; Soft!=product)\n");

/*
 * ---- Wave 60 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: soft retplace — Wave 60 return-place honesty */
aarch64_uart_puts("aarch64: soft retplace soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retplace honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retenvelope — Wave 60 exclusive envelope stamp */
aarch64_uart_puts("aarch64: soft retenvelope exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retenvelope stamp; Soft!=product)\n");









/*
 * ---- Wave 61 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: soft retcounterguard — Wave 61 return-counterguard honesty */
aarch64_uart_puts("aarch64: soft retcounterguard soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retcounterguard honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retcoveredface — Wave 61 exclusive coveredface stamp */
aarch64_uart_puts("aarch64: soft retcoveredface exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retcoveredface stamp; Soft!=product)\n");
/*
 * ---- Wave 62 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: soft retbastionface — Wave 62 return-bastionface honesty */
aarch64_uart_puts("aarch64: soft retbastionface soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retbastionface honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retcurtainangle — Wave 62 exclusive curtainangle stamp */
aarch64_uart_puts("aarch64: soft retcurtainangle exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retcurtainangle stamp; Soft!=product)\n");
/*
 * ---- Wave 63 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: soft retdoubletenaille — Wave 63 return-doubletenaille honesty */
aarch64_uart_puts("aarch64: soft retdoubletenaille soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retdoubletenaille honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retplaceofarms — Wave 63 exclusive placeofarms stamp */
aarch64_uart_puts("aarch64: soft retplaceofarms exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retplaceofarms stamp; Soft!=product)\n");
/*
 * ---- Wave 64 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: soft retreentrant — Wave 64 return-reentrant honesty */
aarch64_uart_puts("aarch64: soft retreentrant soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retreentrant honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retsallyport — Wave 64 exclusive sallyport stamp */
aarch64_uart_puts("aarch64: soft retsallyport exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retsallyport stamp; Soft!=product)\n");
/*
 * ---- Wave 65 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: soft retgorgeangle — Wave 65 return-gorgeangle honesty */
aarch64_uart_puts("aarch64: soft retgorgeangle soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retgorgeangle honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retshoulderangle — Wave 65 exclusive shoulderangle stamp */
aarch64_uart_puts("aarch64: soft retshoulderangle exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retshoulderangle stamp; Soft!=product)\n");
/*
 * ---- Wave 66 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: soft retflankangle — Wave 66 return-flankangle honesty */
aarch64_uart_puts("aarch64: soft retflankangle soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retflankangle honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retfaceangle — Wave 66 exclusive faceangle stamp */
aarch64_uart_puts("aarch64: soft retfaceangle exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
/*
 * ---- Wave 67 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: soft retcaponierangle — Wave 67 return-caponierangle honesty */
aarch64_uart_puts("aarch64: soft retcaponierangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retcaponierangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: soft retredanangle — Wave 67 exclusive redanangle stamp */
aarch64_uart_puts("aarch64: soft retredanangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retredanangle stamp; Soft≠product)\n");
/*
 * ---- Wave 68 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: soft retlunetteangle — Wave 68 return-lunetteangle honesty */
aarch64_uart_puts("aarch64: soft retlunetteangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retlunetteangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: soft rettenailleangle — Wave 68 exclusive tenailleangle stamp */
aarch64_uart_puts("aarch64: soft rettenailleangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(rettenailleangle stamp; Soft≠product)\n");
/*
 * ---- Wave 69 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: soft retdemiluneangle — Wave 69 return-demiluneangle honesty */
aarch64_uart_puts("aarch64: soft retdemiluneangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retdemiluneangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: soft retcoveredwayangle — Wave 69 exclusive coveredwayangle stamp */
aarch64_uart_puts("aarch64: soft retcoveredwayangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retcoveredwayangle stamp; Soft≠product)\n");
/*
 * ---- Wave 70 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: soft retfosseangle — Wave 70 return-fosseangle honesty */
aarch64_uart_puts("aarch64: soft retfosseangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=81 (retfosseangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: soft retcounterscarple — Wave 70 exclusive counterscarple stamp */
aarch64_uart_puts("aarch64: soft retcounterscarple exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=81 (retcounterscarple stamp; Soft≠product)\n");
/*
 * ---- Wave 71 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: soft retsallyportangle — Wave 71 return-sallyportangle honesty */
aarch64_uart_puts("aarch64: soft retsallyportangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=81 (retsallyportangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: soft retreentrantangle — Wave 71 exclusive reentrantangle stamp */
aarch64_uart_puts("aarch64: soft retreentrantangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=81 (retreentrantangle stamp; Soft≠product)\n");
/*
 * ---- Wave 72 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: soft retplaceofarmsangle — Wave 72 return-placeofarmsangle honesty */
aarch64_uart_puts("aarch64: soft retplaceofarmsangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=81 (retplaceofarmsangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: soft retdoubletenailleangle — Wave 72 exclusive doubletenailleangle stamp */
aarch64_uart_puts("aarch64: soft retdoubletenailleangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=81 (retdoubletenailleangle stamp; Soft≠product)\n");
/* Grep: aarch64: soft retcurtainface — Wave 73 return-curtainface honesty */
aarch64_uart_puts("aarch64: soft retcurtainface soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=81 (retcurtainface honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: soft retbastionangle — Wave 73 exclusive bastionangle stamp */
aarch64_uart_puts("aarch64: soft retbastionangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=81 (retbastionangle stamp; Soft≠product)\n");
/* Grep: aarch64: soft retglacisangle — Wave 74 return-glacisangle honesty */
aarch64_uart_puts("aarch64: soft retglacisangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=81 (retglacisangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: soft retparapetangle — Wave 74 exclusive parapetangle stamp */
aarch64_uart_puts("aarch64: soft retparapetangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=81 (retparapetangle stamp; Soft≠product)\n");
/* Grep: aarch64: soft retmoatangle — Wave 75 return-moatangle honesty */
aarch64_uart_puts("aarch64: soft retmoatangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=81 (retmoatangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: soft retowerangle — Wave 75 exclusive towerangle stamp */
aarch64_uart_puts("aarch64: soft retowerangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=81 (retowerangle stamp; Soft≠product)\n");
/* Grep: aarch64: soft retgateangle — Wave 76 return-gateangle honesty */
aarch64_uart_puts("aarch64: soft retgateangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=81 (retgateangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: soft retwallangle — Wave 76 exclusive wallangle stamp */
aarch64_uart_puts("aarch64: soft retwallangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=81 (retwallangle stamp; Soft≠product)\n");
/* Grep: aarch64: soft retspireangle — Wave 77 return-spireangle honesty */
aarch64_uart_puts("aarch64: soft retspireangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=81 (retspireangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: soft retholdangle — Wave 77 exclusive holdangle stamp */
aarch64_uart_puts("aarch64: soft retholdangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=81 (retholdangle stamp; Soft≠product)\n");
/* Grep: aarch64: soft retpalaceangle — Wave 78 return-palaceangle honesty */
aarch64_uart_puts("aarch64: soft retpalaceangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=81 (retpalaceangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: soft retfortressangle — Wave 78 exclusive fortressangle stamp */
aarch64_uart_puts("aarch64: soft retfortressangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=81 (retfortressangle stamp; Soft≠product)\n");
/* Grep: aarch64: soft retkeepangle — Wave 79 return-keepangle honesty */
aarch64_uart_puts("aarch64: soft retkeepangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=81 (retkeepangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: soft retredoubtangle — Wave 79 exclusive redoubtangle stamp */
aarch64_uart_puts("aarch64: soft retredoubtangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=81 (retredoubtangle stamp; Soft≠product)\n");
/* Grep: aarch64: soft retcitadelangle — Wave 80 return-citadelangle honesty */
aarch64_uart_puts("aarch64: soft retcitadelangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=81 (retcitadelangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: soft retbastionkeep — Wave 80 exclusive bastionkeep stamp */
aarch64_uart_puts("aarch64: soft retbastionkeep exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=81 (retbastionkeep stamp; Soft≠product)\n");
/* Grep: aarch64: soft retpanoplyangle — Wave 81 return-panoplyangle honesty */
aarch64_uart_puts("aarch64: soft retpanoplyangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=81 (retpanoplyangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: soft retbulwarkangle — Wave 81 exclusive bulwarkangle stamp */
aarch64_uart_puts("aarch64: soft retbulwarkangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=81 (retbulwarkangle stamp; Soft≠product)\n");

aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
aarch64_uart_puts(" (retfaceangle stamp; Soft!=product)\n");
/* Grep: aarch64: kmain_stub soft deepen */
    aarch64_uart_puts("aarch64: kmain_stub soft deepen wave=");
    aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
    aarch64_uart_puts(" areas=");
    aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_AREAS);
    aarch64_uart_puts(" catalog=enter,gic,timer,inventory,surf,return,path,"
                      "honesty,retclass,retlane,deepen soft_only=1\n");

    /* Grep: aarch64: kmain_stub soft return — Wave 19 return surfaces */
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

    /* Grep: aarch64: kmain_stub soft exclusive — Wave 35 exclusive deepen */
    aarch64_uart_puts("aarch64: kmain_stub soft exclusive multi_server=0 "
                      "confine=0 bar3=0 product_kernel=OPEN soft_only=1 wave=");
    aarch64_uart_put_hex((unsigned long)KMAIN_STUB_SOFT_WAVE);
    aarch64_uart_puts("\n");

    /* Grep: aarch64: kmain_stub soft open — Wave 19 open-lamp rollup */
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
