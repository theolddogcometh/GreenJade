/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Laptop product attention / power path (G752VT class):
 *   - Poll 8042 PS/2 scancodes for Ctrl+Alt+Delete → reboot
 *   - Poll ACPI PM1 PWRBTN_STS → poweroff (S5) when FADT known
 *
 * Clean-room pure C11 freestanding. Public 8042 + ACPI FADT/PM1 layout only.
 * No GPL. Soft!=product residual lamps; Dual MIT OR Apache-2.0. G-AC-1.
 *
 * Policy (product laptop):
 *   CAD       → reboot (classic SAS → restart class for freestanding host)
 *   Power btn → ACPI S5 shutdown; CF9/KBC reboot if S5 unavailable
 *
 * greppable: platform_power: init | CAD | power button | reboot | shutdown
 */
#include <gj/boot_info.h>
#include <gj/fb_console.h>
#include <gj/klog.h>
#include <gj/platform_power.h>
#include <gj/string.h>
#include <gj/types.h>

/* ---- 8042 PS/2 (public PC/AT) ---------------------------------------- */
#define PS2_DATA   0x60u
#define PS2_STATUS 0x64u
#define PS2_CMD    0x64u
#define PS2_ST_OBF 0x01u
#define PS2_ST_IBF 0x02u
#define PS2_CMD_PULSE_RESET 0xfeu /* pulse reset line */

/* Set-1 make/break (8042 translate path common on PC) */
#define SC_CTRL_L   0x1du
#define SC_CTRL_L_B 0x9du
#define SC_ALT_L    0x38u
#define SC_ALT_L_B  0xb8u
#define SC_EXT      0xe0u
#define SC_DEL      0x53u /* E0 53 = Delete make */
#define SC_DEL_B    0xd3u
#define SC_CTRL_R   0x1du /* with E0 = Right Ctrl */
#define SC_ALT_R    0x38u /* with E0 = Right Alt */

/* ---- PCI reset (Intel chipset public) -------------------------------- */
#define CF9_PORT    0xcf9u
#define CF9_FULL    0x0eu /* full reset pulse */

/* ---- ACPI FADT / PM1 (public ACPI tables) ---------------------------- */
struct acpi_sdt_hdr {
    char  aSig[4];
    u32   u32Length;
    u8    u8Rev;
    u8    u8Checksum;
    char  aOem[6];
    char  aOemTable[8];
    u32   u32OemRev;
    u32   u32CreatorId;
    u32   u32CreatorRev;
} __attribute__((packed));

struct acpi_rsdp_v1 {
    char aSig[8]; /* "RSD PTR " */
    u8   u8Checksum;
    char aOem[6];
    u8   u8Rev;
    u32  u32Rsdt;
} __attribute__((packed));

struct acpi_rsdp_v2 {
    struct acpi_rsdp_v1 v1;
    u32 u32Length;
    u64 u64Xsdt;
    u8  u8XChecksum;
    u8  aRsv[3];
} __attribute__((packed));

/*
 * Fixed ACPI Description Table (FACP) — fields we use only.
 * Offsets match ACPI 1.0 / 2.0 public FADT layout for 32-bit PM1 ports.
 */
#define FADT_OFF_PM1A_EVT  56u
#define FADT_OFF_PM1B_EVT  60u
#define FADT_OFF_PM1A_CNT  64u
#define FADT_OFF_PM1B_CNT  68u
#define FADT_OFF_PM1_EVT_LEN 88u /* ACPI 1.0 FADT: PM1_EVT_LEN at 88 */
#define FADT_OFF_PM1_CNT_LEN 89u

/* PM1 status/enable: power button is bit 8 (public ACPI fixed events). */
#define PM1_PWRBTN  (1u << 8)
/* PM1 control: SLP_TYP bits 12:10, SLP_EN bit 13. */
#define PM1_SLP_EN  (1u << 13)

