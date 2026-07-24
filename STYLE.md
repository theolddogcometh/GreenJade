# GreenJade C Style Guide

Pure C11. OpenBSD/KNF *spirit*, not a copy of their tree.

## Priority order (always)

When style or structure fights other goals:

1. **Security**  
2. **Performance**  
3. **Portability**  
4. **Readability** (this document)

Readability loses to a clearer security boundary or a measured hot-path win.  
Do **not** make code “pretty” by removing checks, widening privileges, or hiding arch hazards.

**Still required:** Hungarian variables, MIT headers, pure C — those serve security review as much as style.

## Files

- One logical module per `.c`; matching decls in `kernel/include/gj/` or local `.h`
- Every file starts with:

```c
/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 */
```

## Naming — Hungarian notation (required)

**All variables** (locals, parameters, fields, file-static, globals) use **Systems Hungarian**: a lowercase type/role prefix, then a descriptive name. Prefer `camelCase` after the prefix (`nFrameCount`, `pTag`, `paKernelEnd`).

Functions and types do **not** use Hungarian (they keep module/snake style).

### Prefix table

| Prefix | Meaning | Example |
|--------|---------|---------|
| `f` | boolean / flag (`bool` or 0/1 `int`) | `fReady`, `fPresent` |
| `ch` | single character | `chPad` |
| `n` | signed integer count / general int | `nIndex`, `nWidth` |
| `u` | unsigned integer (generic) | `uVector` |
| `u8` `u16` `u32` `u64` | fixed-width unsigned | `u32TagType`, `u64Length` |
| `i8` `i16` `i32` `i64` | fixed-width signed | `i64Value` |
| `cb` | count of **bytes** (size) | `cbLength`, `cbCopied` |
| `c` | count of **elements** (not bytes) | `cRegions`, `cFrames` |
| `i` | loop / array index | `iFrame`, `iByte` |
| `p` | pointer (any) | `pTag`, `pDst`, `pSrc` |
| `pp` | pointer to pointer | `ppNext` |
| `sz` | C string (`char *` NUL-terminated) | `szFmt`, `szMsg` |
| `pa` | physical address (`gj_paddr_t`) | `paPage`, `paKernelStart` |
| `va` | virtual address (`gj_vaddr_t`) | `vaEntry`, `vaStack` |
| `cap` / `ref` | capability (`struct gj_cap_ref`) | `refEndpoint`, `u64Slot` |
| `st` | status (`gj_status_t`) | `stResult` |
| `pfn` | function pointer | `pfnRead` |
| `h` | opaque handle (non-cap) | `hPort` |
| `g_` | global / file-static duration | `g_cFramesFree`, `g_pBitmap` |
| `s_` | file-static helper (optional if already static) | `s_bitSet` for static funcs? **no** — functions stay non-Hungarian |

### Rules

1. **Every** local and parameter is prefixed. No bare `i`, `n`, `p`, `tmp` without a full name when possible (`iFrame` not just `i` unless trivial 1-letter loop is clearly an index — prefer `iFrame`).
2. **Struct fields** use Hungarian too: `struct gj_mem_region { gj_paddr_t paBase; u64 cbLength; };`
3. **Pointers to structs:** `p` + type-ish name: `pRegion`, `pThread` (not `p_gj_thread`).
4. **Booleans:** `f` prefix, not `b` (reserve `b` if we ever need byte-buffer without length).
5. **Globals:** `g_` + Hungarian: `g_cFramesTotal`, `g_pBitmap`.
6. **Do not** Hungarian-encode function names: `pmm_alloc`, `kprintf`, `serial_init`.
7. **Do not** Hungarian-encode types/macros: `gj_thread_t`, `GJ_PAGE_SIZE`.
8. **No COM-style** `m_pIFoo` / interface vtables as design goal (OOP is out). `pfn` ops tables are OK.

### Examples

```c
void
pmm_init(const struct gj_mem_region *pRegions, size_t cRegions,
         gj_paddr_t paKernelStart, gj_paddr_t paKernelEnd)
{
    size_t iRegion;
    u32 iFrame;

    for (iRegion = 0; iRegion < cRegions; iRegion++) {
        gj_paddr_t paBase = pRegions[iRegion].paBase;
        u64 cbLen = pRegions[iRegion].cbLength;
        /* ... */
        (void)paBase;
        (void)cbLen;
    }
    for (iFrame = 0; iFrame < g_cFramesTotal; iFrame++) {
        /* ... */
    }
}

static void
serial_putchar(char chOut)
{
    u16 uPort = (u16)GJ_SERIAL_PORT;
    /* ... */
    (void)chOut;
    (void)uPort;
}
```

## Other naming

| Kind | Style | Example |
|------|--------|---------|
| Types | `gj_` + snake + `_t` | `gj_thread_t` |
| Functions | module_snake | `pmm_alloc`, `kprintf` |
| Macros | `GJ_` uppercase | `GJ_PAGE_SIZE` |
| Enum constants | `GJ_` uppercase | `GJ_ERR_NOMEM` |

## Formatting

- Indent: 4 spaces (no tabs)
- Opening brace on its **own line** for functions; same line for `if`/`while`/`for`/`switch`.

```c
void
foo(int nArg)
{
    if (nArg < 0) {
        return;
    }
}
```

- Line length: prefer ≤ 100 columns
- Pointer: `type *pName` (space before `*`)
- Prefer `/* */` for block docs

## Language subset

**Allowed:** C11, stdint, freestanding, static inline  
**Forbidden:** VLA in security paths, goto spaghetti (single cleanup `goto out` OK), OOP patterns

## Error handling

- Return `gj_status_t st`: `0` or negative `GJ_ERR_*`
- No exceptions

## Comments

- Why, not what
- Public APIs: brief comment above declaration

## Includes

```c
#include <gj/types.h>
#include <stdint.h>
```

Order: own module header, `gj/*`, freestanding system.

## Testing hooks

- `GJ_DEBUG` gates verbose logs
- Panic paths must not return

## Soft inventory / honesty stamps (Wave 104)

Greppable soft inventory comments and honesty ledgers in headers/docs are **bring-up observability only** (e.g. `*: soft inventory …`). They do **not** light product lamps, close Steam **bar3**, or promote Deck Top 50 rows. Continuum high-water **advancing toward 24600** is CREATE-ONLY soft graph parent wire (Wave 104 soft deepen surfaces **retlinearangle**/**retbilinearangle**) — **soft ≠ product complete**.
