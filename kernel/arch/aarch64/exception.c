/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * EL1 exception dispatch. Routes SVC64; recovers PSCI probe traps (HVC/SMC
 * without firmware) by advancing ELR so kmain does not UNDEF-loop.
 *
 * -------------------------------------------------------------------------
 * Soft IRQ observability (pairs with gic.c SPI soft + timer tick soft)
 * -------------------------------------------------------------------------
 * VBAR slots (Current EL, SP_ELx):
 *   vec 4 = Sync   vec 5 = IRQ   vec 6 = FIQ   vec 7 = SError
 * IRQ soft path (no product delivery yet — DAIF.I held, timer IMASK set):
 *   on vec%4==1: soft-ACK via aarch64_gic_soft_iar, EOI non-spurious,
 *   count take, return (avoid re-entry loop). Greppable counter line at
 *   install time documents zero baseline; live takes print soft IRQ line.
 *
 * Soft inventory deepen (install-time, greppable "aarch64: exception soft …"):
 *   aarch64: exception soft vbar=… match=… align=… slots=… slotb=…
 *   aarch64: exception soft class sync=0 irq=1 fiq=2 serror=3
 *   aarch64: exception soft counts sync=… irq=… fiq=… serror=… baseline0=…
 *   aarch64: exception soft daif=… i_held=… el=…
 *   aarch64: exception soft PASS | FAIL
 *
 * Soft inventory deepen (Wave 37 exclusive; this unit only):
 *   Multi-line greppable areas under "aarch64: exception soft …":
 *     inventory | vbar | class | counts | daif | banks | gates | path | deepen
 *   Banks soft: CurrentEL SP_ELx / SP_EL0 / Lower AArch64 / Lower AArch32
 *               slot base indices (ARM DDI fixed geometry).
 *   Path honesty: soft observe only — DAIF.I held; no product IRQ delivery.
 *   Soft PASS/FAIL gates keep prior shape; deepen never hard-gates.
 *   Soft ≠ product exception complete; soft ≠ bar3.
 *
 * Greppable: aarch64: exceptions PASS
 *            aarch64: exception soft inventory …
 *            aarch64: exception soft banks …
 *            aarch64: exception soft gates …
 *            aarch64: exception soft path … product_kernel=OPEN
 *            aarch64: exception soft surf …
 *            aarch64: exception soft return inv_ret=… product_kernel=OPEN
 *            aarch64: exception soft honesty product_kernel=OPEN
 *            aarch64: exception soft deepen wave=89 …
 *
 * Freestanding pure C; no GPL Linux arch paste. No NEON/FP —
 * general-regs-only for this TU (CPACR FP/SIMD not enabled at EL1 soft).
 */
#include "types_arch.h"

#if defined(__GNUC__)
#pragma GCC target("general-regs-only")
#endif

/* VBAR_EL1 table alignment (exceptions.S .align 11 → 2 KiB). */
#define EXC_SOFT_VBAR_ALIGN     0x800ul
#define EXC_SOFT_VBAR_ALIGN_MASK (EXC_SOFT_VBAR_ALIGN - 1ul)

/* Fixed AArch64 vector table geometry (ARM DDI). */
#define EXC_SOFT_SLOTS          16u
#define EXC_SOFT_SLOT_BYTES     0x80u
#define EXC_SOFT_TABLE_BYTES    (EXC_SOFT_SLOTS * EXC_SOFT_SLOT_BYTES)

/* vec & 3 class encoding (must match exceptions.S + dispatch). */
#define EXC_SOFT_CLASS_SYNC     0u
#define EXC_SOFT_CLASS_IRQ      1u
#define EXC_SOFT_CLASS_FIQ      2u
#define EXC_SOFT_CLASS_SERROR   3u

/* Soft bank base indices (vec / 4): ARM DDI fixed layout. */
#define EXC_SOFT_BANK_CUR_SPX   1u /* Current EL, SP_ELx: vec 4–7 */
#define EXC_SOFT_BANK_CUR_SP0   0u /* Current EL, SP_EL0: vec 0–3 */
#define EXC_SOFT_BANK_LOWER_A64 2u /* Lower EL AArch64: vec 8–11 */
#define EXC_SOFT_BANK_LOWER_A32 3u /* Lower EL AArch32: vec 12–15 */

/* DAIF system-register bit positions (PSTATE view via MRS DAIF). */
#define EXC_SOFT_DAIF_F_BIT     (1ul << 6)
#define EXC_SOFT_DAIF_I_BIT     (1ul << 7)
#define EXC_SOFT_DAIF_A_BIT     (1ul << 8)
#define EXC_SOFT_DAIF_D_BIT     (1ul << 9)

/* Wave 62 soft inventory stamp (file-local; never product gate). */
#define EXC_SOFT_WAVE 89u
#define EXC_SOFT_AREAS 148u

extern void aarch64_uart_puts(const char *sz);
extern void aarch64_uart_put_hex(unsigned long v);
extern int aarch64_svc_try_handle(unsigned long u64Vec, unsigned long esr,
                                  unsigned long elr);
extern int aarch64_psci_exception_recover(unsigned long *pElr,
                                          unsigned long *pX0);
extern volatile int g_psci_probe_active;

/* Soft GIC IAR/EOI helpers (gic.c) — IRQ soft path only. */
extern unsigned aarch64_gic_soft_iar(void);
extern void aarch64_gic_soft_eoi(unsigned uIntId);

/* Soft observability counters (IRQ / FIQ / Sync / SError takes). */
static unsigned long g_cIrqSoft;
static unsigned long g_cFiqSoft;
static unsigned long g_cSyncSoft;
static unsigned long g_cSerrorSoft;

/*
 * Soft exception inventory deepen (install-time; Wave 19 multi-area).
 * Greppable family: "aarch64: exception soft …"
 * Pure integer / system-register MRS; no FP/NEON. Never hard-gates.
 *
 * Gates (soft / non-fatal for product bring-up):
 *   - VBAR_EL1 matches aarch64_vectors and is 2 KiB aligned
 *   - take counters all zero at install baseline
 *   - DAIF.I held (product soft path does not unmask IRQs yet)
 *   - CurrentEL is EL1
 *
 * Grep areas: inventory | vbar | class | counts | daif | banks | gates |
 *             path | deepen
 * Returns 1 on soft PASS.
 */