static u8  g_fInited;
static u8  g_fAcpiOk;
static u8  g_fCadLamp;
static u8  g_fPwrLamp;
static u8  g_fExt;       /* next scancode is E0-extended */
static u8  g_fCtrl;
static u8  g_fAlt;
static u8  g_fDel;
static u16 g_u16Pm1aEvt;
static u16 g_u16Pm1bEvt;
static u16 g_u16Pm1aCnt;
static u16 g_u16Pm1bCnt;
static u8  g_u8Pm1EvtLen; /* typically 4: 2 status + 2 enable */
static u8  g_u8Pm1CntLen;
static u32 g_u32CadHits;
static u32 g_u32PwrHits;
static u32 g_u32PollN;

static inline u8
inb(u16 u16Port)
{
    u8 u8V;
    __asm__ volatile("inb %1, %0" : "=a"(u8V) : "Nd"(u16Port));
    return u8V;
}

static inline void
outb(u16 u16Port, u8 u8V)
{
    __asm__ volatile("outb %0, %1" : : "a"(u8V), "Nd"(u16Port));
}

static inline u16
inw(u16 u16Port)
{
    u16 u16V;
    __asm__ volatile("inw %1, %0" : "=a"(u16V) : "Nd"(u16Port));
    return u16V;
}

static inline void
outw(u16 u16Port, u16 u16V)
{
    __asm__ volatile("outw %0, %1" : : "a"(u16V), "Nd"(u16Port));
}

static int
acpi_checksum(const void *p, u32 cb)
{
    const u8 *pB = (const u8 *)p;
    u32 i;
    u8 u8Sum = 0;

    if (p == NULL || cb == 0u) {
        return 0;
    }
    for (i = 0; i < cb; i++) {
        u8Sum = (u8)(u8Sum + pB[i]);
    }
    return (u8Sum == 0u) ? 1 : 0;
}

static void
fadt_take(const struct acpi_sdt_hdr *pHdr)
{
    const u8 *pB;
    u32 u32Len;

    if (pHdr == NULL) {
        return;
    }
    u32Len = pHdr->u32Length;
    if (u32Len < 90u) {
        return;
    }
    if (!acpi_checksum(pHdr, u32Len)) {
        return;
    }
    pB = (const u8 *)pHdr;
    g_u16Pm1aEvt = (u16)(*(const u32 *)(const void *)(pB + FADT_OFF_PM1A_EVT));
    g_u16Pm1bEvt = (u16)(*(const u32 *)(const void *)(pB + FADT_OFF_PM1B_EVT));
    g_u16Pm1aCnt = (u16)(*(const u32 *)(const void *)(pB + FADT_OFF_PM1A_CNT));
    g_u16Pm1bCnt = (u16)(*(const u32 *)(const void *)(pB + FADT_OFF_PM1B_CNT));
    g_u8Pm1EvtLen = pB[FADT_OFF_PM1_EVT_LEN];
    g_u8Pm1CntLen = pB[FADT_OFF_PM1_CNT_LEN];
    if (g_u16Pm1aEvt == 0u || g_u16Pm1aCnt == 0u) {
        return;
    }
    if (g_u8Pm1EvtLen < 2u) {
        g_u8Pm1EvtLen = 4u; /* common default: 16-bit STS + 16-bit EN */
    }
    if (g_u8Pm1CntLen < 2u) {
        g_u8Pm1CntLen = 2u;
    }
    g_fAcpiOk = 1u;
}

