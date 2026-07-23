/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 */
#include <gj/config.h>
#include <gj/console.h>
#include <gj/klog.h>
#include <gj/types.h>

static inline void
outb(u16 uPort, u8 u8Val)
{
    __asm__ volatile ("outb %0, %1" : : "a"(u8Val), "Nd"(uPort));
}

static inline u8
inb(u16 uPort)
{
    u8 u8Val;

    __asm__ volatile ("inb %1, %0" : "=a"(u8Val) : "Nd"(uPort));
    return u8Val;
}

void
serial_init(void)
{
    u16 uPort = (u16)GJ_SERIAL_PORT;

    outb(uPort + 1, 0x00); /* disable interrupts */
    outb(uPort + 3, 0x80); /* DLAB on */
    outb(uPort + 0, 0x03); /* baud divisor lo (38400) */
    outb(uPort + 1, 0x00); /* divisor hi */
    outb(uPort + 3, 0x03); /* 8N1 */
    outb(uPort + 2, 0xC7); /* FIFO */
    outb(uPort + 4, 0x0B); /* IRQs enabled, RTS/DSR */
}

void
serial_putchar(char chOut)
{
    u16 uPort = (u16)GJ_SERIAL_PORT;

    if (chOut == '\n') {
        serial_putchar('\r');
    }
    while ((inb(uPort + 5) & 0x20) == 0) {
        /* wait transmitter empty */
    }
    outb(uPort, (u8)chOut);
}

void
serial_write(const char *szText)
{
    while (szText && *szText) {
        serial_putchar(*szText++);
    }
}

/* Shared C kernel console (stdio_k / kprintf). */
void
console_putchar(char chOut)
{
    serial_putchar(chOut);
}

void
console_write(const char *szText)
{
    serial_write(szText);
}

int
serial_poll(void)
{
    u16 uPort = (u16)GJ_SERIAL_PORT;

    return (inb(uPort + 5) & 0x01) != 0 ? 1 : 0;
}

int
serial_getchar(void)
{
    u16 uPort = (u16)GJ_SERIAL_PORT;

    while (!serial_poll()) {
        /* spin — product can sleep via yield from userspace poll */
    }
    return (int)inb(uPort);
}
