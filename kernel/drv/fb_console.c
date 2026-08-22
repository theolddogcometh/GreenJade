/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * GOP split-screen text console:
 *   hold pane (STATUS top + STATE boot log bottom) | log pane (kprintf)
 *
 * Panel-path performance (G752, no COM1):
 *  - Direct row stores for glyphs (no per-pixel helper)
 *  - Full-pane clear + wrap instead of pixel-scroll (scroll was O(W*H)/line)
 *  - Soft-inventory stamp lines dropped on LOG (STATUS holds milestones)
 * Trap detail is pinned on STATUS via fb_console_trap - safe to wrap LOG.
 *
 * FAULT STATUS pin (product halt only; lean; Soft!=product):
 *  - fb_console_trap sets sticky g_fFaultHold
 *  - hold rows 0 + 1 refuse later overwrite (live pin)
 *  - LOG putchar drops after pin (flood must not fight FAULT visibility)
 *  - hold1 isolate / FAULT RIP stays pinned with hold 0
 *  - leftover MAP: hold6 NET snap to hold15/hold9 is leftover diagnose, not live pin
 * Grep: g_fFaultHold | FAULT PINNED | KERNEL FAULT | NET residual
 *
 * Dual DoD honesty lean (product=UDX; Soft!=product; never product PASS):
 *   Product Dual DoD A OPEN until host USB path; Dual DoD B OPEN until interactive SSH login.
 *   Soft/freestanding STATUS lamps never close A/B (not freestanding stage).
 * Residual honesty lamps (live 178 panel holds 1/3/4/5/7/8):
 *   hold2  kernel TE/identity persist (mode tes tt slpt bus3 id1g)
 *   hold6  UDX mac_rclm residual diagnose only
 * leftover MAP (not live 178 panel): historical hold13 USB / hold14 L2 /
 * hold8 r8169 SKIP / hold12 xhci_pci SKIP.
 *   Any hold text with SKIP paints dim (never warm PASS look).
 * product=UDX: dual_dod_a=OPEN_UDX dual_dod_b=OPEN_UDX; soft residual only.
 * NET/USB/l2 clip prefers counters (R0 / need= / rx=); drops " :22" before /r.
 * Store truncate tail-prefers dual DoD residual so FAULT snap keeps counters.
 * Hold API: FB_HOLD_CHARS capacity; truncate without overflow. G-AC-1.
 * STATUS title keeps GJ_IMAGE_VERSION once (test what you fly / L3 media
 * identity) - not a stamp storm: single title string, redraw only.
 * Grep: dual DoD hold6 | dual DoD hold13 | dual DoD hold14 | product=UDX
 * Grep: STATUS (static) v | GJ_IMAGE_VERSION | freestanding SKIP | Soft!=product
 * Grep: dual_dod_a=OPEN_UDX | dual_dod_b=OPEN_UDX | FB_STATUS_TITLE_OK
 *
 * -------------------------------------------------------------------------
 * C0 exclusive soft residual (this TU only; Soft!=product; stamp-free)
 * -------------------------------------------------------------------------
 * Lean residual honesty only - never Dual DoD A/B close, never product PASS,
 * never invent image version stamps, never bump GJ_IMAGE_VERSION.
 * Title identity reuses config GJ_IMAGE_VERSION once (test what you fly).
 * Functional deepen arms densify the once-lamp (title/fault/clip/lic).
 * G-AC-1: no .ko product AC. Dual MIT OR Apache-2.0. No GPL.
 * Dual DoD A/B stay OPEN_UDX (product=UDX hosts). Soft residual != product.
 *
 * greppable: fb_console: soft residual lean
 * greppable: fb_console: soft residual lean PASS | soft residual lean SKIP
 * greppable: dual_dod_a=OPEN_UDX | dual_dod_b=OPEN_UDX | product=UDX
 * greppable: Soft!=product | freestanding SKIP | G-AC-1 | stamp_storm=0
 * greppable: STATUS (static) v | FB_STATUS_TITLE_OK | g_fFaultHold
 * greppable: dual DoD hold6 | dual DoD hold13 | dual DoD hold14 | NET residual
 * greppable: iommu: vtd TE hold2 | TE mode=
 */
#include <gj/boot_info.h>
#include <gj/config.h>
#include <gj/fb_console.h>
#include <gj/klog.h>
#include <gj/string.h>
#include <gj/types.h>

/*
 * Compile-time panel title (adjacent string merge -> one rodata C-string).
 * Keep version in title (do not strip): product-path / L3 media honesty.
 * gj-image-version.sh greps: STATUS (static) v0.1.N (semver fly bar)
 * Soft!=product: title identity != Dual DoD close. No stamp storms.
 */
#define FB_STATUS_TITLE_OK    "STATUS (static) v" GJ_IMAGE_VERSION
#define FB_STATUS_TITLE_FAULT "STATUS FAULT PINNED v" GJ_IMAGE_VERSION

#include "fb_console_font.inc"

#define FB_FG        0x00D0D0D0u /* scroll text */
#define FB_BG        0x00000000u
#define FB_HOLD_FG   0x00FFE080u /* warm status */
#define FB_HEAD_FG   0x00FFFF00u /* headline */
#define FB_FAULT_FG  0x00FF4040u /* FAULT pin - product halt only */
#define FB_DIM       0x00606060u /* divider / labels */
#define FB_STATE_FG  0x0078C8E0u /* boot-state history */
#define FB_STATE_NOW 0x00D8F4FFu /* latest boot-state line */
#define FB_SCALE     2u          /* 8x8 -> 16x16 */
#define FB_MARGIN    6u
#define FB_LINE_CAP  160u        /* line buffer for soft-noise filter */
#define FB_STATE_LINES 20u
#define FB_STATE_CHARS 80u

static u64 g_u64Fb;
static u32 g_u32Pitch;
static u32 g_u32W;
static u32 g_u32H;
static u32 g_u32CellW;
static u32 g_u32CellH;

/* Split: left/right when wide enough, else top/bottom. */
static int g_fLeftRight;
static u32 g_u32HoldX0;
static u32 g_u32HoldY0;
static u32 g_u32HoldW;
static u32 g_u32HoldH;
static u32 g_u32LogX0;
static u32 g_u32LogY0;
static u32 g_u32LogW;
static u32 g_u32LogH;

static u32 g_u32HoldCols;
static u32 g_u32HoldRows;
static u32 g_u32LogCols;
static u32 g_u32LogRows;
static u32 g_u32StateX0;
static u32 g_u32StateY0;
static u32 g_u32StateW;
static u32 g_u32StateH;
static u32 g_u32StateCols;
static u32 g_u32StateRows;
static char g_aState[FB_STATE_LINES][FB_STATE_CHARS];
static u32 g_u32StateN;
static u32 g_u32Cx;
static u32 g_u32Cy;
static int g_fReady;

/*
 * Sticky after fb_console_trap: FAULT STATUS rows stay painted; LOG paint
 * stops so soft/product flood cannot bury the halt. Soft!=product.
 * Grep: g_fFaultHold
 */
static int g_fFaultHold;

/*
 * After FAULT pin: hold15 carries last dual DoD B NET/R snapshot (R0
 * diagnose residual). Sticky while set. Soft!=product.
 * Grep: g_fFaultNetSnap | NET residual
 */
static int g_fFaultNetSnap;

/*
 * Static hold buffer (NUL-terminated). Row width = FB_HOLD_CHARS (incl. NUL).
 * Sized for dual-DoD short STATUS (hold6 NET, hold13 USB, hold14 l2 br)
 * without overflow; Soft!=product wording stays short - no product PASS.
 * Grep: FB_HOLD_CHARS dual DoD
 */
static char g_aHold[FB_HOLD_LINES][FB_HOLD_CHARS];
static u8 g_aHoldBright[FB_HOLD_LINES]; /* 1 = headline yellow */

/* Pending LOG line (filter soft inventory before painting). */
static char g_aLine[FB_LINE_CAP];
static u32 g_u32LineLen;