static void
walk_sdt_for_fadt(u64 u64SdtPa, int fXsdt)
{
    const struct acpi_sdt_hdr *pRoot;
    u32 u32Len;
    u32 u32EntBytes;
    u32 cEnt;
    u32 i;

    if (u64SdtPa == 0ull) {
        return;
    }
    pRoot = (const struct acpi_sdt_hdr *)(gj_vaddr_t)u64SdtPa;
    u32Len = pRoot->u32Length;
    if (u32Len < sizeof(*pRoot) + 4u || u32Len > (1u << 20)) {
        return;
    }
    if (!acpi_checksum(pRoot, u32Len)) {
        return;
    }
    u32EntBytes = fXsdt ? 8u : 4u;
    cEnt = (u32Len - (u32)sizeof(*pRoot)) / u32EntBytes;
    for (i = 0; i < cEnt && g_fAcpiOk == 0u; i++) {
        u64 u64Ent;
        const struct acpi_sdt_hdr *pEnt;

        if (fXsdt != 0) {
            u64Ent = ((const u64 *)(const void *)(pRoot + 1))[i];
        } else {
            u64Ent = (u64)((const u32 *)(const void *)(pRoot + 1))[i];
        }
        if (u64Ent == 0ull) {
            continue;
        }
        pEnt = (const struct acpi_sdt_hdr *)(gj_vaddr_t)u64Ent;
        if (pEnt->aSig[0] == 'F' && pEnt->aSig[1] == 'A' &&
            pEnt->aSig[2] == 'C' && pEnt->aSig[3] == 'P') {
            fadt_take(pEnt);
        }
    }
}

static void
acpi_discover(void)
{
    const struct gj_boot_info *pBi;
    const struct acpi_rsdp_v1 *pV1;
    const struct acpi_rsdp_v2 *pV2;
    u64 u64Rsdp;

    pBi = boot_info_get();
    u64Rsdp = (pBi != NULL) ? pBi->u64Rsdp : 0ull;
    if (u64Rsdp == 0ull) {
        return;
    }
    pV1 = (const struct acpi_rsdp_v1 *)(gj_vaddr_t)u64Rsdp;
    if (pV1->aSig[0] != 'R' || pV1->aSig[1] != 'S' || pV1->aSig[2] != 'D' ||
        pV1->aSig[3] != ' ' || pV1->aSig[4] != 'P' || pV1->aSig[5] != 'T' ||
        pV1->aSig[6] != 'R' || pV1->aSig[7] != ' ') {
        return;
    }
    if (!acpi_checksum(pV1, 20u)) {
        return;
    }
    if (pV1->u8Rev >= 2u) {
        pV2 = (const struct acpi_rsdp_v2 *)(gj_vaddr_t)u64Rsdp;
        if (pV2->u32Length >= sizeof(*pV2) &&
            acpi_checksum(pV2, pV2->u32Length) && pV2->u64Xsdt != 0ull) {
            walk_sdt_for_fadt(pV2->u64Xsdt, 1);
        }
    }
    if (g_fAcpiOk == 0u && pV1->u32Rsdt != 0u) {
        walk_sdt_for_fadt((u64)pV1->u32Rsdt, 0);
    }
}

static void
pwrbtn_enable(void)
{
    u16 u16EnPort;
    u16 u16En;

    if (g_fAcpiOk == 0u || g_u16Pm1aEvt == 0u) {
        return;
    }
    /* Enable register follows status (PM1_EVT_LEN/2 bytes). */
    u16EnPort = (u16)(g_u16Pm1aEvt + (u16)(g_u8Pm1EvtLen / 2u));
    u16En = inw(u16EnPort);
    u16En = (u16)(u16En | (u16)PM1_PWRBTN);
    outw(u16EnPort, u16En);
    if (g_u16Pm1bEvt != 0u) {
        u16EnPort = (u16)(g_u16Pm1bEvt + (u16)(g_u8Pm1EvtLen / 2u));
        u16En = inw(u16EnPort);
        u16En = (u16)(u16En | (u16)PM1_PWRBTN);
        outw(u16EnPort, u16En);
    }
    /* Clear sticky power-button status if already set. */
    outw(g_u16Pm1aEvt, (u16)PM1_PWRBTN);
    if (g_u16Pm1bEvt != 0u) {
        outw(g_u16Pm1bEvt, (u16)PM1_PWRBTN);
    }
}

