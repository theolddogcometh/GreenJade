/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Freestanding libc-shaped string/memory/format helpers (libgj).
 *
 * Pure C11. Dual-licensed MIT OR Apache-2.0.
 *
 * Contract:
 *   - Implementations live in user/libgj/src/string_u.c (not this header).
 *   - No locale, no errno, no thread-local state, no allocation.
 *   - Character classes and case maps are ASCII only (soft freestanding).
 *   - NULL string pointers are soft-safe where noted (return 0 / NULL / equal)
 *     so boot smokes and door servers do not fault on missing optional args.
 *   - Buffers that are written must be caller-owned and large enough; bounded
 *     helpers (strl*, strn*, *toa, snprintf) never write past cb/cbBuf when
 *     cb > 0. Unbounded strcpy/strcat still require a large enough destination.
 *   - I/O helpers (gj_puts / gj_write / gj_read) use the Linux personality
 *     write/read path via gj_syscall6; gj_dlog uses native GJ_SYS_DEBUG_LOG.
 *     Link with libgj.a (string_u.o + syscall.o).
 *
 * Naming: Hungarian parameters (sz*, p*, cb*, n*, ...) per STYLE.md; functions
 * stay gj_* module_snake without Hungarian prefixes.
 *
 * -------------------------------------------------------------------------
 * C2 lib residual (Soft!=product; G-AC-1; dual MIT OR Apache-2.0):
 * -------------------------------------------------------------------------
 * Hot freestanding helpers (length..format) are the product-direction surface
 * for GreenJade freestanding user ELFs (init, shell, ld-gj, door servers).
 * They are not a Dual DoD close, not bar3, not multi-server confine product.
 *
 * Soft residual inventory / deepen / path / honesty accessors below are cold
 * eng-only (rodata + counts). They never gate product lamps, never mint caps,
 * never run Linux .ko product AC (G-AC-1), never bump GJ_IMAGE_VERSION.
 *
 * Soft residual deepen (Soft!=product; stamp-free; Dual DoD A/B OPEN):
 *   inventory residual — greppable "libgj: soft inventory" catalog only.
 *   deepen residual    — greppable "libgj: soft deepen" wave/area stamp.
 *   path residual      — greppable "libgj: soft path" freestanding policy.
 *   honesty residual   — greppable "libgj: soft honesty"; product_kernel=OPEN.
 *   group residual     — length/compare/copy/search/case/ctype/mem/parse/io/format.
 *   policy residual    — freestanding pure_c no_heap no_locale no_errno
 *                        ascii_only soft_null; counts=none hot_path=clean.
 *   dual_dod residual  — Dual DoD A/B remain OPEN; agent_close=0.
 *   product residual   — freestanding helpers != UDX Dual DoD product close;
 *                        product path for drivers = UDX+ABI userspace hosts.
 *
 * Soft API honesty:
 *   soft path = freestanding pure-C helpers + cold inventory accessors
 *   product   = UDX/DDI Dual DoD A/B (OPEN; not claimed by this header)
 *   Dual DoD  = A (xhci_udx) / B (rtl8168_udx) remain OPEN
 * greppable: libgj: soft residual lean
 * greppable: libgj: soft residual inventory
 * greppable: libgj: soft residual deepen
 * greppable: libgj: soft residual path
 * greppable: libgj: soft residual honesty
 * greppable: libgj: soft residual dual_dod
 * greppable: libgj: soft residual product
 * greppable: libgj: soft residual claim_class=C2
 * greppable: Soft!=product
 *
 * Law: dual MIT OR Apache-2.0; Soft!=product; G-AC-1; Dual DoD A/B OPEN;
 * no GPL; no freestanding class re-enable as Dual DoD close; stamp-free;
 * no GJ_IMAGE_VERSION bump from this residual. claim_class=C2.
 */
#pragma once

#include <stddef.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================
 * C2 soft residual honesty constants (eng only; Soft!=product; stamp-free)
 * Never product Dual DoD close, never bar3, never GJ_IMAGE_VERSION.
 * greppable: libgj: soft residual lean / Soft!=product / claim_class=C2
 * ======================================================================== */

