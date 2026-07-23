/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * major / minor / makedev (sys/sysmacros.h) + gnu_dev_* names.
 */
#include <sys/sysmacros.h>
#include <sys/types.h>

unsigned int
gnu_dev_major(unsigned long long uDev)
{
    return (unsigned int)(((uDev >> 8) & 0xfff) |
                          ((unsigned int)(uDev >> 32) & ~0xfffu));
}

unsigned int
gnu_dev_minor(unsigned long long uDev)
{
    return (unsigned int)((uDev & 0xff) |
                          ((unsigned int)(uDev >> 12) & ~0xffu));
}

unsigned long long
gnu_dev_makedev(unsigned int uMajor, unsigned int uMinor)
{
    return ((unsigned long long)(uMajor & 0xfff) << 8) |
           ((unsigned long long)(uMajor & ~0xfffu) << 32) |
           ((unsigned long long)(uMinor & 0xff)) |
           ((unsigned long long)(uMinor & ~0xffu) << 12);
}

unsigned int
major(dev_t dev)
{
    return gnu_dev_major((unsigned long long)dev);
}

unsigned int
minor(dev_t dev)
{
    return gnu_dev_minor((unsigned long long)dev);
}

dev_t
makedev(unsigned int uMajor, unsigned int uMinor)
{
    return (dev_t)gnu_dev_makedev(uMajor, uMinor);
}