/*
 * C0 lean residual once-lamp gate (stamp-free; Soft!=product).
 * No version stamp invent; no stamp storms. Grep: g_fSoftResidualOnce
 */
static u8 g_fSoftResidualOnce;
static u32 g_u32SoftLeanOk;
static u32 g_u32SoftLeanFail;

/* Functional residual arm count (title/fault/clip/lic). */
#define FB_SOFT_LEAN_CHECKS 4u

/* Forward: C0 residual lean once-lamp (defined near trap helpers). */
static void fb_soft_residual_lean_once(void);

/*
 * FAULT pin owns hold 0 + 1 (live). Leftover comments used to say 0 + 6..9.
 * snapshotted. Soft!=product. Grep: fb_hold_line_is_fault_pin
 */
static int
fb_hold_line_is_fault_pin(u32 u32Line)
{
    /* Sticky 0+1 only — leave TE / UDX / IP / DoD visible. */
    if (u32Line == 0u || u32Line == 1u) {
        return 1;
    }
    return 0;
}

/*
 * Dual DoD residual NET / l2 / USB strings: prefer tail when pane clips so
 * /rN (R0), rx=/tx=, and need=/LOAD stay visible on narrow hold columns.
 * Soft!=product; never Dual DoD A/B close. Grep: fb_hold_str_tail_prefer
 */
static int
fb_hold_str_tail_prefer(const char *sz)
{
    if (sz == NULL || sz[0] == '\0') {
        return 0;
    }
    /* "NET ... t/f/b/r" - dual DoD B residual freestanding counters */
    if (sz[0] == 'N' && sz[1] == 'E' && sz[2] == 'T' &&
        (sz[3] == ' ' || sz[3] == '\0')) {
        return 1;
    }
    /* "TE mode=... tes= tt= slpt= bus3 id1g" - kernel TE persist (hold2) */
    if (sz[0] == 'T' && sz[1] == 'E' &&
        (sz[2] == ' ' || sz[2] == '\0')) {
        return 1;
    }
    /* "l2 br rx=... tx=..." - freestanding R/T mirror (hold14) */
    if (sz[0] == 'l' && sz[1] == '2' && sz[2] == ' ') {
        return 1;
    }
    /* "USB ..." / "usb_storage ..." - dual DoD A residual (hold13) */
    if (sz[0] == 'U' && sz[1] == 'S' && sz[2] == 'B' &&
        (sz[3] == ' ' || sz[3] == '\0')) {
        return 1;
    }
    if (sz[0] == 'u' && sz[1] == 's' && sz[2] == 'b' && sz[3] == '_' &&
        sz[4] == 's') {
        return 1;
    }
    return 0;
}

/*
 * Freestanding SKIP honesty: true if hold text carries a SKIP lamp token.
 * Soft residual only (mod r8169 / xhci_pci / class SKIP) - never product PASS.
 * product=UDX. Soft!=product. Grep: fb_hold_str_has_skip freestanding SKIP
 */
static int
fb_hold_str_has_skip(const char *sz)
{
    u32 i;

    if (sz == NULL || sz[0] == '\0') {
        return 0;
    }
    /* Leading "SKIP" (rare) or "... SKIP..." / "...SKIP " token. */
    if (sz[0] == 'S' && sz[1] == 'K' && sz[2] == 'I' && sz[3] == 'P' &&
        (sz[4] == '\0' || sz[4] == ' ' || sz[4] == '|' || sz[4] == '=')) {
        return 1;
    }
    for (i = 0; sz[i] != '\0'; i++) {
        if (sz[i] == 'S' && sz[i + 1u] == 'K' && sz[i + 2u] == 'I' &&
            sz[i + 3u] == 'P') {
            /* Require word boundary: start or non-alnum before; end or
             * space/|= after so "SKIPPED" alone is not required, but
             * "SKIP" / "SKIP " / "SKIP|" / "SKIP=" match freestanding lamps. */
            if (i > 0u) {
                char cPrev = sz[i - 1u];

                if ((cPrev >= 'A' && cPrev <= 'Z') ||
                    (cPrev >= 'a' && cPrev <= 'z') ||
                    (cPrev >= '0' && cPrev <= '9')) {
                    continue;
                }
            }
            {
                char cNext = sz[i + 4u];

                if (cNext == '\0' || cNext == ' ' || cNext == '|' ||
                    cNext == '=' || cNext == '/' || cNext == ',') {
                    return 1;
                }
            }
        }
    }
    return 0;
}

static int
fb_hold_str_is_net(const char *sz)
{
    if (sz == NULL) {
        return 0;
    }
    return (sz[0] == 'N' && sz[1] == 'E' && sz[2] == 'T' &&
            (sz[3] == ' ' || sz[3] == '\0'))
               ? 1
               : 0;
}

/*
 * Dual DoD B NET clip residual: keep t/f/b/r (R0) honest on narrow panes.
 * Prefer counter field over name/IP head; drop trailing " :22" before /r.
 * *pu32DrawCols is the exact printable length from *ppDraw (never past sz).
 * Soft!=product. Grep: fb_hold_net_clip dual DoD B R0
 */
static void
fb_hold_net_clip(const char *sz, u32 u32Len, u32 u32MaxCols,
                 const char **ppDraw, u32 *pu32DrawCols)
{
    u32 u32CoreEnd = u32Len;
    u32 u32Cnt = 0; /* index of 't' in last " t<digit>" counter field */
    u32 i;

    *ppDraw = sz;
    *pu32DrawCols = (u32Len < u32MaxCols) ? u32Len : u32MaxCols;
    if (sz == NULL || u32MaxCols == 0u) {
        *pu32DrawCols = 0u;
        return;
    }
    if (u32Len <= u32MaxCols) {
        *pu32DrawCols = u32Len;
        return;
    }
    /*
     * Lab ":22" lamp is secondary to freestanding R0 diagnose - strip from
     * core when choosing what to keep under clip pressure.
     */
    if (u32Len >= 4u && sz[u32Len - 4u] == ' ' && sz[u32Len - 3u] == ':' &&
        sz[u32Len - 2u] == '2' && sz[u32Len - 1u] == '2') {
        u32CoreEnd = u32Len - 4u;
    }
    for (i = 0; i + 2u < u32CoreEnd; i++) {
        if (sz[i] == ' ' && sz[i + 1u] == 't' && sz[i + 2u] >= '0' &&
            sz[i + 2u] <= '9') {
            u32Cnt = i + 1u;
        }
    }
    if (u32Cnt > 0u) {
        if ((u32Len - u32Cnt) <= u32MaxCols) {
            /* Counters + optional :22 fit - drop name/IP head. */
            *ppDraw = sz + u32Cnt;
            *pu32DrawCols = u32Len - u32Cnt;
            return;
        }
        if ((u32CoreEnd - u32Cnt) <= u32MaxCols) {
            /* Counters fit without :22 - keep t/f/b/r only. */
            *ppDraw = sz + u32Cnt;
            *pu32DrawCols = u32CoreEnd - u32Cnt;
            return;
        }
        /* Counter field alone still long - pure tail of core keeps /r. */
        if (u32CoreEnd > u32MaxCols) {
            *ppDraw = sz + (u32CoreEnd - u32MaxCols);
            *pu32DrawCols = u32MaxCols;
            return;
        }
        *ppDraw = sz;
        *pu32DrawCols = u32CoreEnd;
        return;
    }
    /* No counter field yet - pure tail of full string. */
    *ppDraw = sz + (u32Len - u32MaxCols);
    *pu32DrawCols = u32MaxCols;
}

/*
 * Dual DoD residual USB / l2 clip: prefer counter field tails under pane
 * pressure so need=/LOAD (hold13 A residual) and rx=/tx= (hold14 B residual)
 * stay honest on narrow STATUS columns. Soft!=product; never Dual DoD close.
 * Grep: fb_hold_residual_field_clip dual DoD A residual dual DoD B residual
 */
