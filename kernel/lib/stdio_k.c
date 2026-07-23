/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Shared freestanding kprintf (x86_64 + aarch64 product). Console via
 * console_putchar / console_write (arch provides).
 */
#include <gj/console.h>
#include <gj/klog.h>
#include <gj/types.h>
#include <stdarg.h>

/* provided by string.c */
size_t strlen(const char *szText);

static void
print_u64(u64 u64Val, unsigned uBase, int nWidth, char chPad)
{
    char szBuf[32];
    const char *szDigits = "0123456789abcdef";
    int nLen = 0;

    if (uBase < 2 || uBase > 16) {
        uBase = 10;
    }
    if (u64Val == 0) {
        szBuf[nLen++] = '0';
    } else {
        while (u64Val) {
            szBuf[nLen++] = szDigits[u64Val % uBase];
            u64Val /= uBase;
        }
    }
    while (nLen < nWidth) {
        szBuf[nLen++] = chPad;
    }
    while (nLen > 0) {
        console_putchar(szBuf[--nLen]);
    }
}

static void
print_i64(i64 i64Val)
{
    if (i64Val < 0) {
        console_putchar('-');
        print_u64((u64)(-i64Val), 10, 0, '0');
    } else {
        print_u64((u64)i64Val, 10, 0, '0');
    }
}

void
kprintf(const char *szFmt, ...)
{
    va_list vaArgs;
    const char *szCursor;

    va_start(vaArgs, szFmt);
    for (szCursor = szFmt; szCursor && *szCursor; szCursor++) {
        if (*szCursor != '%') {
            console_putchar(*szCursor);
            continue;
        }
        szCursor++;
        if (!*szCursor) {
            break;
        }
        switch (*szCursor) {
        case '%':
            console_putchar('%');
            break;
        case 'c':
            console_putchar((char)va_arg(vaArgs, int));
            break;
        case 's': {
            const char *szArg = va_arg(vaArgs, const char *);

            console_write(szArg ? szArg : "(null)");
            break;
        }
        case 'd':
        case 'i':
            print_i64((i64)va_arg(vaArgs, int));
            break;
        case 'u':
            print_u64((u64)va_arg(vaArgs, unsigned), 10, 0, '0');
            break;
        case 'x':
            print_u64((u64)va_arg(vaArgs, unsigned), 16, 0, '0');
            break;
        case 'p':
            console_write("0x");
            print_u64((u64)va_arg(vaArgs, void *), 16, 16, '0');
            break;
        case 'l':
            if (szCursor[1] == 'u') {
                szCursor++;
                print_u64(va_arg(vaArgs, unsigned long), 10, 0, '0');
            } else if (szCursor[1] == 'x') {
                szCursor++;
                print_u64(va_arg(vaArgs, unsigned long), 16, 0, '0');
            } else if (szCursor[1] == 'd') {
                szCursor++;
                print_i64((i64)va_arg(vaArgs, long));
            }
            break;
        default:
            console_putchar('%');
            console_putchar(*szCursor);
            break;
        }
    }
    va_end(vaArgs);
}

void
panic(const char *szMsg)
{
    kprintf("PANIC: %s\n", szMsg ? szMsg : "(null)");
    for (;;) {
#if defined(GJ_ARCH_AARCH64)
        __asm__ volatile("wfe");
#else
        __asm__ volatile("cli; hlt");
#endif
    }
}
