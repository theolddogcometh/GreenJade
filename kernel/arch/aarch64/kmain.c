/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * aarch64 product kmain — shared C (string/kprintf/pmm_core/sched_coop) +
 * arch bring-up (GIC/timer/MMU/SVC/virtio/PSCI).
 *
 * Greppable markers — see README.md
 */
#include <gj/klog.h>
#include <gj/pmm_core.h>
#include <gj/sched_coop.h>
#include <gj/string.h>
#include <gj/types.h>

void aarch64_gic_init(void);
void aarch64_timer_probe(void);
void aarch64_exceptions_install(void);
void aarch64_cpu_probe(void);
void aarch64_pmm_init(void);
void aarch64_mmu_init(void);
void aarch64_svc_selftest(void);
void aarch64_virtio_mmio_probe(void);
void aarch64_psci_probe(void);

static void
shared_c_probe(void)
{
    char a[16];
    const char *sz = "shared";

    memset(a, 0, sizeof(a));
    memcpy(a, sz, 6);
    if (memcmp(a, sz, 6) != 0 || strlen(a) != 6u) {
        kprintf("aarch64: shared C kernel soft FAIL\n");
        return;
    }
    kprintf("aarch64: shared C kernel PASS (string+kprintf)\n");
}

static void
shared_mm_sched_probe(void)
{
    if (gj_coop_selftest() != 0) {
        kprintf("aarch64: shared sched PASS\n");
    } else {
        kprintf("aarch64: shared sched soft FAIL\n");
    }
    /* pmm selftest already in aarch64_pmm_init; note shared core marker. */
    kprintf("aarch64: shared pmm core free=%u\n", gj_pmm_core_free_count());
}

static void
mem_probe(void)
{
    volatile unsigned long *p = (volatile unsigned long *)0x40080000ul;
    unsigned long v;

    *p = 0xa5a5a5a5a5a5a5a5ul;
    v = *p;
    if (v == 0xa5a5a5a5a5a5a5a5ul) {
        kprintf("aarch64: mem probe PASS\n");
    } else {
        kprintf("aarch64: mem probe soft FAIL\n");
    }
}

void
aarch64_kmain(void)
{
    kprintf("GreenJade aarch64 product kernel (shared C)\n");
    shared_c_probe();
    aarch64_exceptions_install();
    aarch64_cpu_probe();
    /* PSCI: HVC/SMC with fault recovery (PASS or soft SKIP). */
    aarch64_psci_probe();
    aarch64_gic_init();
    aarch64_timer_probe();
    aarch64_pmm_init();
    aarch64_mmu_init();
    gj_coop_init();
    shared_mm_sched_probe();
    aarch64_svc_selftest();
    aarch64_virtio_mmio_probe();
    mem_probe();
    kprintf("M0 OK\n");
    for (;;) {
        __asm__ volatile("wfe");
    }
}

void
aarch64_kmain_stub(void)
{
    aarch64_kmain();
}
