/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * GOP split-screen text console:
 *   hold pane (static milestones) | log pane (kprintf stream)
 *
 * Panel-path performance (G752, no COM1):
 *  - Direct row stores for glyphs (no per-pixel helper)
 *  - Full-pane clear + wrap instead of pixel-scroll (scroll was O(W*H)/line)
 *  - Soft-inventory stamp lines dropped on LOG (STATUS holds milestones)
 * Trap detail is pinned on STATUS via fb_console_trap — safe to wrap LOG.
 */
#include <gj/boot_info.h>
#include <gj/fb_console.h>
#include <gj/string.h>
#include <gj/types.h>

#include "fb_console_font.inc"

#define FB_FG        0x00D0D0D0u /* scroll text */
#define FB_BG        0x00000000u
#define FB_HOLD_FG   0x00FFE080u /* warm status */
#define FB_HEAD_FG   0x00FFFF00u /* headline */
#define FB_DIM       0x00606060u /* divider / labels */
#define FB_SCALE     2u          /* 8x8 → 16x16 */
#define FB_MARGIN    6u
#define FB_LINE_CAP  160u        /* line buffer for soft-noise filter */

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
static u32 g_u32Cx;
static u32 g_u32Cy;
static int g_fReady;

/* Static hold buffer (NUL-terminated, truncated). */
static char g_aHold[FB_HOLD_LINES][96];
static u8 g_aHoldBright[FB_HOLD_LINES]; /* 1 = headline yellow */

/* Pending LOG line (filter soft inventory before painting). */
static char g_aLine[FB_LINE_CAP];
static u32 g_u32LineLen;

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
 * Fast 8×8 glyph scaled by FB_SCALE. Writes rows directly (no per-pixel call).
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
    u32Color = g_aHoldBright[u32Line] != 0 ? FB_HEAD_FG : FB_HOLD_FG;
    fb_draw_str(g_u32HoldX0 + FB_MARGIN, u32Y, g_u32HoldCols, g_aHold[u32Line],
                u32Color);
}

static void
fb_hold_redraw_all(void)
{
    u32 i;

    fb_fill_rect(g_u32HoldX0, g_u32HoldY0, g_u32HoldW, g_u32HoldH, FB_BG);
    /* Title bar at visual row 0 */
    fb_draw_str(g_u32HoldX0 + FB_MARGIN,
                g_u32HoldY0 + FB_MARGIN, g_u32HoldCols, "STATUS (static)",
                FB_DIM);
    for (i = 0; i < FB_HOLD_LINES; i++) {
        fb_hold_redraw_line(i);
    }
}