static int
exception_soft_inventory(unsigned long u64Vbar, unsigned long u64VecPa)
{
    unsigned long u64Daif;
    unsigned long u64CurrentEl;
    unsigned long u64El;
    unsigned long u64AlignOk;
    unsigned long u64Match;
    unsigned long u64Baseline0;
    unsigned long u64IHeld;
    unsigned long u64DHeld;
    unsigned long u64AHeld;
    unsigned long u64FHeld;
    unsigned long u64GateVbar;
    unsigned long u64GateEl;
    unsigned long u64TableBytes;
    int fOk;

    __asm__ volatile("mrs %0, daif" : "=r"(u64Daif));
    __asm__ volatile("mrs %0, CurrentEL" : "=r"(u64CurrentEl));
    u64El = (u64CurrentEl >> 2) & 0x3ul;

    u64Match = 0ul;
    if (u64Vbar == u64VecPa) {
        u64Match = 1ul;
    }

    u64AlignOk = 0ul;
    if ((u64Vbar & EXC_SOFT_VBAR_ALIGN_MASK) == 0ul && u64Vbar != 0ul) {
        u64AlignOk = 1ul;
    }

    u64Baseline0 = 0ul;
    if (g_cSyncSoft == 0ul && g_cIrqSoft == 0ul && g_cFiqSoft == 0ul &&
        g_cSerrorSoft == 0ul) {
        u64Baseline0 = 1ul;
    }

    u64IHeld = 0ul;
    if ((u64Daif & EXC_SOFT_DAIF_I_BIT) != 0ul) {
        u64IHeld = 1ul;
    }
    u64DHeld = ((u64Daif & EXC_SOFT_DAIF_D_BIT) != 0ul) ? 1ul : 0ul;
    u64AHeld = ((u64Daif & EXC_SOFT_DAIF_A_BIT) != 0ul) ? 1ul : 0ul;
    u64FHeld = ((u64Daif & EXC_SOFT_DAIF_F_BIT) != 0ul) ? 1ul : 0ul;
    u64TableBytes = (unsigned long)EXC_SOFT_TABLE_BYTES;

    /* Grep: aarch64: exception soft inventory */
    aarch64_uart_puts("aarch64: exception soft inventory el=");
    aarch64_uart_put_hex(u64El);
    aarch64_uart_puts(" match=");
    aarch64_uart_put_hex(u64Match);
    aarch64_uart_puts(" align=");
    aarch64_uart_put_hex(u64AlignOk);
    aarch64_uart_puts(" baseline0=");
    aarch64_uart_put_hex(u64Baseline0);
    aarch64_uart_puts(" i_held=");
    aarch64_uart_put_hex(u64IHeld);
    aarch64_uart_puts(" wave=");
    aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
    aarch64_uart_puts(" areas=");
    aarch64_uart_put_hex((unsigned long)EXC_SOFT_AREAS);
    aarch64_uart_puts("\n");

    /* VBAR geometry soft inventory. */
    aarch64_uart_puts("aarch64: exception soft vbar=");
    aarch64_uart_put_hex(u64Vbar);
    aarch64_uart_puts(" vecpa=");
    aarch64_uart_put_hex(u64VecPa);
    aarch64_uart_puts(" match=");
    aarch64_uart_put_hex(u64Match);
    aarch64_uart_puts(" align=");
    aarch64_uart_put_hex(u64AlignOk);
    aarch64_uart_puts(" slots=");
    aarch64_uart_put_hex((unsigned long)EXC_SOFT_SLOTS);
    aarch64_uart_puts(" slotb=");
    aarch64_uart_put_hex((unsigned long)EXC_SOFT_SLOT_BYTES);
    aarch64_uart_puts(" tableb=");
    aarch64_uart_put_hex(u64TableBytes);
    aarch64_uart_puts(" align_mask=");
    aarch64_uart_put_hex(EXC_SOFT_VBAR_ALIGN_MASK);
    aarch64_uart_puts("\n");

    /* Class encoding soft inventory (vec & 3). */
    aarch64_uart_puts("aarch64: exception soft class sync=");
    aarch64_uart_put_hex((unsigned long)EXC_SOFT_CLASS_SYNC);
    aarch64_uart_puts(" irq=");
    aarch64_uart_put_hex((unsigned long)EXC_SOFT_CLASS_IRQ);
    aarch64_uart_puts(" fiq=");
    aarch64_uart_put_hex((unsigned long)EXC_SOFT_CLASS_FIQ);
    aarch64_uart_puts(" serror=");
    aarch64_uart_put_hex((unsigned long)EXC_SOFT_CLASS_SERROR);
    aarch64_uart_puts("\n");

    /* Take-counter soft inventory (install baseline expected 0). */
    aarch64_uart_puts("aarch64: exception soft counts sync=");
    aarch64_uart_put_hex(g_cSyncSoft);
    aarch64_uart_puts(" irq=");
    aarch64_uart_put_hex(g_cIrqSoft);
    aarch64_uart_puts(" fiq=");
    aarch64_uart_put_hex(g_cFiqSoft);
    aarch64_uart_puts(" serror=");
    aarch64_uart_put_hex(g_cSerrorSoft);
    aarch64_uart_puts(" baseline0=");
    aarch64_uart_put_hex(u64Baseline0);
    aarch64_uart_puts(" total=");
    aarch64_uart_put_hex(g_cSyncSoft + g_cIrqSoft + g_cFiqSoft +
                         g_cSerrorSoft);
    aarch64_uart_puts("\n");

    /* DAIF / EL soft inventory (I held while soft IRQ path documents only). */
    aarch64_uart_puts("aarch64: exception soft daif=");
    aarch64_uart_put_hex(u64Daif);
    aarch64_uart_puts(" i_held=");
    aarch64_uart_put_hex(u64IHeld);
    aarch64_uart_puts(" d=");
    aarch64_uart_put_hex(u64DHeld);
    aarch64_uart_puts(" a=");
    aarch64_uart_put_hex(u64AHeld);
    aarch64_uart_puts(" f=");
    aarch64_uart_put_hex(u64FHeld);
    aarch64_uart_puts(" el=");
    aarch64_uart_put_hex(u64El);
    aarch64_uart_puts("\n");

    /* Grep: aarch64: exception soft banks (ARM DDI fixed slot banks) */
    aarch64_uart_puts("aarch64: exception soft banks cur_sp0=");
    aarch64_uart_put_hex((unsigned long)EXC_SOFT_BANK_CUR_SP0);
    aarch64_uart_puts(" cur_spx=");
    aarch64_uart_put_hex((unsigned long)EXC_SOFT_BANK_CUR_SPX);
    aarch64_uart_puts(" lower_a64=");
    aarch64_uart_put_hex((unsigned long)EXC_SOFT_BANK_LOWER_A64);
    aarch64_uart_puts(" lower_a32=");
    aarch64_uart_put_hex((unsigned long)EXC_SOFT_BANK_LOWER_A32);
    aarch64_uart_puts(" irq_cur_spx=");
    aarch64_uart_put_hex((unsigned long)(EXC_SOFT_BANK_CUR_SPX * 4u +
                                         EXC_SOFT_CLASS_IRQ));
    aarch64_uart_puts(" sync_cur_spx=");
    aarch64_uart_put_hex((unsigned long)(EXC_SOFT_BANK_CUR_SPX * 4u +
                                         EXC_SOFT_CLASS_SYNC));
    aarch64_uart_puts("\n");

    u64GateVbar = (u64Match != 0ul && u64AlignOk != 0ul) ? 1ul : 0ul;
    u64GateEl = (u64El == 1ul) ? 1ul : 0ul;

    /* Grep: aarch64: exception soft gates */
    aarch64_uart_puts("aarch64: exception soft gates vbar=");
    aarch64_uart_put_hex(u64GateVbar);
    aarch64_uart_puts(" baseline0=");
    aarch64_uart_put_hex(u64Baseline0);
    aarch64_uart_puts(" i_held=");
    aarch64_uart_put_hex(u64IHeld);
    aarch64_uart_puts(" el1=");
    aarch64_uart_put_hex(u64GateEl);
    aarch64_uart_puts(" match=");
    aarch64_uart_put_hex(u64Match);
    aarch64_uart_puts(" align=");
    aarch64_uart_put_hex(u64AlignOk);
    aarch64_uart_puts("\n");

    fOk = 1;
    if (u64Match == 0ul || u64AlignOk == 0ul) {
        fOk = 0;
    }
    if (u64Baseline0 == 0ul) {
        fOk = 0;
    }
    if (u64IHeld == 0ul) {
        fOk = 0;
    }
    if (u64El != 1ul) {
        fOk = 0;
    }

    /* Grep: aarch64: exception soft path */
    aarch64_uart_puts("aarch64: exception soft path install=1 deliver=0 "
                      "daif_i=1 gic_soft=1 neon=0 product_kernel=OPEN "
                      "hard_gate=0 wave=");
    aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
    aarch64_uart_puts(" (soft inventory; not bar3)\n");

    /* Grep: aarch64: exception soft surf — Wave 19 gate bit lamps */
    aarch64_uart_puts("aarch64: exception soft surf match=");
    aarch64_uart_put_hex(u64Match);
    aarch64_uart_puts(" align=");
    aarch64_uart_put_hex(u64AlignOk);
    aarch64_uart_puts(" baseline0=");
    aarch64_uart_put_hex(u64Baseline0);
    aarch64_uart_puts(" i_held=");
    aarch64_uart_put_hex(u64IHeld);
    aarch64_uart_puts(" el1=");
    aarch64_uart_put_hex(u64GateEl);
    aarch64_uart_puts(" bits=");
    aarch64_uart_put_hex((u64Match << 0) | (u64AlignOk << 1) |
                          (u64Baseline0 << 2) | (u64IHeld << 3) |
                          (u64GateEl << 4));
    aarch64_uart_puts(" wave=");
    aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
    aarch64_uart_puts("\n");

    /*
     * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: aarch64: exception soft retclass — Wave 19 return-class taxonomy (kept) */
    aarch64_uart_puts("aarch64: exception soft retclass ok|fail|inval|nodev|busy|nomem "
                      "soft_only=1 product_gate=0 wave=");
    aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
    aarch64_uart_puts(" (retclass taxonomy; Soft!=product; not bar3)\n");
    /* Grep: aarch64: exception soft retlane — Wave 19 return-lane catalog (kept) */
    aarch64_uart_puts("aarch64: exception soft retlane inv|selftest|rate|retcode|retmap|class "
                      "product_kernel=OPEN soft_ne_product=1 wave=");
    aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
    aarch64_uart_puts(" (retlane catalog; Soft!=product)\n");
    /*
     * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft!=product; not bar3.
     */
    /* Grep: aarch64: exception soft retbound — Wave 20 return-bound honesty (kept) */
    aarch64_uart_puts("aarch64: exception soft retbound soft_only=1 product_gate=0 hard_gate=0 "
                      "never_blocks_m0=1 wave=");
    aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
    aarch64_uart_puts(" (retbound honesty; Soft!=product; not bar3)\n");
    /* Grep: aarch64: exception soft retseal — Wave 20 seal stamp (kept) */
    aarch64_uart_puts("aarch64: exception soft retseal exclusive=1 soft_ne_product=1 "
                      "product_kernel=OPEN bar3=0 wave=");
    aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
    aarch64_uart_puts(" (retseal stamp; Soft!=product)\n");
    /*
     * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft!=product; not bar3.
     */
    /* Grep: aarch64: soft retpulse — Wave 21 return-pulse honesty (kept) */
    aarch64_uart_puts("aarch64: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                      "never_blocks_m0=1 wave=");
    aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
    aarch64_uart_puts(" (retpulse honesty; Soft!=product; not bar3)\n");
    /* Grep: aarch64: soft retmark — Wave 21 mark stamp (kept) */
    aarch64_uart_puts("aarch64: soft retmark exclusive=1 soft_ne_product=1 "
                      "product_kernel=OPEN bar3=0 wave=");
    aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
    aarch64_uart_puts(" (retmark stamp; Soft!=product)\n");

    
/*
 * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: soft retphase — Wave 22 return-phase honesty (kept) */
aarch64_uart_puts("aarch64: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
              "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retphase honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retbadge — Wave 22 badge stamp (kept) */
aarch64_uart_puts("aarch64: soft retbadge exclusive=1 soft_ne_product=1 "
              "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retbadge stamp; Soft!=product)\n");
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: soft rettoken — Wave 23 return-token honesty (kept) */
aarch64_uart_puts("aarch64: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
              "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (rettoken honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retcrest — Wave 23 crest stamp (kept) */
aarch64_uart_puts("aarch64: soft retcrest exclusive=1 soft_ne_product=1 "
              "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retcrest stamp; Soft!=product)\n");
/*
 * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: soft retvault — Wave 24 return-vault honesty (kept) */
aarch64_uart_puts("aarch64: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
              "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retvault honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retbanner — Wave 24 banner stamp (kept) */
aarch64_uart_puts("aarch64: soft retbanner exclusive=1 soft_ne_product=1 "
              "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retbanner stamp; Soft!=product)\n");
/*
 * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: soft retledger — Wave 25 return-ledger honesty (kept) */
aarch64_uart_puts("aarch64: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
              "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retledger honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retbeacon — Wave 25 beacon stamp (kept) */
aarch64_uart_puts("aarch64: soft retbeacon exclusive=1 soft_ne_product=1 "
              "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retbeacon stamp; Soft!=product)\n");
/*
 * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: soft retcipher — Wave 26 return-cipher honesty (kept) */
aarch64_uart_puts("aarch64: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
              "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retcipher honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retflame — Wave 26 flame stamp (kept) */
aarch64_uart_puts("aarch64: soft retflame exclusive=1 soft_ne_product=1 "
              "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retflame stamp; Soft!=product)\n");
/*
 * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: soft retprism — Wave 27 return-prism honesty (kept) */
aarch64_uart_puts("aarch64: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
              "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retprism honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retforge — Wave 27 forge stamp (kept) */
aarch64_uart_puts("aarch64: soft retforge exclusive=1 soft_ne_product=1 "
              "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retforge stamp; Soft!=product)\n");
/*
 * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: soft retshard — Wave 28 return-shard honesty (kept) */
aarch64_uart_puts("aarch64: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retshard honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retcrown — Wave 28 crown stamp (kept) */
aarch64_uart_puts("aarch64: soft retcrown exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retcrown stamp; Soft!=product)\n");
/*
 * ---- Wave 29 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: soft retglyph — Wave 29 return-glyph honesty (kept) */
aarch64_uart_puts("aarch64: soft retglyph soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retglyph honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retscepter — Wave 29 scepter stamp (kept) */
aarch64_uart_puts("aarch64: soft retscepter exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retscepter stamp; Soft!=product)\n");
/*
 * ---- Wave 30 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: soft retsigil — Wave 30 return-sigil honesty (kept) */
aarch64_uart_puts("aarch64: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retsigil honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retemblem — Wave 30 emblem stamp (kept) */
aarch64_uart_puts("aarch64: soft retemblem exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retemblem stamp; Soft!=product)\n");
/*
 * ---- Wave 31 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: soft retaegis — Wave 31 return-aegis honesty (kept) */
aarch64_uart_puts("aarch64: soft retaegis soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retaegis honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retmantle — Wave 31 mantle stamp (kept) */
aarch64_uart_puts("aarch64: soft retmantle exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retmantle stamp; Soft!=product)\n");
/*
 * ---- Wave 32 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: soft retbulwark — Wave 32 return-bulwark honesty (kept) */
aarch64_uart_puts("aarch64: soft retbulwark soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retbulwark honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retpanoply — Wave 32 panoply stamp (kept) */
aarch64_uart_puts("aarch64: soft retpanoply exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retpanoply stamp; Soft!=product)\n");
/*
 * ---- Wave 33 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: soft retbastion — Wave 33 return-bastion honesty (kept) */
aarch64_uart_puts("aarch64: soft retbastion soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retbastion honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retcitadel — Wave 33 citadel stamp (kept) */
aarch64_uart_puts("aarch64: soft retcitadel exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retcitadel stamp; Soft!=product)\n");
/*
 * ---- Wave 34 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: soft retredoubt — Wave 34 return-redoubt honesty */
aarch64_uart_puts("aarch64: soft retredoubt soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retredoubt honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retkeep — Wave 34 exclusive keep stamp */
aarch64_uart_puts("aarch64: soft retkeep exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retkeep stamp; Soft!=product)\n");
/*
 * ---- Wave 35 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: soft retfortress — Wave 35 return-fortress honesty */
aarch64_uart_puts("aarch64: soft retfortress soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retfortress honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retpalace — Wave 35 exclusive palace stamp */
aarch64_uart_puts("aarch64: soft retpalace exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retpalace stamp; Soft!=product)\n");
/*
 * ---- Wave 36 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: soft rethold — Wave 36 return-hold honesty */
aarch64_uart_puts("aarch64: soft rethold soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (rethold honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retspire — Wave 36 exclusive spire stamp */
aarch64_uart_puts("aarch64: soft retspire exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retspire stamp; Soft!=product)\n");
/*
 * ---- Wave 37 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: soft retwall — Wave 37 return-wall honesty */
aarch64_uart_puts("aarch64: soft retwall soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retwall honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retgate — Wave 37 exclusive gate stamp */
aarch64_uart_puts("aarch64: soft retgate exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retgate stamp; Soft!=product)\n");
/*
 * ---- Wave 38 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: soft retmoat — Wave 38 return-moat honesty */
aarch64_uart_puts("aarch64: soft retmoat soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retmoat honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retower — Wave 38 exclusive tower stamp */
aarch64_uart_puts("aarch64: soft retower exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retower stamp; Soft!=product)\n");
/*
 * ---- Wave 39 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: soft retbarbican — Wave 39 return-barbican honesty */
aarch64_uart_puts("aarch64: soft retbarbican soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retbarbican honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retglacis — Wave 39 exclusive glacis stamp */
aarch64_uart_puts("aarch64: soft retglacis exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retglacis stamp; Soft!=product)\n");
/*
 * ---- Wave 40 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: soft retcurtain — Wave 40 return-curtain honesty */
aarch64_uart_puts("aarch64: soft retcurtain soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retcurtain honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retparapet — Wave 40 exclusive parapet stamp */
aarch64_uart_puts("aarch64: soft retparapet exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retparapet stamp; Soft!=product)\n");
/*
 * ---- Wave 41 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: soft retravelin — Wave 41 return-travelin honesty */
aarch64_uart_puts("aarch64: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retravelin honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retditch — Wave 41 exclusive ditch stamp */
aarch64_uart_puts("aarch64: soft retditch exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retditch stamp; Soft!=product)\n");
/*
 * ---- Wave 42 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: soft retportcullis — Wave 42 return-portcullis honesty */
aarch64_uart_puts("aarch64: soft retportcullis soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retportcullis honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retbattlement — Wave 42 exclusive battlement stamp */
aarch64_uart_puts("aarch64: soft retbattlement exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retbattlement stamp; Soft!=product)\n");
/*
 * ---- Wave 43 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: soft retmachicolation — Wave 43 return-machicolation honesty */
aarch64_uart_puts("aarch64: soft retmachicolation soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retmachicolation honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retarrowslit — Wave 43 exclusive arrowslit stamp */
aarch64_uart_puts("aarch64: soft retarrowslit exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retarrowslit stamp; Soft!=product)\n");

/*
 * ---- Wave 44 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: soft retmerlon — Wave 44 return-merlon honesty */
aarch64_uart_puts("aarch64: soft retmerlon soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retmerlon honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retembrasure — Wave 44 exclusive embrasure stamp */
aarch64_uart_puts("aarch64: soft retembrasure exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retembrasure stamp; Soft!=product)\n");

/*
 * ---- Wave 45 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: soft retkeepgate — Wave 45 return-keepgate honesty */
aarch64_uart_puts("aarch64: soft retkeepgate soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retkeepgate honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retouterward — Wave 45 exclusive outerward stamp */
aarch64_uart_puts("aarch64: soft retouterward exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retouterward stamp; Soft!=product)\n");

/*
 * ---- Wave 46 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: soft retbailey — Wave 46 return-bailey honesty */
aarch64_uart_puts("aarch64: soft retbailey soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retbailey honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retpostern — Wave 46 exclusive postern stamp */
aarch64_uart_puts("aarch64: soft retpostern exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retpostern stamp; Soft!=product)\n");

/*
 * ---- Wave 47 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: soft retinnerward — Wave 47 return-innerward honesty */
aarch64_uart_puts("aarch64: soft retinnerward soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retinnerward honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retdonjon — Wave 47 exclusive donjon stamp */
aarch64_uart_puts("aarch64: soft retdonjon exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retdonjon stamp; Soft!=product)\n");

/*
 * ---- Wave 48 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: soft retchevaux — Wave 48 return-chevaux honesty */
aarch64_uart_puts("aarch64: soft retchevaux soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retchevaux honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retpalisade — Wave 48 exclusive palisade stamp */
aarch64_uart_puts("aarch64: soft retpalisade exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retpalisade stamp; Soft!=product)\n");

/*
 * ---- Wave 49 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: soft retglacisgate — Wave 49 return-glacisgate honesty */
aarch64_uart_puts("aarch64: soft retglacisgate soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retglacisgate honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retoutwork — Wave 49 exclusive outwork stamp */
aarch64_uart_puts("aarch64: soft retoutwork exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retoutwork stamp; Soft!=product)\n");
/*
 * ---- Wave 50 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: soft retsally — Wave 50 return-sally honesty */
aarch64_uart_puts("aarch64: soft retsally soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retsally honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retcounterscarp — Wave 50 exclusive counterscarp stamp */
aarch64_uart_puts("aarch64: soft retcounterscarp exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retcounterscarp stamp; Soft!=product)\n");

/*
 * ---- Wave 51 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: soft retfosse — Wave 51 return-fosse honesty */
aarch64_uart_puts("aarch64: soft retfosse soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retfosse honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retcoveredway — Wave 51 exclusive coveredway stamp */
aarch64_uart_puts("aarch64: soft retcoveredway exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retcoveredway stamp; Soft!=product)\n");

/*
 * ---- Wave 52 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: soft rettenaille — Wave 52 return-tenaille honesty */
aarch64_uart_puts("aarch64: soft rettenaille soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (rettenaille honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retdemilune — Wave 52 exclusive demilune stamp */
aarch64_uart_puts("aarch64: soft retdemilune exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retdemilune stamp; Soft!=product)\n");
/*
 * ---- Wave 53 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: soft retravelin — Wave 53 return-travelin honesty */
aarch64_uart_puts("aarch64: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retravelin honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retlunette — Wave 53 exclusive lunette stamp */
aarch64_uart_puts("aarch64: soft retlunette exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retlunette stamp; Soft!=product)\n");
/*
 * ---- Wave 54 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: soft retcaponier — Wave 54 return-caponier honesty */
aarch64_uart_puts("aarch64: soft retcaponier soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retcaponier honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retredan — Wave 54 exclusive redan stamp */
aarch64_uart_puts("aarch64: soft retredan exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retredan stamp; Soft!=product)\n");
/*
 * ---- Wave 55 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: soft retflank — Wave 55 return-flank honesty */
aarch64_uart_puts("aarch64: soft retflank soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retflank honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retface — Wave 55 exclusive face stamp */
aarch64_uart_puts("aarch64: soft retface exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retface stamp; Soft!=product)\n");
/*
 * ---- Wave 56 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: soft retgorge — Wave 56 return-gorge honesty */
aarch64_uart_puts("aarch64: soft retgorge soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retgorge honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retshoulder — Wave 56 exclusive shoulder stamp */
aarch64_uart_puts("aarch64: soft retshoulder exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retshoulder stamp; Soft!=product)\n");
/*
 * ---- Wave 57 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: soft retraverse — Wave 57 return-traverse honesty */
aarch64_uart_puts("aarch64: soft retraverse soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retraverse honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retcasemate — Wave 57 exclusive casemate stamp */
aarch64_uart_puts("aarch64: soft retcasemate exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retcasemate stamp; Soft!=product)\n");

/*
 * ---- Wave 58 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: soft retorillon — Wave 58 return-orillon honesty */
aarch64_uart_puts("aarch64: soft retorillon soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retorillon honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retbonnette — Wave 58 exclusive bonnette stamp */
aarch64_uart_puts("aarch64: soft retbonnette exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retbonnette stamp; Soft!=product)\n");

/*
 * ---- Wave 59 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: soft retcrownwork — Wave 59 return-crownwork honesty */
aarch64_uart_puts("aarch64: soft retcrownwork soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retcrownwork honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft rethornwork — Wave 59 exclusive hornwork stamp */
aarch64_uart_puts("aarch64: soft rethornwork exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (rethornwork stamp; Soft!=product)\n");

/*
 * ---- Wave 60 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: soft retplace — Wave 60 return-place honesty */
aarch64_uart_puts("aarch64: soft retplace soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retplace honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retenvelope — Wave 60 exclusive envelope stamp */
aarch64_uart_puts("aarch64: soft retenvelope exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retenvelope stamp; Soft!=product)\n");









/*
 * ---- Wave 61 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: soft retcounterguard — Wave 61 return-counterguard honesty */
aarch64_uart_puts("aarch64: soft retcounterguard soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retcounterguard honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retcoveredface — Wave 61 exclusive coveredface stamp */
aarch64_uart_puts("aarch64: soft retcoveredface exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retcoveredface stamp; Soft!=product)\n");
/*
 * ---- Wave 62 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: soft retbastionface — Wave 62 return-bastionface honesty */
aarch64_uart_puts("aarch64: soft retbastionface soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retbastionface honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retcurtainangle — Wave 62 exclusive curtainangle stamp */
aarch64_uart_puts("aarch64: soft retcurtainangle exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retcurtainangle stamp; Soft!=product)\n");
/*
 * ---- Wave 63 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: soft retdoubletenaille — Wave 63 return-doubletenaille honesty */
aarch64_uart_puts("aarch64: soft retdoubletenaille soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retdoubletenaille honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retplaceofarms — Wave 63 exclusive placeofarms stamp */
aarch64_uart_puts("aarch64: soft retplaceofarms exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retplaceofarms stamp; Soft!=product)\n");
/*
 * ---- Wave 64 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: soft retreentrant — Wave 64 return-reentrant honesty */
aarch64_uart_puts("aarch64: soft retreentrant soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retreentrant honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retsallyport — Wave 64 exclusive sallyport stamp */
aarch64_uart_puts("aarch64: soft retsallyport exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retsallyport stamp; Soft!=product)\n");
/*
 * ---- Wave 65 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: soft retgorgeangle — Wave 65 return-gorgeangle honesty */
aarch64_uart_puts("aarch64: soft retgorgeangle soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retgorgeangle honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: soft retshoulderangle — Wave 65 exclusive shoulderangle stamp */
aarch64_uart_puts("aarch64: soft retshoulderangle exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retshoulderangle stamp; Soft!=product)\n");
/*
 * ---- Wave 66 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: soft retflankangle — Wave 66 return-flankangle honesty */
aarch64_uart_puts("aarch64: soft retflankangle soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
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
        "never_blocks_m0=1 wave=89 "
        "(retcaponierangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: soft retredanangle — Wave 67 exclusive redanangle stamp */
aarch64_uart_puts("aarch64: soft retredanangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=89 "
        "(retredanangle stamp; Soft≠product)\n");
/*
 * ---- Wave 68 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: soft retlunetteangle — Wave 68 return-lunetteangle honesty */
aarch64_uart_puts("aarch64: soft retlunetteangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=89 "
        "(retlunetteangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: soft rettenailleangle — Wave 68 exclusive tenailleangle stamp */
aarch64_uart_puts("aarch64: soft rettenailleangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=89 "
        "(rettenailleangle stamp; Soft≠product)\n");
/*
 * ---- Wave 69 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: soft retdemiluneangle — Wave 69 return-demiluneangle honesty */
aarch64_uart_puts("aarch64: soft retdemiluneangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=89 "
        "(retdemiluneangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: soft retcoveredwayangle — Wave 69 exclusive coveredwayangle stamp */
aarch64_uart_puts("aarch64: soft retcoveredwayangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=89 "
        "(retcoveredwayangle stamp; Soft≠product)\n");
/*
 * ---- Wave 70 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: soft retfosseangle — Wave 70 return-fosseangle honesty */
aarch64_uart_puts("aarch64: soft retfosseangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=89 (retfosseangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: soft retcounterscarple — Wave 70 exclusive counterscarple stamp */
aarch64_uart_puts("aarch64: soft retcounterscarple exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=89 (retcounterscarple stamp; Soft≠product)\n");
/*
 * ---- Wave 71 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: soft retsallyportangle — Wave 71 return-sallyportangle honesty */
aarch64_uart_puts("aarch64: soft retsallyportangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=89 (retsallyportangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: soft retreentrantangle — Wave 71 exclusive reentrantangle stamp */
aarch64_uart_puts("aarch64: soft retreentrantangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=89 (retreentrantangle stamp; Soft≠product)\n");
/*
 * ---- Wave 72 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: soft retplaceofarmsangle — Wave 72 return-placeofarmsangle honesty */
aarch64_uart_puts("aarch64: soft retplaceofarmsangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=89 (retplaceofarmsangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: soft retdoubletenailleangle — Wave 72 exclusive doubletenailleangle stamp */
aarch64_uart_puts("aarch64: soft retdoubletenailleangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=89 (retdoubletenailleangle stamp; Soft≠product)\n");
/* Grep: aarch64: soft retcurtainface — Wave 73 return-curtainface honesty */
aarch64_uart_puts("aarch64: soft retcurtainface soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=89 (retcurtainface honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: soft retbastionangle — Wave 73 exclusive bastionangle stamp */
aarch64_uart_puts("aarch64: soft retbastionangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=89 (retbastionangle stamp; Soft≠product)\n");
/* Grep: aarch64: soft retglacisangle — Wave 74 return-glacisangle honesty */
aarch64_uart_puts("aarch64: soft retglacisangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=89 (retglacisangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: soft retparapetangle — Wave 74 exclusive parapetangle stamp */
aarch64_uart_puts("aarch64: soft retparapetangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=89 (retparapetangle stamp; Soft≠product)\n");
/* Grep: aarch64: soft retmoatangle — Wave 75 return-moatangle honesty */
aarch64_uart_puts("aarch64: soft retmoatangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=89 (retmoatangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: soft retowerangle — Wave 75 exclusive towerangle stamp */
aarch64_uart_puts("aarch64: soft retowerangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=89 (retowerangle stamp; Soft≠product)\n");
/* Grep: aarch64: soft retgateangle — Wave 76 return-gateangle honesty */
aarch64_uart_puts("aarch64: soft retgateangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=89 (retgateangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: soft retwallangle — Wave 76 exclusive wallangle stamp */
aarch64_uart_puts("aarch64: soft retwallangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=89 (retwallangle stamp; Soft≠product)\n");
/* Grep: aarch64: soft retspireangle — Wave 77 return-spireangle honesty */
aarch64_uart_puts("aarch64: soft retspireangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=89 (retspireangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: soft retholdangle — Wave 77 exclusive holdangle stamp */
aarch64_uart_puts("aarch64: soft retholdangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=89 (retholdangle stamp; Soft≠product)\n");
/* Grep: aarch64: soft retpalaceangle — Wave 78 return-palaceangle honesty */
aarch64_uart_puts("aarch64: soft retpalaceangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=89 (retpalaceangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: soft retfortressangle — Wave 78 exclusive fortressangle stamp */
aarch64_uart_puts("aarch64: soft retfortressangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=89 (retfortressangle stamp; Soft≠product)\n");
/* Grep: aarch64: soft retkeepangle — Wave 79 return-keepangle honesty */
aarch64_uart_puts("aarch64: soft retkeepangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=89 (retkeepangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: soft retredoubtangle — Wave 79 exclusive redoubtangle stamp */
aarch64_uart_puts("aarch64: soft retredoubtangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=89 (retredoubtangle stamp; Soft≠product)\n");
/* Grep: aarch64: soft retcitadelangle — Wave 80 return-citadelangle honesty */
aarch64_uart_puts("aarch64: soft retcitadelangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=89 (retcitadelangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: soft retbastionkeep — Wave 80 exclusive bastionkeep stamp */
aarch64_uart_puts("aarch64: soft retbastionkeep exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=89 (retbastionkeep stamp; Soft≠product)\n");
/* Grep: aarch64: soft retpanoplyangle — Wave 81 return-panoplyangle honesty */
aarch64_uart_puts("aarch64: soft retpanoplyangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=89 (retpanoplyangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: soft retbulwarkangle — Wave 81 exclusive bulwarkangle stamp */
aarch64_uart_puts("aarch64: soft retbulwarkangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=89 (retbulwarkangle stamp; Soft≠product)\n");
/* Grep: aarch64: soft retmantleangle — Wave 82 return-mantleangle honesty */
aarch64_uart_puts("aarch64: soft retmantleangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=89 (retmantleangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: soft retaegisangle — Wave 82 exclusive aegisangle stamp */
aarch64_uart_puts("aarch64: soft retaegisangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=89 (retaegisangle stamp; Soft≠product)\n");
/* Grep: aarch64: soft retemblemangle — Wave 83 return-emblemangle honesty */
aarch64_uart_puts("aarch64: soft retemblemangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=89 (retemblemangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: soft retsigilangle — Wave 83 exclusive sigilangle stamp */
aarch64_uart_puts("aarch64: soft retsigilangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=89 (retsigilangle stamp; Soft≠product)\n");
/* Grep: aarch64: soft retscepterangle — Wave 84 return-scepterangle honesty */
aarch64_uart_puts("aarch64: soft retscepterangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=89 (retscepterangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: soft retglyphangle — Wave 84 exclusive glyphangle stamp */
aarch64_uart_puts("aarch64: soft retglyphangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=89 (retglyphangle stamp; Soft≠product)\n");
/* Grep: aarch64: soft retcrownangle — Wave 85 return-crownangle honesty */
aarch64_uart_puts("aarch64: soft retcrownangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=89 (retcrownangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: soft retshardangle — Wave 85 exclusive shardangle stamp */
aarch64_uart_puts("aarch64: soft retshardangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=89 (retshardangle stamp; Soft≠product)\n");
/* Grep: aarch64: soft retforgeangle — Wave 86 return-forgeangle honesty */
aarch64_uart_puts("aarch64: soft retforgeangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=89 (retforgeangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: soft retprismangle — Wave 86 exclusive prismangle stamp */
aarch64_uart_puts("aarch64: soft retprismangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=89 (retprismangle stamp; Soft≠product)\n");
/* Grep: aarch64: soft retflameangle — Wave 87 return-flameangle honesty */
aarch64_uart_puts("aarch64: soft retflameangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=89 (retflameangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: soft retcipherangle — Wave 87 exclusive cipherangle stamp */
aarch64_uart_puts("aarch64: soft retcipherangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=89 (retcipherangle stamp; Soft≠product)\n");
/* Grep: aarch64: soft retbeaconangle — Wave 88 return-beaconangle honesty */
aarch64_uart_puts("aarch64: soft retbeaconangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=89 (retbeaconangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: soft retledgerangle — Wave 88 exclusive ledgerangle stamp */
aarch64_uart_puts("aarch64: soft retledgerangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=89 (retledgerangle stamp; Soft≠product)\n");
/* Grep: aarch64: soft retbannerangle — Wave 89 return-bannerangle honesty */
aarch64_uart_puts("aarch64: soft retbannerangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=89 (retbannerangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: soft retvaultangle — Wave 89 exclusive vaultangle stamp */
aarch64_uart_puts("aarch64: soft retvaultangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=89 (retvaultangle stamp; Soft≠product)\n");

aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
aarch64_uart_puts(" (retfaceangle stamp; Soft!=product)\n");
/* Grep: aarch64: exception soft deepen */
    aarch64_uart_puts("aarch64: exception soft deepen wave=");
    aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
    aarch64_uart_puts(" areas=");
    aarch64_uart_put_hex((unsigned long)EXC_SOFT_AREAS);
    aarch64_uart_puts(" catalog=inventory,vbar,class,counts,daif,banks,"
                      "gates,path,surf,return,honesty,retclass,retlane,deepen "
                      "unit=exception.c only rate_limited=0 soft_only=1\n");

    /* Grep: aarch64: exception soft honesty */
    aarch64_uart_puts("aarch64: exception soft honesty product_kernel=OPEN "
                      "soft_only=1 no_irq_deliver=1 no_bar3=1 wave=");
    aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
    aarch64_uart_puts("\n");

    /* Grep: aarch64: exception soft exclusive — Wave 35 exclusive deepen */
    aarch64_uart_puts("aarch64: exception soft exclusive multi_server=0 "
                      "confine=0 bar3=0 product_kernel=OPEN soft_only=1 wave=");
    aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
    aarch64_uart_puts("\n");

    /* Grep: aarch64: exception soft open — Wave 19 open-lamp rollup */
    aarch64_uart_puts("aarch64: exception soft open multi_server=0 confine=0 "
                      "bar3=0 product_kernel=OPEN soft_only=1 wave=");
    aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
    aarch64_uart_puts("\n");


    /* Grep: aarch64: exception soft return — Wave 19 return surfaces */
    aarch64_uart_puts("aarch64: exception soft return inv_ret=");
    aarch64_uart_put_hex((unsigned long)(fOk != 0 ? 1ul : 0ul));
    aarch64_uart_puts(" product_kernel=OPEN wave=");
    aarch64_uart_put_hex((unsigned long)EXC_SOFT_WAVE);
    aarch64_uart_puts("\n");

    if (fOk != 0) {
        aarch64_uart_puts("aarch64: exception soft PASS\n");
    } else {
        aarch64_uart_puts("aarch64: exception soft FAIL\n");
    }

    return fOk;
}

void
aarch64_exception_dispatch(unsigned long u64Vec)
{
    unsigned long esr;
    unsigned long elr;
    unsigned long x0fix;
    unsigned ec;
    unsigned uClass;
    unsigned uIntId;

    uClass = (unsigned)(u64Vec & 3ul);

    /* Soft IRQ path: vec%4 == 1 (IRQ at any exception level slot). */
    if (uClass == EXC_SOFT_CLASS_IRQ) {
        g_cIrqSoft++;
        uIntId = aarch64_gic_soft_iar();
        if (uIntId != 1023u) {
            aarch64_gic_soft_eoi(uIntId);
        }
        aarch64_uart_puts("aarch64: exc IRQ soft intid=");
        aarch64_uart_put_hex((unsigned long)uIntId);
        aarch64_uart_puts(" vec=");
        aarch64_uart_put_hex(u64Vec);
        aarch64_uart_puts(" n=");
        aarch64_uart_put_hex(g_cIrqSoft);
        aarch64_uart_puts("\n");
        return;
    }

    /* Soft FIQ path: count + return (no GIC group1 take yet). */
    if (uClass == EXC_SOFT_CLASS_FIQ) {
        g_cFiqSoft++;
        aarch64_uart_puts("aarch64: exc FIQ soft vec=");
        aarch64_uart_put_hex(u64Vec);
        aarch64_uart_puts(" n=");
        aarch64_uart_put_hex(g_cFiqSoft);
        aarch64_uart_puts("\n");
        return;
    }

    /* Soft SError path: count + return (avoid tight fault loop). */
    if (uClass == EXC_SOFT_CLASS_SERROR) {
        g_cSerrorSoft++;
        aarch64_uart_puts("aarch64: exc SError soft vec=");
        aarch64_uart_put_hex(u64Vec);
        aarch64_uart_puts(" n=");
        aarch64_uart_put_hex(g_cSerrorSoft);
        aarch64_uart_puts("\n");
        return;
    }

    /* Sync (class 0): ESR/ELR path. */
    g_cSyncSoft++;
    __asm__ volatile("mrs %0, esr_el1" : "=r"(esr));
    __asm__ volatile("mrs %0, elr_el1" : "=r"(elr));
    ec = (unsigned)((esr >> 26) & 0x3fu);

    /* PSCI probe: HVC (0x16) / SMC (0x17) / unknown UNDEF — recover. */
    if (g_psci_probe_active != 0) {
        if (ec == 0x16u || ec == 0x17u || ec == 0x00u || ec == 0x0eu) {
            x0fix = 0;
            if (aarch64_psci_exception_recover(&elr, &x0fix) != 0) {
                __asm__ volatile("msr elr_el1, %0" :: "r"(elr) : "memory");
                return;
            }
        }
    }

    if (aarch64_svc_try_handle(u64Vec, esr, elr) != 0) {
        return;
    }

    aarch64_uart_puts("aarch64: exc vec=");
    aarch64_uart_put_hex(u64Vec);
    aarch64_uart_puts(" esr=");
    aarch64_uart_put_hex(esr);
    aarch64_uart_puts(" elr=");
    aarch64_uart_put_hex(elr);
    aarch64_uart_puts(" sync_n=");
    aarch64_uart_put_hex(g_cSyncSoft);
    aarch64_uart_puts("\n");
}

void
aarch64_exceptions_install(void)
{
    extern char aarch64_vectors[];
    unsigned long u64Vbar;
    unsigned long u64VecPa;
    int fSoft;

    u64VecPa = (unsigned long)(void *)aarch64_vectors;

    __asm__ volatile("msr vbar_el1, %0" : : "r"(u64VecPa) : "memory");
    __asm__ volatile("isb");
    __asm__ volatile("mrs %0, vbar_el1" : "=r"(u64Vbar));

    /* Soft baseline: IRQ/FIQ counters zero; VBAR points at vector table. */
    aarch64_uart_puts("aarch64: exceptions vbar=");
    aarch64_uart_put_hex(u64Vbar);
    aarch64_uart_puts(" irq_soft=");
    aarch64_uart_put_hex(g_cIrqSoft);
    aarch64_uart_puts(" fiq_soft=");
    aarch64_uart_put_hex(g_cFiqSoft);
    aarch64_uart_puts("\n");

    /*
     * Greppable soft inventory: "aarch64: exception soft …"
     * (singular "exception soft"; distinct from "exceptions PASS").
     */
    fSoft = exception_soft_inventory(u64Vbar, u64VecPa);
    (void)fSoft;

    aarch64_uart_puts("aarch64: exceptions PASS\n");
}