static int
pwrbtn_fired(void)
{
    u16 u16St;

    if (g_fAcpiOk == 0u || g_u16Pm1aEvt == 0u) {
        return 0;
    }
    u16St = inw(g_u16Pm1aEvt);
    if ((u16St & (u16)PM1_PWRBTN) != 0u) {
        outw(g_u16Pm1aEvt, (u16)PM1_PWRBTN); /* W1C clear */
        if (g_u16Pm1bEvt != 0u) {
            outw(g_u16Pm1bEvt, (u16)PM1_PWRBTN);
        }
        return 1;
    }
    if (g_u16Pm1bEvt != 0u) {
        u16St = inw(g_u16Pm1bEvt);
        if ((u16St & (u16)PM1_PWRBTN) != 0u) {
            outw(g_u16Pm1bEvt, (u16)PM1_PWRBTN);
            return 1;
        }
    }
    return 0;
}

void
platform_power_reboot(void)
{
    u32 i;

    kprintf("platform_power: reboot path=kbc+cf9 Soft!=product\n");
    fb_console_hold(0, "SHUTDOWN: reboot (Ctrl+Alt+Del / fallback)");
    /* Drain IBF then pulse reset via 8042. */
    for (i = 0; i < 100000u; i++) {
        if ((inb(PS2_STATUS) & PS2_ST_IBF) == 0u) {
            break;
        }
    }
    outb(PS2_CMD, (u8)PS2_CMD_PULSE_RESET);
    for (i = 0; i < 1000000u; i++) {
        __asm__ volatile("pause");
    }
    /* Chipset CF9 full reset. */
    outb(CF9_PORT, 0x02u);
    for (i = 0; i < 10000u; i++) {
        __asm__ volatile("pause");
    }
    outb(CF9_PORT, (u8)CF9_FULL);
    for (;;) {
        __asm__ volatile("cli; hlt");
    }
}

void
platform_power_shutdown(void)
{
    u32 u32Typ;
    u32 i;

    kprintf("platform_power: shutdown path=acpi_s5 acpi_ok=%u Soft!=product\n",
            (unsigned)g_fAcpiOk);
    fb_console_hold(0, "SHUTDOWN: power button / S5");

    if (g_fAcpiOk != 0u && g_u16Pm1aCnt != 0u) {
        /*
         * Public PM1_CNT: SLP_TYPx in bits 12:10, SLP_EN bit 13.
         * SLP_TYP for S5 lives in DSDT _S5_; freestanding tries common
         * values 0..7 (public sleep-type field width). Soft!=product.
         */
        for (u32Typ = 0u; u32Typ < 8u; u32Typ++) {
            u16 u16Cnt = (u16)(((u32Typ & 7u) << 10) | PM1_SLP_EN);

            outw(g_u16Pm1aCnt, u16Cnt);
            if (g_u16Pm1bCnt != 0u) {
                outw(g_u16Pm1bCnt, u16Cnt);
            }
            for (i = 0; i < 200000u; i++) {
                __asm__ volatile("pause");
            }
        }
    }
    /* S5 did not take: reboot so the laptop is not wedged on. */
    platform_power_reboot();
}

static void
ps2_wait_ibf_clear(void)
{
    u32 i;

    for (i = 0; i < 10000u; i++) {
        if ((inb(PS2_STATUS) & PS2_ST_IBF) == 0u) {
            return;
        }
    }
}