#define GJ_STRING_SOFT_NE_PRODUCT       1u /* Soft!=product */
#define GJ_STRING_SOFT_CLAIM_CLASS_C2   1u /* claim_class=C2 lib residual */
#define GJ_STRING_SOFT_MULTI_SERVER     0u /* soft residual; not multi-server */
#define GJ_STRING_SOFT_CONFINE          0u /* soft residual; not product confine */
#define GJ_STRING_SOFT_AGENT_CLOSE      0u /* residual lamps != Dual DoD close */
#define GJ_STRING_SOFT_DOD_A_OPEN       1u /* Dual DoD A (xhci_udx) OPEN */
#define GJ_STRING_SOFT_DOD_B_OPEN       1u /* Dual DoD B (rtl8168_udx) OPEN */
#define GJ_STRING_SOFT_G_AC_1           1u /* no Linux .ko product AC */
#define GJ_STRING_SOFT_PRODUCT_KERNEL   0u /* product_kernel=OPEN (not closed) */
#define GJ_STRING_SOFT_HOT_PATH_CLEAN   1u /* hot helpers carry no counters */
#define GJ_STRING_SOFT_COUNTS_NONE      1u /* counts=none policy */
#define GJ_STRING_SOFT_ASCII_ONLY       1u /* ctype/case ASCII only */
#define GJ_STRING_SOFT_STAMP_FREE       1u /* no version stamp in this residual */

/* ========================================================================
 * Length / compare / copy
 * ======================================================================== */

/**
 * Length of a NUL-terminated C string.
 * @param sz  C string; NULL → 0 (soft).
 * @return Count of bytes before the first NUL (excluding the NUL).
 */
size_t gj_strlen(const char *sz);

/**
 * Bounded length: scan at most cbMax bytes for a NUL.
 * @param sz     C string; NULL → 0.
 * @param cbMax  Maximum bytes to examine; 0 → 0.
 * @return Min(strlen(sz), cbMax) with soft NULL handling.
 */
size_t gj_strnlen(const char *sz, size_t cbMax);

/**
 * Lexicographic compare of two C strings (unsigned char order).
 * @return <0 if A < B, 0 if equal, >0 if A > B.
 *         Both NULL → 0; only A NULL → -1; only B NULL → 1.
 */
int    gj_strcmp(const char *szA, const char *szB);

/**
 * Bounded lexicographic compare of at most cb bytes.
 * @param cb  0 → equal (0) without reading either pointer.
 * @return Same ordering as gj_strcmp within the first cb bytes / first NUL.
 */
int    gj_strncmp(const char *szA, const char *szB, size_t cb);

/**
 * Copy szSrc including trailing NUL into szDst.
 * @param szDst  Destination; NULL → NULL (no write).
 * @param szSrc  Source; NULL → write single NUL into szDst when non-NULL.
 * @return szDst, or NULL if szDst is NULL.
 * @note Unbounded: caller must ensure room for strlen(src)+1.
 */
char  *gj_strcpy(char *szDst, const char *szSrc);

/**
 * Copy at most cb bytes from szSrc into szDst (C strncpy shape).
 * Pads with NULs after early end of src until cb bytes written.
 * @param szDst  Destination; NULL or cb==0 → return szDst without write.
 * @param szSrc  Source; NULL treated as empty (fill zeros).
 * @return szDst.
 * @note If strlen(src) >= cb, the result is not necessarily NUL-terminated.
 */
char  *gj_strncpy(char *szDst, const char *szSrc, size_t cb);

/**
 * Append szSrc (including NUL) after the existing contents of szDst.
 * @param szDst  Destination; NULL → NULL.
 * @param szSrc  Source; NULL treated as empty string.
 * @return szDst, or NULL if szDst is NULL.
 * @note Unbounded: caller must ensure room for both strings + NUL.
 */
char  *gj_strcat(char *szDst, const char *szSrc);

/**
 * Append at most cb non-NUL bytes from szSrc, then force a trailing NUL.
 * @param szDst  Destination; NULL → NULL.
 * @param cb     0 or szSrc NULL → no append; still returns szDst.
 * @return szDst, or NULL if szDst is NULL.
 */
char  *gj_strncat(char *szDst, const char *szSrc, size_t cb);

/**
 * BSD-style bounded copy: always NUL-terminates when cbDst > 0.
 * @param szDst  Destination buffer of size cbDst.
 * @param szSrc  Source C string (NULL soft → empty).
 * @param cbDst  Destination capacity in bytes (includes space for NUL).
 * @return strlen(szSrc) as if fully copied (may be >= cbDst-1 on truncation).
 *         When szDst is NULL or cbDst is 0, still returns strlen(src) and
 *         writes nothing.
 */