static void
fb_log_redraw_title(void)
{
    fb_draw_str(g_u32LogX0 + FB_MARGIN, g_u32LogY0 + 2u, g_u32LogCols,
                "LOG (fast) — product + xHCI (soft flood filtered)", FB_DIM);
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
        return 1; /* empty — nothing to paint */
    }
    for (p = sz; *p != '\0'; p++) {
        if (fb_prefix_eq(p, "soft_only=1") ||
            fb_prefix_eq(p, "soft_ne_product") ||
            fb_prefix_eq(p, "product_kernel=OPEN")) {
            return 1;
        }
        /* " soft <stamp>" after subsystem prefix (timer: soft inventory …) */
        /* " soft …" / " SOFT …" (kprintf may upper-case on some paths). */
        if (p[0] == ' ' &&
            (p[1] == 's' || p[1] == 'S') &&
            (p[2] == 'o' || p[2] == 'O') &&
            (p[3] == 'f' || p[3] == 'F') &&
            (p[4] == 't' || p[4] == 'T') &&
            p[5] == ' ') {
            const char *q = p + 6;

            if (fb_prefix_eq(q, "inventory") || fb_prefix_eq(q, "ret") ||
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
        /* "APIC: SOFT …" / "TIMER: SOFT …" with colon form */
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

/* Advance cursor; wrap by clearing body (trap is on STATUS — no scroll cost). */
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

    for (i = 0; i < FB_HOLD_LINES; i++) {
        g_aHold[i][0] = '\0';
        g_aHoldBright[i] = 0;
    }

    fb_fill_rect(0, 0, g_u32W, g_u32H, FB_BG);
    g_fReady = 1;
    g_u32Cx = 0;
    g_u32Cy = 1; /* leave title row on log pane */

    g_u32LineLen = 0;
    fb_hold_redraw_all();
    fb_draw_divider();
    fb_log_clear();

    fb_console_hold(0, "GreenJade panel (text-first)");
    g_aHoldBright[0] = 1;
    fb_hold_redraw_line(0);
    fb_console_hold(1, "hold=static milestones");
    fb_console_hold(2, "log=product+xhci (soft filtered)");
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
fb_console_hold(u32 u32Line, const char *szText)
{
    u32 i;

    if (g_fReady == 0 || u32Line >= FB_HOLD_LINES) {
        return;
    }
    if (szText == NULL) {
        g_aHold[u32Line][0] = '\0';
        g_aHoldBright[u32Line] = 0;
        fb_hold_redraw_line(u32Line);
        return;
    }
    for (i = 0; i < sizeof(g_aHold[0]) - 1u && szText[i] != '\0'; i++) {
        if (szText[i] == '\n' || szText[i] == '\r') {
            break;
        }
        g_aHold[u32Line][i] = szText[i];
    }
    g_aHold[u32Line][i] = '\0';
    /* Shift content down one visual row so title "STATUS" stays free:
     * we draw hold buffer line N at visual row N+1 in redraw — already
     * handled: redraw_line uses u32Line as visual index starting 0 under title.
     * Title is drawn at Y+2; line 0 at first content row. OK. */
    fb_hold_redraw_line(u32Line);
}

void
fb_console_status(const char *szLine)
{
    /* Headline on hold line 0 (bright). */
    if (g_fReady == 0) {
        return;
    }
    g_aHoldBright[0] = 1;
    fb_console_hold(0, szLine != NULL ? szLine : "");
}

/* Append hex digits of u64Val (u32Nibbles high→low) into *pq; advances *pq. */
static void
fb_append_hex(char **pq, u64 u64Val, u32 u32Nibbles)
{
    u32 i;
    char *q = *pq;

    for (i = 0; i < u32Nibbles; i++) {
        u32 nibble = (u32)((u64Val >> ((u32Nibbles - 1u - i) * 4u)) & 0xfu);

        *q++ = (char)(nibble < 10u ? ('0' + nibble) : ('A' + nibble - 10u));
    }
    *pq = q;
}

static void
fb_append_str(char **pq, const char *p)
{
    char *q = *pq;

    while (*p != '\0') {
        *q++ = *p++;
    }
    *pq = q;
}

void
fb_console_trap(u32 u32Vec, u64 u64Err, u64 u64Rip, u64 u64Cr2, u32 u32Thr,
                u32 u32State)
{
    char sz[96];
    char *q;

    if (g_fReady == 0) {
        return;
    }

    /* Line 0 headline — pin so scroll cannot hide the fault. */
    g_aHoldBright[0] = 1;
    q = sz;
    fb_append_str(&q, "FAULT vec=");
    if (u32Vec >= 100u) {
        *q++ = (char)('0' + (u32Vec / 100u) % 10u);
    }
    if (u32Vec >= 10u) {
        *q++ = (char)('0' + (u32Vec / 10u) % 10u);
    }
    *q++ = (char)('0' + (u32Vec % 10u));
    *q = '\0';
    fb_console_hold(0, sz);

    /* Keep xHCI stage lines 2–4; put trap detail on 6–10. */
    q = sz;
    fb_append_str(&q, "err=0x");
    fb_append_hex(&q, u64Err, 16u);
    *q = '\0';
    fb_console_hold(6, sz);

    q = sz;
    fb_append_str(&q, "rip=0x");
    fb_append_hex(&q, u64Rip, 16u);
    *q = '\0';
    fb_console_hold(7, sz);

    q = sz;
    fb_append_str(&q, "cr2=0x");
    fb_append_hex(&q, u64Cr2, 16u);
    *q = '\0';
    fb_console_hold(8, sz);

    q = sz;
    fb_append_str(&q, "thr=");
    if (u32Thr >= 10u) {
        *q++ = (char)('0' + (u32Thr / 10u) % 10u);
    }
    *q++ = (char)('0' + (u32Thr % 10u));
    fb_append_str(&q, " state=");
    if (u32State >= 10u) {
        *q++ = (char)('0' + (u32State / 10u) % 10u);
    }
    *q++ = (char)('0' + (u32State % 10u));
    *q = '\0';
    fb_console_hold(9, sz);

    g_aHoldBright[10] = 1;
    /* ASCII only — 8x8 font has no em-dash (was rendering as ???). */
    fb_console_hold(10, "KERNEL FAULT - HALTED");
}