static void
ps2_note_scancode(u8 u8Sc)
{
    if (u8Sc == SC_EXT) {
        g_fExt = 1u;
        return;
    }

    if (g_fExt != 0u) {
        g_fExt = 0u;
        if (u8Sc == SC_CTRL_R) {
            g_fCtrl = 1u;
        } else if (u8Sc == (u8)(SC_CTRL_R | 0x80u)) {
            g_fCtrl = 0u;
        } else if (u8Sc == SC_ALT_R) {
            g_fAlt = 1u;
        } else if (u8Sc == (u8)(SC_ALT_R | 0x80u)) {
            g_fAlt = 0u;
        } else if (u8Sc == SC_DEL) {
            g_fDel = 1u;
        } else if (u8Sc == SC_DEL_B) {
            g_fDel = 0u;
        }
    } else {
        if (u8Sc == SC_CTRL_L) {
            g_fCtrl = 1u;
        } else if (u8Sc == SC_CTRL_L_B) {
            g_fCtrl = 0u;
        } else if (u8Sc == SC_ALT_L) {
            g_fAlt = 1u;
        } else if (u8Sc == SC_ALT_L_B) {
            g_fAlt = 0u;
        } else if (u8Sc == SC_DEL) {
            /* non-E0 Del rare; still accept */
            g_fDel = 1u;
        } else if (u8Sc == SC_DEL_B) {
            g_fDel = 0u;
        }
    }

    if (g_fCtrl != 0u && g_fAlt != 0u && g_fDel != 0u) {
        g_u32CadHits++;
        if (g_fCadLamp == 0u) {
            g_fCadLamp = 1u;
            kprintf("platform_power: Ctrl+Alt+Del hit=%u path=reboot "
                    "Soft!=product G-AC-1\n",
                    (unsigned)g_u32CadHits);
        }
        platform_power_reboot();
    }
}

static void
ps2_drain(void)
{
    u32 u32N = 0u;

    while ((inb(PS2_STATUS) & PS2_ST_OBF) != 0u && u32N < 32u) {
        u8 u8Sc = inb(PS2_DATA);
        ps2_note_scancode(u8Sc);
        u32N++;
    }
}

void
platform_power_init(void)
{
    if (g_fInited != 0u) {
        return;
    }
    g_fInited = 1u;
    g_fAcpiOk = 0u;
    g_fExt = 0u;
    g_fCtrl = 0u;
    g_fAlt = 0u;
    g_fDel = 0u;
    g_u16Pm1aEvt = 0u;
    g_u16Pm1bEvt = 0u;
    g_u16Pm1aCnt = 0u;
    g_u16Pm1bCnt = 0u;
    g_u8Pm1EvtLen = 4u;
    g_u8Pm1CntLen = 2u;

    acpi_discover();
    pwrbtn_enable();

    /* Enable keyboard + aux IRQs optional; poll-mode only for bring-up.
     * Soft residual: do not thrash 8042 command byte; OBF poll is enough. */
    ps2_wait_ibf_clear();
    /* Drain any stale bytes. */
    ps2_drain();

    kprintf("platform_power: init acpi_ok=%u pm1a_evt=0x%x pm1a_cnt=0x%x "
            "cad=1 pwrbtn=%u path=poll Soft!=product G-AC-1\n",
            (unsigned)g_fAcpiOk, (unsigned)g_u16Pm1aEvt,
            (unsigned)g_u16Pm1aCnt, (unsigned)(g_fAcpiOk != 0u ? 1u : 0u));
}

void
platform_power_poll(void)
{
    if (g_fInited == 0u) {
        return;
    }
    g_u32PollN++;

    ps2_drain();

    if (pwrbtn_fired() != 0) {
        g_u32PwrHits++;
        if (g_fPwrLamp == 0u) {
            g_fPwrLamp = 1u;
            kprintf("platform_power: power button hit=%u path=shutdown "
                    "Soft!=product G-AC-1\n",
                    (unsigned)g_u32PwrHits);
        }
        platform_power_shutdown();
    }
}

u32
platform_power_cad_hits(void)
{
    return g_u32CadHits;
}

u32
platform_power_pwrbtn_hits(void)
{
    return g_u32PwrHits;
}

int
platform_power_acpi_ok(void)
{
    return (g_fAcpiOk != 0u) ? 1 : 0;
}