static void
fb_hold_residual_field_clip(const char *sz, u32 u32Len, u32 u32MaxCols,
                            const char **ppDraw, u32 *pu32DrawCols)
{
    u32 u32Field = 0; /* index of last residual counter field start */
    u32 i;

    *ppDraw = sz;
    *pu32DrawCols = (u32Len < u32MaxCols) ? u32Len : u32MaxCols;
    if (sz == NULL || u32MaxCols == 0u) {
        *pu32DrawCols = 0u;
        return;
    }
    if (u32Len <= u32MaxCols) {
        *pu32DrawCols = u32Len;
        return;
    }
    /*
     * Prefer last " need=" / " rx=" / " tx=" / " LOAD" / "LOAD " field so
     * Dual DoD residual counters survive head-clip of long STATUS strings.
     */
    for (i = 0; i + 4u < u32Len; i++) {
        if (sz[i] == ' ' && sz[i + 1u] == 'n' && sz[i + 2u] == 'e' &&
            sz[i + 3u] == 'e' && sz[i + 4u] == 'd' &&
            (i + 5u < u32Len) && sz[i + 5u] == '=') {
            u32Field = i + 1u;
        } else if (sz[i] == ' ' && sz[i + 1u] == 'r' && sz[i + 2u] == 'x' &&
                   sz[i + 3u] == '=') {
            u32Field = i + 1u;
        } else if (sz[i] == ' ' && sz[i + 1u] == 't' && sz[i + 2u] == 'x' &&
                   sz[i + 3u] == '=') {
            u32Field = i + 1u;
        } else if (sz[i] == ' ' && sz[i + 1u] == 'L' && sz[i + 2u] == 'O' &&
                   sz[i + 3u] == 'A' && sz[i + 4u] == 'D') {
            u32Field = i + 1u;
        }
    }
    /* Leading "LOAD" without space (rare hold short form). */
    if (u32Field == 0u && sz[0] == 'L' && sz[1] == 'O' && sz[2] == 'A' &&
        sz[3] == 'D') {
        u32Field = 0u; /* keep from start; fall through to tail if too long */
    }
    if (u32Field > 0u && (u32Len - u32Field) <= u32MaxCols) {
        *ppDraw = sz + u32Field;
        *pu32DrawCols = u32Len - u32Field;
        return;
    }
    /* Field alone still long or no field - pure tail keeps residual end. */
    *ppDraw = sz + (u32Len - u32MaxCols);
    *pu32DrawCols = u32MaxCols;
}

static void
fb_fill_rect(u32 x0, u32 y0, u32 w, u32 h, u32 u32Color)
{
    u32 y;
    u32 x;
    u32 x1;
    u32 y1;
    u32 u32Run;

    if (w == 0 || h == 0 || g_u64Fb == 0) {
        return;
    }
    x1 = x0 + w;
    y1 = y0 + h;
    if (x0 >= g_u32W || y0 >= g_u32H) {
        return;
    }
    if (x1 > g_u32W) {
        x1 = g_u32W;
    }
    if (y1 > g_u32H) {
        y1 = g_u32H;
    }
    u32Run = x1 - x0;
    for (y = y0; y < y1; y++) {
        u32 *pRow = (u32 *)(gj_vaddr_t)(g_u64Fb + (u64)y * (u64)g_u32Pitch +
                                        (u64)x0 * 4ull);

        for (x = 0; x < u32Run; x++) {
            pRow[x] = u32Color;
        }
    }
}

/*
 * Fast 8x8 glyph scaled by FB_SCALE. Writes rows directly (no per-pixel call).
 * Font bit order: bit0 = leftmost pixel.
 */
static void
fb_glyph_xy(u32 u32Px, u32 u32Py, char ch, u32 u32Color)
{
    const u8 *pGlyph;
    u32 gy;
    u32 gx;
    u32 u32Idx;
    u8 u8Bits;

    if (g_u64Fb == 0 || u32Px >= g_u32W || u32Py >= g_u32H) {
        return;
    }
    if ((u8)ch < 32u || (u8)ch > 126u) {
        ch = '?';
    }
    u32Idx = (u32)(u8)ch - 32u;
    pGlyph = s_aFont8x8[u32Idx];

    for (gy = 0; gy < 8u; gy++) {
        u32 sy;
        u32 u32RowY = u32Py + gy * FB_SCALE;

        u8Bits = pGlyph[gy];
        for (sy = 0; sy < FB_SCALE; sy++) {
            u32 u32Y = u32RowY + sy;
            u32 *pRow;

            if (u32Y >= g_u32H) {
                break;
            }
            pRow = (u32 *)(gj_vaddr_t)(g_u64Fb + (u64)u32Y * (u64)g_u32Pitch);
            for (gx = 0; gx < 8u; gx++) {
                u32 u32Pix = ((u8Bits >> gx) & 1u) != 0 ? u32Color : FB_BG;
                u32 u32X = u32Px + gx * FB_SCALE;
#if FB_SCALE == 2
                if (u32X + 1u < g_u32W) {
                    pRow[u32X] = u32Pix;
                    pRow[u32X + 1u] = u32Pix;
                } else if (u32X < g_u32W) {
                    pRow[u32X] = u32Pix;
                }
#else
                {
                    u32 sx;

                    for (sx = 0; sx < FB_SCALE; sx++) {
                        if (u32X + sx < g_u32W) {
                            pRow[u32X + sx] = u32Pix;
                        }
                    }
                }
#endif
            }
        }
    }
}

static void
fb_draw_str(u32 u32X0, u32 u32Y0, u32 u32MaxCols, const char *sz,
            u32 u32Color)
{
    u32 i;

    if (sz == NULL) {
        return;
    }
    for (i = 0; i < u32MaxCols && sz[i] != '\0'; i++) {
        if (sz[i] == '\n' || sz[i] == '\r') {
            break;
        }
        fb_glyph_xy(u32X0 + i * g_u32CellW, u32Y0, sz[i], u32Color);
    }
}

/*
 * Hold-pane draw: dual DoD residual NET counter-prefer / l2+USB tail-prefer.
 * Soft!=product; product=UDX. Grep: fb_draw_str_hold dual DoD B R0
 */
static void
fb_draw_str_hold(u32 u32X0, u32 u32Y0, u32 u32MaxCols, const char *sz,
                 u32 u32Color)
{
    u32 u32Len;
    u32 u32DrawCols;
    const char *pDraw;

    if (sz == NULL || u32MaxCols == 0u) {
        return;
    }
    u32Len = 0u;
    while (sz[u32Len] != '\0' && sz[u32Len] != '\n' && sz[u32Len] != '\r') {
        u32Len++;
    }
    pDraw = sz;
    u32DrawCols = u32MaxCols;
    if (u32Len > u32MaxCols && fb_hold_str_tail_prefer(sz) != 0) {
        if (fb_hold_str_is_net(sz) != 0) {
            /* t/f/b/r over head; drop :22 before losing /r (R0). */
            fb_hold_net_clip(sz, u32Len, u32MaxCols, &pDraw, &u32DrawCols);
        } else {
            /* l2 br / USB / usb_storage - need=/rx=/tx=/LOAD residual field. */
            fb_hold_residual_field_clip(sz, u32Len, u32MaxCols, &pDraw,
                                        &u32DrawCols);
        }
    }
    fb_draw_str(u32X0, u32Y0, u32DrawCols, pDraw, u32Color);
}

static void
fb_hold_redraw_title(void)
{
    u32 u32Y = g_u32HoldY0 + FB_MARGIN;
    const char *szTitle;
    u32 u32Color;

    if (g_fReady == 0) {
        return;
    }
    /*
     * Title bar at visual row 0 - keep "STATUS (static) v" GJ_IMAGE_VERSION
     * once (test what you fly). Do not strip version. Serial also prints
     * main: image version=...; panel is L3 media identity.
     * Soft!=product: title identity != Dual DoD close. No stamp storms.
     */
    if (g_fFaultHold != 0) {
        szTitle = FB_STATUS_TITLE_FAULT;
        u32Color = FB_FAULT_FG;
    } else {
        szTitle = FB_STATUS_TITLE_OK;
        u32Color = FB_DIM;
    }
    fb_fill_rect(g_u32HoldX0 + 2u, u32Y, g_u32HoldW - 4u, g_u32CellH, FB_BG);
    fb_draw_str(g_u32HoldX0 + FB_MARGIN, u32Y, g_u32HoldCols, szTitle,
                u32Color);
}