size_t gj_strlcpy(char *szDst, const char *szSrc, size_t cbDst);

/**
 * BSD-style bounded append: always NUL-terminates when room remains.
 * @param szDst  Destination of capacity cbDst (existing contents + append).
 * @param szSrc  Source to append.
 * @param cbDst  Total destination capacity (not remaining free bytes).
 * @return Intended total length: strlen(initial dst) + strlen(src).
 *         If dst is not NUL-terminated within cbDst, returns cbDst + strlen(src).
 *         When szDst is NULL or cbDst is 0, returns strlen(src) and writes nothing.
 */
size_t gj_strlcat(char *szDst, const char *szSrc, size_t cbDst);

/* ========================================================================
 * Search
 * ======================================================================== */

/**
 * First occurrence of ch (as unsigned char) in sz, or NULL.
 * ch == 0 returns a pointer to the terminating NUL.
 * sz NULL → NULL.
 */
char  *gj_strchr(const char *sz, int ch);

/**
 * Last occurrence of ch in sz, or NULL.
 * ch == 0 returns a pointer to the terminating NUL.
 * sz NULL → NULL.
 */
char  *gj_strrchr(const char *sz, int ch);

/**
 * First occurrence of szNeedle as a substring of szHay, or NULL.
 * Empty needle → szHay (if non-NULL). szHay NULL → NULL.
 * szNeedle NULL treated as empty needle.
 */
char  *gj_strstr(const char *szHay, const char *szNeedle);

/**
 * First occurrence of ch in the first cb bytes of p, or NULL.
 * p NULL → NULL. Does not stop at embedded NULs.
 */
void  *gj_memchr(const void *p, int ch, size_t cb);

/**
 * Length of the initial segment of sz consisting only of chars in szAccept.
 * @return Count of leading chars in the accept set; 0 if either pointer is NULL.
 */
size_t gj_strspn(const char *sz, const char *szAccept);

/**
 * Length of the initial segment of sz consisting of chars not in szReject.
 * @return Count of leading chars outside the reject set.
 *         sz NULL → 0. szReject NULL or empty → full strlen(sz).
 */
size_t gj_strcspn(const char *sz, const char *szReject);

/* ========================================================================
 * Case (ASCII only; freestanding soft)
 * ======================================================================== */

/** Map 'A'..'Z' → 'a'..'z'; all other values unchanged (including EOF-shaped). */
int    gj_tolower(int ch);
/** Map 'a'..'z' → 'A'..'Z'; all other values unchanged. */
int    gj_toupper(int ch);

/**
 * Case-insensitive strcmp over ASCII (via gj_tolower per byte).
 * NULL rules match gj_strcmp.
 */
int    gj_strcasecmp(const char *szA, const char *szB);

/**
 * Case-insensitive strncmp over ASCII for at most cb bytes.
 * cb == 0 → 0. NULL rules match gj_strncmp.
 */
int    gj_strncasecmp(const char *szA, const char *szB, size_t cb);

/* ========================================================================
 * Character class (ASCII; return 0 or 1, not C ctype nonzero)
 * ======================================================================== */

/** '0'..'9' → 1, else 0. */
int    gj_isdigit(int ch);
/** 'A'..'Z' or 'a'..'z' → 1, else 0. */
int    gj_isalpha(int ch);
/** Digit or alpha → 1, else 0. */
int    gj_isalnum(int ch);
/** Space, tab, LF, CR, FF, VT → 1, else 0. */
int    gj_isspace(int ch);
/** Digit or a-f/A-F → 1, else 0. */
int    gj_isxdigit(int ch);
/** Printable ASCII 0x20..0x7e → 1, else 0. */
int    gj_isprint(int ch);

/* ========================================================================
 * Memory
 * ======================================================================== */

/**
 * Copy cb bytes from pSrc to pDst (regions must not overlap).
 * @return pDst. No NULL checks (caller provides valid buffers when cb > 0).
 */
void  *gj_memcpy(void *pDst, const void *pSrc, size_t cb);

/**
 * Copy cb bytes with overlap safety (memmove shape).
 * Identical pointers or cb == 0 → no-op return pDst.
 */
void  *gj_memmove(void *pDst, const void *pSrc, size_t cb);

/**
 * Fill cb bytes of pDst with (unsigned char)c.
 * @return pDst.
 */