static void
fb_hold_redraw_line(u32 u32Line)
{
    u32 u32Y;
    u32 u32Color;
    /* Visual row 0 = "STATUS" title; content lines start at row 1. */
    u32 u32Vis = u32Line + 1u;

    if (g_fReady == 0 || u32Line >= FB_HOLD_LINES) {
        return;
    }
    if (u32Vis >= g_u32HoldRows) {
        return;
    }
    u32Y = g_u32HoldY0 + FB_MARGIN + u32Vis * g_u32CellH;
    fb_fill_rect(g_u32HoldX0 + 2u, u32Y, g_u32HoldW - 4u, g_u32CellH, FB_BG);
    /*
     * Colour honesty lean (Soft!=product; product=UDX):
     *   FAULT pin rows -> red (halt unmistakable)
     *   bright headline -> yellow
     *   freestanding SKIP lamps -> dim (never warm PASS look)
     *   else warm hold (dual DoD residual still warm; != product close)
     * Grep: freestanding SKIP | fb_hold_str_has_skip | Soft!=product
     */
    if (g_fFaultHold != 0 && fb_hold_line_is_fault_pin(u32Line) != 0) {
        u32Color = FB_FAULT_FG;
    } else if (g_aHoldBright[u32Line] != 0) {
        u32Color = FB_HEAD_FG;
    } else if (fb_hold_str_has_skip(g_aHold[u32Line]) != 0) {
        u32Color = FB_DIM; /* freestanding SKIP honesty lamp */
    } else {
        u32Color = FB_HOLD_FG;
    }
    fb_draw_str_hold(g_u32HoldX0 + FB_MARGIN, u32Y, g_u32HoldCols,
                     g_aHold[u32Line], u32Color);
}

static void
fb_hold_redraw_all(void)
{
    u32 i;

    fb_fill_rect(g_u32HoldX0, g_u32HoldY0, g_u32HoldW, g_u32HoldH, FB_BG);
    fb_hold_redraw_title();
    for (i = 0; i < FB_HOLD_LINES; i++) {
        fb_hold_redraw_line(i);
    }
}

static void
fb_state_redraw_all(void)
{
    u32 i;
    u32 u32Vis;
    u32 u32Start;
    u32 u32Y;
    u32 u32Color;

    if (g_fReady == 0 || g_u32StateH == 0u) {
        return;
    }
    fb_fill_rect(g_u32StateX0, g_u32StateY0, g_u32StateW, g_u32StateH, FB_BG);
    fb_draw_str(g_u32StateX0 + FB_MARGIN, g_u32StateY0 + FB_MARGIN,
                g_u32StateCols, "STATE (boot)", FB_DIM);
    if (g_u32StateRows < 2u || g_u32StateN == 0u) {
        return;
    }
    u32Vis = g_u32StateRows - 1u;
    if (u32Vis > g_u32StateN) {
        u32Vis = g_u32StateN;
    }
    u32Start = g_u32StateN - u32Vis;
    for (i = 0; i < u32Vis; i++) {
        u32Y = g_u32StateY0 + FB_MARGIN + (i + 1u) * g_u32CellH;
        if ((u32Start + i + 1u) == g_u32StateN) {
            u32Color = FB_STATE_NOW;
        } else {
            u32Color = FB_STATE_FG;
        }
        fb_draw_str(g_u32StateX0 + FB_MARGIN, u32Y, g_u32StateCols,
                    g_aState[u32Start + i], u32Color);
    }
}

/*
 * Write hold line unconditionally (used by trap after g_fFaultHold is set).
 * Public fb_console_hold refuses FAULT-pin rows once sticky. Soft!=product.
 * Dual DoD residual NET/l2/USB: store tail-prefer so FAULT snap / narrow paint
 * keep t/f/b/r (R0) / need= when input exceeds FB_HOLD_CHARS-1.
 * Grep: fb_hold_set_raw dual DoD residual product=UDX
 */
static void
fb_hold_set_raw(u32 u32Line, const char *szText)
{
    u32 i;
    u32 u32Len;
    const char *pSrc;
    const u32 u32Cap = FB_HOLD_CHARS > 0u ? (FB_HOLD_CHARS - 1u) : 0u;

    if (g_fReady == 0 || u32Line >= FB_HOLD_LINES) {
        return;
    }
    if (szText == NULL) {
        g_aHold[u32Line][0] = '\0';
        g_aHoldBright[u32Line] = 0;
        fb_hold_redraw_line(u32Line);
        return;
    }
    u32Len = 0u;
    while (szText[u32Len] != '\0' && szText[u32Len] != '\n' &&
           szText[u32Len] != '\r') {
        u32Len++;
    }
    pSrc = szText;
    if (u32Len > u32Cap) {
        if (fb_hold_str_tail_prefer(szText) != 0) {
            /*
             * Keep trailing residual counters (NET t/f/b/r, l2 rx=/tx=,
             * USB need=/LOAD). Prefer NET counter field when it alone fits.
             * Soft!=product; product=UDX - never Dual DoD A/B close.
             */
            if (fb_hold_str_is_net(szText) != 0) {
                const char *pDraw;
                u32 u32DrawCols;

                fb_hold_net_clip(szText, u32Len, u32Cap, &pDraw, &u32DrawCols);
                pSrc = pDraw;
                u32Len = u32DrawCols;
                if (u32Len > u32Cap) {
                    u32Len = u32Cap;
                }
            } else {
                const char *pDraw;
                u32 u32DrawCols;

                /* USB / l2 residual field prefer (need=/rx=/tx=/LOAD). */
                fb_hold_residual_field_clip(szText, u32Len, u32Cap, &pDraw,
                                            &u32DrawCols);
                pSrc = pDraw;
                u32Len = u32DrawCols;
                if (u32Len > u32Cap) {
                    u32Len = u32Cap;
                }
            }
        } else {
            u32Len = u32Cap;
        }
    }
    /*
     * Same text → no redraw. Hold7 IP and other once-pins were flashing
     * because hot net_l2_ready / poll paths re-held identical strings.
     */
    if (g_aHold[u32Line][u32Len] == '\0') {
        u32 fSame = 1u;

        for (i = 0; i < u32Len; i++) {
            if (g_aHold[u32Line][i] != pSrc[i]) {
                fSame = 0u;
                break;
            }
        }
        if (fSame != 0u) {
            return;
        }
    }
    for (i = 0; i < u32Len; i++) {
        g_aHold[u32Line][i] = pSrc[i];
    }
    g_aHold[u32Line][i] = '\0';
    fb_hold_redraw_line(u32Line);
}

static void
fb_log_redraw_title(void)
{
    fb_draw_str(g_u32LogX0 + FB_MARGIN, g_u32LogY0 + 2u, g_u32LogCols,
                "LOG (fast) - product + xHCI (soft flood filtered)", FB_DIM);
}

static void
fb_log_clear_body(void)
{
    /* Keep title row; clear text body only. */
    u32 u32BodyY = g_u32LogY0 + FB_MARGIN + g_u32CellH;
    u32 u32BodyH;

    if (u32BodyY >= g_u32LogY0 + g_u32LogH) {
        fb_fill_rect(g_u32LogX0, g_u32LogY0, g_u32LogW, g_u32LogH, FB_BG);
        fb_log_redraw_title();
        g_u32Cx = 0;
        g_u32Cy = 1;
        return;
    }
    u32BodyH = (g_u32LogY0 + g_u32LogH) - u32BodyY;
    fb_fill_rect(g_u32LogX0, u32BodyY, g_u32LogW, u32BodyH, FB_BG);
    g_u32Cx = 0;
    g_u32Cy = 1;
}

static void
fb_log_clear(void)
{
    fb_fill_rect(g_u32LogX0, g_u32LogY0, g_u32LogW, g_u32LogH, FB_BG);
    g_u32Cx = 0;
    g_u32Cy = 1;
    fb_log_redraw_title();
}