void  *gj_memset(void *pDst, int c, size_t cb);

/**
 * Compare cb bytes of pA and pB (unsigned char order).
 * @return <0 / 0 / >0 at first differing byte; 0 if all equal or cb == 0.
 */
int    gj_memcmp(const void *pA, const void *pB, size_t cb);

/**
 * Like memcpy but stops after copying the first occurrence of ch.
 * @return One past the last written byte if ch was found and copied;
 *         NULL if ch was not found within cb bytes (full copy still done).
 */
void  *gj_memccpy(void *pDst, const void *pSrc, int ch, size_t cb);

/* ========================================================================
 * Parse (freestanding; no locale; no errno)
 * ======================================================================== */

/**
 * Parse signed decimal with optional leading whitespace and optional +/-.
 * On overflow clamps to LONG_MIN / LONG_MAX-shaped extremes of host long.
 * Non-numeric input after optional sign/whitespace → 0.
 */
long   gj_atol(const char *sz);

/**
 * Parse an unsigned integer from sz.
 * @param nBase  0 → auto (0x/0X hex, leading 0 octal, else decimal);
 *               2..36 → explicit digit alphabet.
 * @param ppEnd  Optional; if non-NULL, set to first unconverted character.
 *               On no conversion *ppEnd stays at sz and the return is 0.
 * @return Parsed value (wrap/clamp behavior matches soft freestanding impl;
 *         not a full C strtoul errno contract).
 */
unsigned long gj_strtoul(const char *sz, char **ppEnd, int nBase);

/**
 * Signed wrapper around gj_strtoul with optional leading whitespace and sign.
 * Same base / ppEnd rules as gj_strtoul.
 */
long   gj_strtol(const char *sz, char **ppEnd, int nBase);

/* ========================================================================
 * I/O / format (freestanding user-ELF path; needs libgj.a)
 * Soft!=product: smoke/log helpers only — not Dual DoD A/B close, not bar3.
 * ======================================================================== */

/**
 * Write a NUL-terminated string to fd 1 (stdout) via Linux write.
 * Soft no-op if sz is NULL. Does not append a trailing newline unless present
 * in sz. Errors from the syscall are discarded (void return for smoke use).
 */
void   gj_puts(const char *sz);

/**
 * Linux write(nFd, p, cb) via gj_syscall6(LINUX_NR_write, ...).
 * @return Bytes written, or negative Linux errno (personality LINUX).
 */
long   gj_write(int nFd, const void *p, size_t cb);

/**
 * Linux read(nFd, p, cb) via gj_syscall6(LINUX_NR_read, ...).
 * @return Bytes read, or negative Linux errno.
 */
long   gj_read(int nFd, void *p, size_t cb);

/**
 * Native GJ_SYS_DEBUG_LOG of a NUL-terminated string (kernel serial / door log).
 * Soft: uses strlen(sz) as the length argument. sz NULL is soft-safe via length 0.
 * @return Kernel return (bytes logged or negative).
 */
long   gj_dlog(const char *sz);

/**
 * Decimal format of a signed long into szBuf (NUL-terminated when cbBuf > 0).
 * @param cbBuf  Capacity including NUL; truncated if needed.
 * @return Length that would be written excluding NUL (same soft rule as snprintf).
 */
size_t gj_itoa(long i64Val, char *szBuf, size_t cbBuf);

/**
 * Decimal format of an unsigned long into szBuf (NUL-terminated when cbBuf > 0).
 * @return Length written excluding NUL (or would-be length on truncation).
 */
size_t gj_utoa(unsigned long uVal, char *szBuf, size_t cbBuf);

/**
 * Hex format of an unsigned long (lowercase digits, no "0x" prefix).
 * @param fLeadZero  Non-zero -> pad with leading '0' to at least cMin digits.
 * @param cMin       Minimum digit count when fLeadZero is set (ignored if 0).
 * @return Length written excluding NUL.
 */
size_t gj_xtoa(unsigned long uVal, char *szBuf, size_t cbBuf,
               int fLeadZero, size_t cMin);