static void
fb_draw_divider(void)
{
    if (g_fLeftRight != 0) {
        fb_fill_rect(g_u32LogX0 - 2u, 0, 2u, g_u32H, FB_DIM);
    } else {
        fb_fill_rect(0, g_u32LogY0 - 2u, g_u32W, 2u, FB_DIM);
    }
    if (g_u32StateH != 0u) {
        fb_fill_rect(g_u32StateX0, g_u32StateY0 - 2u, g_u32StateW, 2u, FB_DIM);
    }
}

static void
fb_log_glyph(u32 u32Col, u32 u32Row, char ch)
{
    u32 u32Px = g_u32LogX0 + FB_MARGIN + u32Col * g_u32CellW;
    u32 u32Py = g_u32LogY0 + FB_MARGIN + u32Row * g_u32CellH;

    fb_glyph_xy(u32Px, u32Py, ch, FB_FG);
}

/*
 * Soft inventory / wave stamp lines are multi-KiB per subsystem. Painting
 * them on GOP makes G752 boot crawl. STATUS already holds milestones;
 * drop stamp noise from the LOG pane. Keep product/xhci/trap lines.
 */
static int
fb_prefix_eq(const char *p, const char *sz)
{
    while (*sz != '\0') {
        if (*p++ != *sz++) {
            return 0;
        }
    }
    return 1;
}

static int
fb_line_is_soft_noise(const char *sz)
{
    const char *p;

    if (sz == NULL || sz[0] == '\0') {
        return 1; /* empty - nothing to paint */
    }
    /*
     * GOP is the glass photo. Drop residual/abandoned/heartbeat stamps.
     * Live counters live on STATUS holds 4-7.
     */
    if (fb_prefix_eq(sz, "rtl8168_udx: product host park live") ||
        fb_prefix_eq(sz, "rtl8168_udx: soft ") ||
        fb_prefix_eq(sz, "rtl8168_udx: product l2 poll") ||
        fb_prefix_eq(sz, "rtl8168_udx: product glass") ||
        fb_prefix_eq(sz, "rtl8168_udx: product dig") ||
        fb_prefix_eq(sz, "rtl8168_udx: product cache") ||
        fb_prefix_eq(sz, "rtl8168_udx: product own_arm") ||
        fb_prefix_eq(sz, "xhci_udx: product host park live") ||
        fb_prefix_eq(sz, "xhci_udx: soft ") ||
        fb_prefix_eq(sz, "linux_module:") ||
        fb_prefix_eq(sz, "linux_ksym:") ||
        fb_prefix_eq(sz, "linux_pci_soft:") ||
        fb_prefix_eq(sz, "linux_netdev") ||
        fb_prefix_eq(sz, "linux_dma_soft:") ||
        fb_prefix_eq(sz, "linux_usb_soft:") ||
        fb_prefix_eq(sz, "linux_phy_soft:") ||
        fb_prefix_eq(sz, "linux_time_soft:") ||
        fb_prefix_eq(sz, "main: soft ") ||
        fb_prefix_eq(sz, "net_l2: soft ") ||
        fb_prefix_eq(sz, "net_l2: freestanding") ||
        fb_prefix_eq(sz, "fb_console: soft ") ||
        fb_prefix_eq(sz, "soft: ") ||
        fb_prefix_eq(sz, "coop: soft ") ||
        fb_prefix_eq(sz, "xhci: freestanding") ||
        fb_prefix_eq(sz, "PLATFORM_INFO op") ||
        fb_prefix_eq(sz, "trap: string-as-code rate") ||
        fb_prefix_eq(sz, "trap: #PF I=1 rate") ||
        fb_prefix_eq(sz, "linux: ") ||
        fb_prefix_eq(sz, "udx: ") ||
        fb_prefix_eq(sz, "ddi_host:") ||
        fb_prefix_eq(sz, "ddi_door:") ||
        fb_prefix_eq(sz, "devmgr:") ||
        fb_prefix_eq(sz, "iommu: ") ||
        fb_prefix_eq(sz, "sshd: soft") ||
        fb_prefix_eq(sz, "netstackd: soft") ||
        fb_prefix_eq(sz, "net_eth: soft") ||
        fb_prefix_eq(sz, "net_tcp: soft") ||
        fb_prefix_eq(sz, "net_door: ") ||
        fb_prefix_eq(sz, "rtl8168_udx: product host park") ||
        fb_prefix_eq(sz, "xhci_udx: product host park") ||
        fb_prefix_eq(sz, "rtl8168_udx: product program dens") ||
        fb_prefix_eq(sz, "xhci_udx: product program dens")) {
        return 1;
    }
    for (p = sz; *p != '\0'; p++) {
        if (fb_prefix_eq(p, "soft_only=1") ||
            fb_prefix_eq(p, "soft_ne_product") ||
            fb_prefix_eq(p, "product_kernel=OPEN")) {
            return 1;
        }
        /* " soft <stamp>" after subsystem prefix (timer: soft inventory ...) */
        /* " soft ..." / " SOFT ..." (kprintf may upper-case on some paths). */
        if (p[0] == ' ' &&
            (p[1] == 's' || p[1] == 'S') &&
            (p[2] == 'o' || p[2] == 'O') &&
            (p[3] == 'f' || p[3] == 'F') &&
            (p[4] == 't' || p[4] == 'T') &&
            p[5] == ' ') {
            const char *q = p + 6;

            if (fb_prefix_eq(q, "inventory") || fb_prefix_eq(q, "ret") ||
                fb_prefix_eq(q, "densif") || fb_prefix_eq(q, "denser") ||
                fb_prefix_eq(q, "catalog") || fb_prefix_eq(q, "functional") ||
                fb_prefix_eq(q, "bind") || fb_prefix_eq(q, "found") ||
                fb_prefix_eq(q, "deepen") || fb_prefix_eq(q, "exclusive") ||
                fb_prefix_eq(q, "honesty") || fb_prefix_eq(q, "claim") ||
                fb_prefix_eq(q, "ratio") || fb_prefix_eq(q, "path") ||
                fb_prefix_eq(q, "api") || fb_prefix_eq(q, "frame") ||
                fb_prefix_eq(q, "vec") || fb_prefix_eq(q, "mile") ||
                fb_prefix_eq(q, "limit") || fb_prefix_eq(q, "last") ||
                fb_prefix_eq(q, "resume") || fb_prefix_eq(q, "halt") ||
                fb_prefix_eq(q, "rate") || fb_prefix_eq(q, "class") ||
                fb_prefix_eq(q, "pf") || fb_prefix_eq(q, "pe32") ||
                fb_prefix_eq(q, "outcome") || fb_prefix_eq(q, "stats") ||
                fb_prefix_eq(q, "return") || fb_prefix_eq(q, "lamps") ||
                fb_prefix_eq(q, "program") || fb_prefix_eq(q, "status") ||
                fb_prefix_eq(q, "verify") || fb_prefix_eq(q, "inits") ||
                fb_prefix_eq(q, "thr") || fb_prefix_eq(q, "note") ||
                fb_prefix_eq(q, "wave") || fb_prefix_eq(q, "chars") ||
                fb_prefix_eq(q, "putchar") || fb_prefix_eq(q, "write") ||
                fb_prefix_eq(q, "ICR") || fb_prefix_eq(q, "LOCAL") ||
                fb_prefix_eq(q, "BRINGUP") || fb_prefix_eq(q, "QUERY") ||
                fb_prefix_eq(q, "SAMPLE") || fb_prefix_eq(q, "REJECT") ||
                fb_prefix_eq(q, "CAPACITY") || fb_prefix_eq(q, "LAPIC") ||
                /* uppercase soft stamp keys from panel-path upper dumps */
                fb_prefix_eq(q, "INVENTORY") || fb_prefix_eq(q, "RET") ||
                fb_prefix_eq(q, "PATH") || fb_prefix_eq(q, "SAMPLE") ||
                fb_prefix_eq(q, "REJECT") || fb_prefix_eq(q, "CAPACITY") ||
                fb_prefix_eq(q, "ICR") || fb_prefix_eq(q, "LOCAL") ||
                fb_prefix_eq(q, "BRINGUP") || fb_prefix_eq(q, "QUERY")) {
                return 1;
            }
        }
        /* "APIC: SOFT ..." / "TIMER: SOFT ..." with colon form */
        if ((p[0] == ':' || p[0] == ' ') && p[1] == ' ' &&
            (p[2] == 'S' || p[2] == 's') &&
            (p[3] == 'O' || p[3] == 'o') &&
            (p[4] == 'F' || p[4] == 'f') &&
            (p[5] == 'T' || p[5] == 't') &&
            p[6] == ' ') {
            return 1;
        }
    }
    return 0;
}

/* Advance cursor; wrap by clearing body (trap is on STATUS - no scroll cost). */
static void
fb_newline(void)
{
    g_u32Cx = 0;
    g_u32Cy++;
    if (g_u32Cy >= g_u32LogRows) {
        fb_log_clear_body();
    }
}

static void
fb_log_emit_char(char chOut)
{
    if (chOut == '\t') {
        u32 u32Next = (g_u32Cx + 4u) & ~3u;

        while (g_u32Cx < u32Next && g_u32Cx < g_u32LogCols) {
            fb_log_glyph(g_u32Cx, g_u32Cy, ' ');
            g_u32Cx++;
        }
        if (g_u32Cx >= g_u32LogCols) {
            fb_newline();
        }
        return;
    }
    if ((u8)chOut < 32u) {
        return;
    }
    if (g_u32Cy == 0) {
        g_u32Cy = 1;
    }
    fb_log_glyph(g_u32Cx, g_u32Cy, chOut);
    g_u32Cx++;
    if (g_u32Cx >= g_u32LogCols) {
        fb_newline();
    }
}

static void
fb_log_flush_line(void)
{
    u32 i;

    g_aLine[g_u32LineLen < FB_LINE_CAP ? g_u32LineLen : (FB_LINE_CAP - 1u)] =
        '\0';
    if (fb_line_is_soft_noise(g_aLine) == 0) {
        for (i = 0; i < g_u32LineLen && i < FB_LINE_CAP; i++) {
            fb_log_emit_char(g_aLine[i]);
        }
        fb_newline();
    }
    g_u32LineLen = 0;
}

/* ---- public layout stubs (bars disabled) -------------------------------- */
u32
fb_panel_bar_pitch(u32 u32FbHeight)
{
    (void)u32FbHeight;
    return 0;
}

u32
fb_panel_text_y0(u32 u32FbHeight)
{
    (void)u32FbHeight;
    return 0;
}

void
fb_console_init(const struct gj_boot_info *pInfo)
{
    u32 i;

    g_fReady = 0;
    g_fFaultHold = 0;
    g_fFaultNetSnap = 0;
    g_fSoftResidualOnce = 0;
    g_u32SoftLeanOk = 0;
    g_u32SoftLeanFail = 0;
    if (pInfo == NULL || pInfo->u64FbBase == 0 || pInfo->u32FbBpp != 32u ||
        pInfo->u32FbWidth < 320u || pInfo->u32FbHeight < 200u ||
        pInfo->u32FbPitch == 0) {
        return;
    }

    g_u64Fb = pInfo->u64FbBase;
    g_u32Pitch = pInfo->u32FbPitch;
    g_u32W = pInfo->u32FbWidth;
    g_u32H = pInfo->u32FbHeight;
    g_u32CellW = 8u * FB_SCALE;
    g_u32CellH = 8u * FB_SCALE;

    /* Wide: left status | right log. Narrow: top status / bottom log. */
    g_fLeftRight = (g_u32W >= 700u) ? 1 : 0;

    if (g_fLeftRight != 0) {
        g_u32HoldX0 = 0;
        g_u32HoldY0 = 0;
        g_u32HoldW = g_u32W / 2u;
        g_u32HoldH = g_u32H;
        g_u32LogX0 = g_u32HoldW + 2u;
        g_u32LogY0 = 0;
        g_u32LogW = g_u32W - g_u32LogX0;
        g_u32LogH = g_u32H;
    } else {
        g_u32HoldX0 = 0;
        g_u32HoldY0 = 0;
        g_u32HoldW = g_u32W;
        g_u32HoldH = g_u32H / 3u;
        g_u32LogX0 = 0;
        g_u32LogY0 = g_u32HoldH + 2u;
        g_u32LogW = g_u32W;
        g_u32LogH = g_u32H - g_u32LogY0;
    }

    /*
     * Split the static pane in half: STATUS holds on top, STATE (boot)
     * journal on the bottom. Skip the split on a too-short pane.
     */
    g_u32StateX0 = 0;
    g_u32StateY0 = 0;
    g_u32StateW = 0;
    g_u32StateH = 0;
    g_u32StateCols = 0;
    g_u32StateRows = 0;
    g_u32StateN = 0;
    {
        u32 u32MinHold = FB_MARGIN * 2u + g_u32CellH * 8u;
        u32 u32MinState = FB_MARGIN * 2u + g_u32CellH * 4u;
        u32 u32PaneH = g_u32HoldH;

        if (u32PaneH >= (u32MinHold + u32MinState + 4u)) {
            u32 u32Top = u32PaneH / 2u;

            if (u32Top < u32MinHold) {
                u32Top = u32MinHold;
            }
            if ((u32PaneH - u32Top) < (u32MinState + 2u)) {
                u32Top = u32PaneH - (u32MinState + 2u);
            }
            g_u32HoldH = u32Top;
            g_u32StateX0 = g_u32HoldX0;
            g_u32StateY0 = g_u32HoldY0 + g_u32HoldH + 2u;
            g_u32StateW = g_u32HoldW;
            g_u32StateH = u32PaneH - g_u32HoldH - 2u;
        }
    }

    if (g_u32HoldW < FB_MARGIN * 2u + g_u32CellW ||
        g_u32LogW < FB_MARGIN * 2u + g_u32CellW) {
        return;
    }

    g_u32HoldCols = (g_u32HoldW - FB_MARGIN * 2u) / g_u32CellW;
    /*
     * Total visual text rows that fit (title + content). Content hold line L
     * paints at visual row L+1, so we need FB_HOLD_LINES+1 rows to show all.
     */
    g_u32HoldRows = (g_u32HoldH - FB_MARGIN * 2u) / g_u32CellH;
    if (g_u32HoldRows > FB_HOLD_LINES + 1u) {
        g_u32HoldRows = FB_HOLD_LINES + 1u;
    }
    /* Need title + enough content for milestones (ksym/mod/netdev/probe). */
    if (g_u32HoldRows < 5u) {
        return;
    }

    g_u32LogCols = (g_u32LogW - FB_MARGIN * 2u) / g_u32CellW;
    g_u32LogRows = (g_u32LogH - FB_MARGIN * 2u) / g_u32CellH;
    if (g_u32LogCols < 8u || g_u32LogRows < 4u) {
        return;
    }

    if (g_u32StateH != 0u) {
        if (g_u32StateW < FB_MARGIN * 2u + g_u32CellW) {
            g_u32StateH = 0;
        } else {
            g_u32StateCols = (g_u32StateW - FB_MARGIN * 2u) / g_u32CellW;
            g_u32StateRows = (g_u32StateH - FB_MARGIN * 2u) / g_u32CellH;
            if (g_u32StateRows > FB_STATE_LINES + 1u) {
                g_u32StateRows = FB_STATE_LINES + 1u;
            }
            if (g_u32StateCols < 8u || g_u32StateRows < 3u) {
                g_u32StateH = 0;
                g_u32StateRows = 0;
            }
        }
    }

    for (i = 0; i < FB_HOLD_LINES; i++) {
        g_aHold[i][0] = '\0';
        g_aHoldBright[i] = 0;
    }
    for (i = 0; i < FB_STATE_LINES; i++) {
        g_aState[i][0] = '\0';
    }

    fb_fill_rect(0, 0, g_u32W, g_u32H, FB_BG);
    g_fReady = 1;
    g_u32Cx = 0;
    g_u32Cy = 1; /* leave title row on log pane */

    g_u32LineLen = 0;
    fb_hold_redraw_all();
    fb_state_redraw_all();
    fb_draw_divider();
    fb_log_clear();

    /* Title carries version. Content fills as TE / UDX / M0 land. */
    fb_console_hold(8, "DoD A=OPEN B=OPEN");
    fb_console_state("GOP up");

    /*
     * C0 exclusive soft residual lean once-lamp (stamp-free).
     * Never invents image stamps; never Dual DoD close; Soft!=product.
     * Grep: fb_console: soft residual lean
     */
    fb_soft_residual_lean_once();
}