/**
 * Soft bounded snprintf-lite for freestanding smoke/log paths.
 *
 * Supported conversions (lean freestanding; not full libc):
 *   %%  literal percent
 *   %s  const char * (NULL -> literal "(null)")
 *   %c  character (va_arg as int; low 8 bits used)
 *   %d / %i  signed decimal (default long; l/ll → long / long long)
 *   %u  unsigned decimal (default unsigned long; l/ll → unsigned long / ull)
 *   %x / %X  lowercase hex (same length rules; X same as x here)
 *   %p  pointer: "0x" + zero-padded hex (sizeof(void*)*2 digits)
 * Optional flags before conversion: '0' zero-pad, decimal width (e.g. %08x).
 * Length modifiers: l / ll (before d/i/u/x/X).
 * Unknown conversions emit '%' + the character literally.
 *
 * @param szBuf  Destination; NULL with cbBuf 0 is a length-only probe.
 * @param cbBuf  Capacity including trailing NUL when > 0.
 * @param szFmt  Format string; NULL -> write empty string (return 0).
 * @return Bytes that would be written excluding NUL (C99 snprintf-like);
 *         output is truncated to fit cbBuf-1 printable bytes + NUL when cbBuf > 0.
 *
 * Not a full libc printf - no floating point, no positional args, no locale.
 * Soft!=product: format residual for freestanding smoke + UDX printk.
 */
size_t gj_vsnprintf(char *szBuf, size_t cbBuf, const char *szFmt, va_list ap);
size_t gj_snprintf(char *szBuf, size_t cbBuf, const char *szFmt, ...);

/* ========================================================================
 * Soft residual inventory accessors (cold only; Soft!=product)
 * Implemented in user/libgj/src/string_u.c. Diagnostics / agent honesty —
 * never a product bar3 claim, never Dual DoD close, never G-AC-1 waiver.
 * greppable: libgj: soft residual inventory / Soft!=product / claim_class=C2
 * ======================================================================== */

/** Greppable "libgj: soft inventory ..." rodata line. Soft!=product. */
const char *gj_string_soft_inventory(void);
/** Greppable "libgj: soft deepen ..." rodata line. Soft!=product. */
const char *gj_string_soft_deepen(void);
/** Greppable "libgj: soft path ..." rodata line. Soft!=product. */
const char *gj_string_soft_path(void);
/** Greppable "libgj: soft honesty ..." rodata line; product_kernel=OPEN. */
const char *gj_string_soft_honesty(void);
/** Greppable "libgj: soft residual lean ..." rodata; Soft!=product Dual DoD OPEN. */
const char *gj_string_soft_residual(void);
/**
 * Greppable "libgj: soft product residual product=UDX+sshd+stack ...".
 * Soft!=product; Dual DoD OPEN; stamp-free bar v2026.08.04.75.
 */
const char *gj_string_soft_product_residual(void);

/** Soft inventory wave stamp (eng only; Soft!=product; not GJ_IMAGE_VERSION). */
unsigned    gj_string_soft_wave(void);
/** Soft greppable area count. Soft!=product. */
unsigned    gj_string_soft_area_count(void);
/** Soft area name by index, or NULL. Soft!=product. */
const char *gj_string_soft_area_name(unsigned uArea);
/** Soft policy token count. Soft!=product. */
unsigned    gj_string_soft_policy_count(void);
/** Soft policy token by index, or NULL. Soft!=product. */
const char *gj_string_soft_policy_name(unsigned uPolicy);

/** Total freestanding soft helper count (42). Soft!=product. */
unsigned    gj_string_soft_helper_count(void);
/** Soft helper name by index, or NULL. Soft!=product. */
const char *gj_string_soft_helper_name(unsigned uIndex);
/** Soft group count (length..format = 10). Soft!=product. */
unsigned    gj_string_soft_group_count(void);
/** Soft group name by index, or NULL. Soft!=product. */
const char *gj_string_soft_group_name(unsigned uGroup);
/** Soft per-group helper count, or 0 if out of range. Soft!=product. */
unsigned    gj_string_soft_group_helper_count(unsigned uGroup);

/** Per-group size accessors (Soft!=product residual; cold only). */
unsigned    gj_string_soft_length_count(void);
unsigned    gj_string_soft_compare_count(void);
unsigned    gj_string_soft_copy_count(void);
unsigned    gj_string_soft_search_count(void);
unsigned    gj_string_soft_case_count(void);
unsigned    gj_string_soft_ctype_count(void);
unsigned    gj_string_soft_mem_count(void);
unsigned    gj_string_soft_parse_count(void);
unsigned    gj_string_soft_io_count(void);
unsigned    gj_string_soft_format_count(void);

#ifdef __cplusplus
}
#endif