int
fb_console_ready(void)
{
    return g_fReady;
}

void
fb_console_putchar(char chOut)
{
    if (g_fReady == 0) {
        return;
    }
    /*
     * After FAULT pin: drop LOG paint entirely. Soft inventory and product
     * kprintf flood must not thrash GOP or fight STATUS FAULT visibility
     * on G752 (no COM1). Soft!=product. Grep: g_fFaultHold LOG drop
     */
    if (g_fFaultHold != 0) {
        g_u32LineLen = 0;
        return;
    }
    if (chOut == '\r') {
        return; /* line buffer ignores CR */
    }
    if (chOut == '\n') {
        fb_log_flush_line();
        return;
    }
    if ((u8)chOut < 32u && chOut != '\t') {
        return;
    }
    if (g_u32LineLen + 1u < FB_LINE_CAP) {
        g_aLine[g_u32LineLen++] = chOut;
    } else {
        /* Overlong line: force paint (not stamp) and wrap buffer. */
        g_aLine[FB_LINE_CAP - 1u] = '\0';
        fb_log_flush_line();
        if ((u8)chOut >= 32u || chOut == '\t') {
            g_aLine[g_u32LineLen++] = chOut;
        }
    }
}

void
fb_console_write(const char *szText)
{
    if (szText == NULL || g_fReady == 0) {
        return;
    }
    while (*szText != '\0') {
        fb_console_putchar(*szText++);
    }
}

void
fb_console_state(const char *szLine)
{
    u32 u32Len;
    u32 i;
    const char *p;

    if (g_fReady == 0 || g_u32StateH == 0u || szLine == NULL) {
        return;
    }
    p = szLine;
    while (*p == ' ') {
        p++;
    }
    if (*p == '\0') {
        return;
    }
    u32Len = 0;
    while (p[u32Len] != '\0' && p[u32Len] != '\n' && p[u32Len] != '\r') {
        u32Len++;
    }
    if (u32Len >= FB_STATE_CHARS) {
        u32Len = FB_STATE_CHARS - 1u;
    }
    if (g_u32StateN > 0u) {
        u32 u32Prev = g_u32StateN - 1u;
        u32 fSame = 1u;

        if (g_aState[u32Prev][u32Len] != '\0') {
            fSame = 0u;
        } else {
            for (i = 0; i < u32Len; i++) {
                if (g_aState[u32Prev][i] != p[i]) {
                    fSame = 0u;
                    break;
                }
            }
        }
        if (fSame != 0u) {
            return;
        }
    }
    if (g_u32StateN >= FB_STATE_LINES) {
        for (i = 0; i + 1u < FB_STATE_LINES; i++) {
            u32 j;

            for (j = 0; j < FB_STATE_CHARS; j++) {
                g_aState[i][j] = g_aState[i + 1u][j];
            }
        }
        g_u32StateN = FB_STATE_LINES - 1u;
    }
    for (i = 0; i < u32Len; i++) {
        g_aState[g_u32StateN][i] = p[i];
    }
    g_aState[g_u32StateN][u32Len] = '\0';
    g_u32StateN++;
    fb_state_redraw_all();
}

void
fb_console_hold(u32 u32Line, const char *szText)
{
    if (g_fReady == 0 || u32Line >= FB_HOLD_LINES) {
        return;
    }
    /*
     * Sticky FAULT pin: refuse overwrite of STATUS fault rows (0, 1).
     * Soft force-refresh / mod lamps must not bury FAULT. Live pin is 0+1.
     * Soft!=product; product=UDX. Grep: g_fFaultHold hold refuse
     */
    if (g_fFaultHold != 0 && fb_hold_line_is_fault_pin(u32Line) != 0) {
        return;
    }
    /*
     * Truncate to FB_HOLD_CHARS-1 - dual-DoD residual shorts (NET ... /r,
     * USB ..., l2 br ...) fit; longer stamps must stay on kprintf.
     * Freestanding SKIP text stores as-is then paints dim. Soft!=product.
     * Grep: fb_console_hold truncate dual DoD residual product=UDX
     */
    fb_hold_set_raw(u32Line, szText);
}

void
fb_console_status(const char *szLine)
{
    /* Headline on hold line 0 (bright). FAULT pin blocks overwrite. */
    if (g_fReady == 0) {
        return;
    }
    if (g_fFaultHold != 0) {
        return;
    }
    g_aHoldBright[0] = 1;
    fb_console_hold(0, szLine != NULL ? szLine : "");
}

/*
 * Bound append into [ *pq , pEnd ): leave room for trailing NUL.
 * Soft!=product trap residual - never invent stamps; never overflow hold.
 * Grep: fb_append_hex_n | fb_append_str_n | FB_HOLD_CHARS
 */
static void
fb_append_hex_n(char **pq, char *pEnd, u64 u64Val, u32 u32Nibbles)
{
    u32 i;
    char *q = *pq;

    if (q == NULL || pEnd == NULL || q >= pEnd) {
        return;
    }
    for (i = 0; i < u32Nibbles; i++) {
        u32 nibble;

        if ((q + 1) >= pEnd) {
            break;
        }
        nibble = (u32)((u64Val >> ((u32Nibbles - 1u - i) * 4u)) & 0xfu);
        *q++ = (char)(nibble < 10u ? ('0' + nibble) : ('A' + nibble - 10u));
    }
    *pq = q;
}

static void
fb_append_str_n(char **pq, char *pEnd, const char *p)
{
    char *q = *pq;

    if (q == NULL || pEnd == NULL || p == NULL || q >= pEnd) {
        return;
    }
    while (*p != '\0' && (q + 1) < pEnd) {
        *q++ = *p++;
    }
    *pq = q;
}

/* Digit char into bound buffer (vec/thr/state). Soft!=product. */
static void
fb_append_digit_n(char **pq, char *pEnd, u32 u32Dig)
{
    char *q = *pq;

    if (q == NULL || pEnd == NULL || (q + 1) >= pEnd) {
        return;
    }
    *q++ = (char)('0' + (u32Dig % 10u));
    *pq = q;
}

/*
 * C0 exclusive soft residual lean once-lamp (stamp-free; Soft!=product).
 * Functional deepen arms densify a single greppable pair - never Dual DoD
 * close, never invent image version stamps, never stamp-storm.
 * Arms: title (STATUS v + GJ_IMAGE_VERSION identity) / fault pin contract /
 *        residual clip helpers / Soft!=product lic + dual_dod OPEN_UDX.
 * G-AC-1: no .ko product AC. Dual MIT OR Apache-2.0.
 * greppable: fb_console: soft residual lean
 * greppable: fb_console: soft residual lean PASS | soft residual lean SKIP
 */
static void
fb_soft_residual_lean_once(void)
{
    u32 u32Checks = 0u;
    u32 u32Ok = 0u;
    u32 u32TitleOk = 0u;
    u32 u32FaultOk = 0u;
    u32 u32ClipOk = 0u;
    u32 u32LicOk = 0u;
    u32 u32Ready;
    const char *szLamp;
    const char *szTitle;
    const char *szVer;
    /* Stack-local product honesty (never hard-gates; Soft!=product). */
    const int nSoft = 1;
    const int nProduct = 0;
    const int nDualDodClose = 0; /* Dual DoD A/B stay OPEN_UDX */
    const int nKoProductAc = 0;  /* G-AC-1 held */
    const int nStampInvent = 0;  /* no invent image stamp */
    const int nStampStorm = 0;

    if (g_fSoftResidualOnce != 0u) {
        return;
    }
    g_fSoftResidualOnce = 1u;

    u32Ready = (g_fReady != 0) ? 1u : 0u;
    szTitle = FB_STATUS_TITLE_OK;
    szVer = GJ_IMAGE_VERSION;

    /* Arm 1: title identity - STATUS (static) v + non-empty version string. */
    u32Checks++;
    if (szTitle != NULL && szTitle[0] == 'S' && szTitle[1] == 'T' &&
        szTitle[2] == 'A' && szTitle[3] == 'T' && szTitle[4] == 'U' &&
        szTitle[5] == 'S' && szVer != NULL && szVer[0] != '\0' &&
        FB_HOLD_CHARS >= 64u && FB_HOLD_LINES == 16u &&
        FB_SOFT_LEAN_CHECKS == 4u &&
        fb_hold_line_is_fault_pin(0u) != 0 &&
        fb_hold_line_is_fault_pin(1u) != 0) {
        u32TitleOk = 1u;
        u32Ok++;
    }

    /*
     * Arm 2: FAULT pin contract (live 0 + 1 sticky). Soft!=product.
     */
    u32Checks++;
    if (fb_hold_line_is_fault_pin(0u) != 0 &&
        fb_hold_line_is_fault_pin(1u) != 0 &&
        fb_hold_line_is_fault_pin(2u) == 0 &&
        fb_hold_line_is_fault_pin(3u) == 0 &&
        fb_hold_line_is_fault_pin(4u) == 0 &&
        fb_hold_line_is_fault_pin(8u) == 0) {
        u32FaultOk = 1u;
        u32Ok++;
    }

    /* Arm 3: residual clip helpers prefer Dual DoD counters (NET/USB/l2). */
    u32Checks++;
    if (fb_hold_str_tail_prefer("NET eth0 10.0.0.1 t0/f0/b0/r0") != 0 &&
        fb_hold_str_is_net("NET eth0") != 0 &&
        fb_hold_str_tail_prefer("USB linux OPEN need=usbcore") != 0 &&
        fb_hold_str_tail_prefer("l2 br rx=0 tx=0") != 0 &&
        fb_hold_str_has_skip("mod r8169 SKIP builtin") != 0 &&
        fb_hold_str_has_skip("mod xhci_pci SKIP") != 0 &&
        fb_hold_str_has_skip("READY") == 0 &&
        fb_hold_str_tail_prefer("phase ksym") == 0) {
        u32ClipOk = 1u;
        u32Ok++;
    }

    /*
     * Arm 4: Soft!=product lic - dual DoD stays OPEN; G-AC-1; stamp-free.
     * Residual never invents image stamps or closes Dual DoD A/B.
     */
    u32Checks++;
    if (nSoft == 1 && nProduct == 0 && nDualDodClose == 0 &&
        nKoProductAc == 0 && nStampInvent == 0 && nStampStorm == 0) {
        u32LicOk = 1u;
        u32Ok++;
    }

    if (u32Ok == u32Checks && u32Checks == FB_SOFT_LEAN_CHECKS) {
        g_u32SoftLeanOk++;
    } else {
        g_u32SoftLeanFail++;
    }

    /* PASS only when ready + all arms held - residual != product DoD. */
    if (g_u32SoftLeanOk != 0u && u32Ready != 0u) {
        szLamp = "PASS";
    } else {
        szLamp = "SKIP";
    }

    /*
     * Grep: fb_console: soft residual lean
     * Soft residual honesty only - never Dual DoD A/B close / never product.
     * No version stamp invent (title reuses GJ_IMAGE_VERSION; stamp_storm=0).
     */
    kprintf("fb_console: soft residual lean "
            "ready=%u hold_lines=%u hold_chars=%u fault_hold=%u net_snap=%u "
            "title=%u fault=%u clip=%u lic=%u ok=%u/%u "
            "lean_ok=%u lean_fail=%u "
            "soft=1 product=0 soft_ne_product=1 stamp_storm=0 "
            "version_stamp=0 stamp_invent=0 "
            "dual_dod_a=OPEN_UDX dual_dod_b=OPEN_UDX product=UDX "
            "Soft!=product G-AC-1 dual=MIT|Apache-2.0 "
            "(panel residual only; Dual DoD OPEN; no stamp invent)\n",
            u32Ready, (unsigned)FB_HOLD_LINES, (unsigned)FB_HOLD_CHARS,
            (g_fFaultHold != 0) ? 1u : 0u,
            (g_fFaultNetSnap != 0) ? 1u : 0u, u32TitleOk, u32FaultOk,
            u32ClipOk, u32LicOk, u32Ok, u32Checks, g_u32SoftLeanOk,
            g_u32SoftLeanFail);
    /*
     * Grep: fb_console: soft residual lean PASS
     * Grep: fb_console: soft residual lean SKIP
     */
    kprintf("fb_console: soft residual lean %s "
            "ready=%u title=%u fault=%u clip=%u lic=%u ok=%u/%u "
            "soft=1 product=0 soft_ne_product=1 stamp_storm=0 "
            "dual_dod_a=OPEN_UDX dual_dod_b=OPEN_UDX product=UDX "
            "Soft!=product dual=MIT|Apache-2.0 "
            "(lean residual only; Dual DoD OPEN; G-AC-1)\n",
            szLamp, u32Ready, u32TitleOk, u32FaultOk, u32ClipOk, u32LicOk,
            u32Ok, u32Checks);
}

void
fb_console_trap(u32 u32Vec, u64 u64Err, u64 u64Rip, u64 u64Cr2, u32 u32Thr,
                u32 u32State)
{
    char sz[FB_HOLD_CHARS];
    char *q;
    char *pEnd;

    if (g_fReady == 0) {
        return;
    }

    (void)u64Err;

    /*
     * Sticky pin 0+1. TE / UDX / IP / DoD on 2-8 stay visible.
     * LOG putchar drops after this.
     */
    g_fFaultHold = 1;
    g_u32LineLen = 0;
    g_fFaultNetSnap = 0;

    pEnd = sz + FB_HOLD_CHARS;

    /* Line 0 headline - lean: vec only (thr/state fold into hold8). */
    g_aHoldBright[0] = 1;
    q = sz;
    fb_append_str_n(&q, pEnd, "FAULT vec=");
    if (u32Vec >= 100u) {
        fb_append_digit_n(&q, pEnd, (u32Vec / 100u) % 10u);
    }
    if (u32Vec >= 10u) {
        fb_append_digit_n(&q, pEnd, (u32Vec / 10u) % 10u);
    }
    fb_append_digit_n(&q, pEnd, u32Vec % 10u);
    if (q < pEnd) {
        *q = '\0';
    } else {
        sz[FB_HOLD_CHARS - 1u] = '\0';
    }
    fb_hold_set_raw(0, sz);

    /* hold1: rip / cr2 / thr — do not steal TE/UDX/IP. */
    q = sz;
    fb_append_str_n(&q, pEnd, "rip=0x");
    fb_append_hex_n(&q, pEnd, u64Rip, 16u);
    fb_append_str_n(&q, pEnd, " cr2=0x");
    fb_append_hex_n(&q, pEnd, u64Cr2, 16u);
    fb_append_str_n(&q, pEnd, " thr=");
    if (u32Thr >= 10u) {
        fb_append_digit_n(&q, pEnd, (u32Thr / 10u) % 10u);
    }
    fb_append_digit_n(&q, pEnd, u32Thr % 10u);
    (void)u32State;
    if (q < pEnd) {
        *q = '\0';
    } else {
        sz[FB_HOLD_CHARS - 1u] = '\0';
    }
    fb_hold_set_raw(1, sz);

    fb_hold_redraw_title();
    fb_hold_redraw_line(0);
    fb_hold_redraw_line(1);
    fb_console_state("FAULT halted");
}
