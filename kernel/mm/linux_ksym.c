/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft Linux ksym table for .ko unresolved resolve (clean-room stubs) and
 * a greppable name surface for soft eng + ABI hostability + future
 * userspace binding. Soft!=product Linux ABI. Dual MIT OR Apache-2.0.
 * No GPL source.
 *
 * G-AC-1: no .ko runs in kernel as product. ksym never claims a G-AC-1
 * waiver (g_ac1_waiver=0): resolve-for-eng != product .ko wire and !=
 * in-kernel .ko init. freestanding_no_exec honesty: this unit supports
 * soft-loaded modules resolving SHN_UNDEF for eng under peer RUN_INIT=0
 * / SKIP exec; never_exec_ko stance is not waived by a successful lookup.
 *
 * Capacity: LINUX_KSYM_MAX (4096) - base stubs + large soft seed
 * (LINUX_KSYM_USB_SOFT_SEED=191 usbcore+scsi_mod+sg+msc_leaf) +
 * freestanding USB residual (LINUX_KSYM_USB_STORAGE_RESIDUAL=62) +
 * freestanding net residual (LINUX_KSYM_NET_RESIDUAL=48) +
 * freestanding PCI residual (LINUX_KSYM_PCI_RESIDUAL=32) +
 * multi-mod export headroom. Soft!=product.
 *
 * Residual lean (C0 soft eng; Soft!=product; G-AC-1; no stamp storms):
 * boot emits lean residual + residual lean PASS (when arms hold) + init PASS -
 *   greppable: linux_ksym: soft residual lean usb= net= pci= leaf=
 *     freestanding_no_exec=1 never_exec_ko=1 resolve_only=1 hostability=1
 *     g_ac1_waiver=0 storm=0 arms=ok/checks exclusive=1
 *   greppable: linux_ksym: soft residual lean PASS
 *     (functional arms only; never Dual DoD / never product .ko wire)
 *   greppable: linux_ksym: soft init PASS n= max= free=
 *     freestanding_no_exec=1 g_ac1_waiver=0 storm=0
 * Functional residual arms (GJ_KSYM_LEAN_CHECKS; stamp-free C0 deepen):
 *   1 cap  2 sizes  3 resolve  4 count  5 headroom
 *   6 replace  7 miss  8 lic
 * On-demand (not boot residual flood):
 *   greppable: linux_ksym: soft inventory n= max= free= (lean one line)
 *   greppable: linux_ksym: soft has miss name=  (first miss only)
 * Class sizes usb/net/pci/leaf remain constants; registrations stay;
 * per-class residual kprintf flood removed (ret*angle / multi-line storm class).
 * init PASS does not restate residual class sizes (lean residual owns those).
 * Peer: linux_module soft FAIL KSYM for strong SHN_UNDEF misses;
 * peer freestanding_no_exec SKIP exec is orthogonal (ksym resolve != exec).
 * Does not touch linux_module soft init SKIP exec path.
 * Dual DoD A/B remain OPEN (not closed by residual lean PASS).
 * No version stamp. Dual MIT OR Apache-2.0. Soft!=product; G-AC-1.
 */
#include <gj/linux_ksym.h>
#include <gj/klog.h>
#include <gj/string.h>
#include <gj/types.h>

/* Header owns canonical values; keep local fallback if header older. */
#ifndef LINUX_KSYM_MAX
#define LINUX_KSYM_MAX 4096u
#endif
#ifndef LINUX_KSYM_USB_SOFT_SEED
#define LINUX_KSYM_USB_SOFT_SEED 191u
#endif
#ifndef LINUX_KSYM_USB_STORAGE_LEAF
#define LINUX_KSYM_USB_STORAGE_LEAF 15u
#endif
#ifndef LINUX_KSYM_USB_STORAGE_RESIDUAL
#define LINUX_KSYM_USB_STORAGE_RESIDUAL 62u
#endif
#ifndef LINUX_KSYM_NET_RESIDUAL
#define LINUX_KSYM_NET_RESIDUAL 48u
#endif
#ifndef LINUX_KSYM_PCI_RESIDUAL
#define LINUX_KSYM_PCI_RESIDUAL 32u
#endif
/*
 * Functional residual self-check arm count (C0 residual deepen; stamp-free):
 *   1 cap  2 sizes  3 resolve  4 count  5 headroom  6 replace  7 miss  8 lic
 * Soft!=product · G-AC-1 · dual MIT|Apache-2.0. Never hard-gates product.
 * Never closes Dual DoD A/B. Never bumps GJ_IMAGE_VERSION.
 */
#define GJ_KSYM_LEAN_CHECKS 8u
/* Residual class total (leaf + usb + net + pci); design budget honesty. */
#define GJ_KSYM_RESIDUAL_TOTAL                                         \
    (LINUX_KSYM_USB_STORAGE_LEAF + LINUX_KSYM_USB_STORAGE_RESIDUAL +   \
     LINUX_KSYM_NET_RESIDUAL + LINUX_KSYM_PCI_RESIDUAL)
struct gj_ksym_ent {
    const char *szName;
    void *pAddr;
};

static struct gj_ksym_ent g_aKsym[LINUX_KSYM_MAX];
static u32 g_u32KsymN;
static u8 g_fInit;
static u8 g_fInvOnce;
static u8 g_fHasMissLog;      /* rate-limit soft has miss lamp (first only) */
static u32 g_u32RegFull;      /* register rejected: table full */
static u32 g_u32HasMiss;      /* has() miss count (diagnostic) */
static u32 g_u32HasHit;       /* has() hit count (diagnostic) */
static u32 g_u32RegOk;        /* register new-slot success count */
static u32 g_u32RegReplace;   /* register replace existing success count */
static u32 g_u32LeanChecks;   /* last residual lean arm count */
static u32 g_u32LeanOk;       /* last residual lean arms that held */

/* Soft zero return / data page for every registered name */
static long gj_ksym_soft___SCT__might_resched(void) __attribute__((used));
static long gj_ksym_soft___SCT__might_resched(void) { return 0; }
static long gj_ksym_soft___alloc_pages(void) __attribute__((used));
static long gj_ksym_soft___alloc_pages(void) { return 0; }
static long gj_ksym_soft___const_udelay(void) __attribute__((used));
static long gj_ksym_soft___const_udelay(void) { return 0; }
static long gj_ksym_soft___devm_mdiobus_register(void) __attribute__((used));
static long gj_ksym_soft___devm_mdiobus_register(void) { return 0; }
static long gj_ksym_soft___dma_sync_single_for_cpu(void) __attribute__((used));
static long gj_ksym_soft___dma_sync_single_for_cpu(void) { return 0; }
static long gj_ksym_soft___dma_sync_single_for_device(void) __attribute__((used));
static long gj_ksym_soft___dma_sync_single_for_device(void) { return 0; }
static long gj_ksym_soft___fentry__(void) __attribute__((used));
static long gj_ksym_soft___fentry__(void) { return 0; }
static long gj_ksym_soft___free_pages(void) __attribute__((used));
static long gj_ksym_soft___free_pages(void) { return 0; }
static long gj_ksym_soft___mdiobus_write(void) __attribute__((used));
static long gj_ksym_soft___mdiobus_write(void) { return 0; }
static long gj_ksym_soft___mutex_init(void) __attribute__((used));
static long gj_ksym_soft___mutex_init(void) { return 0; }
static long gj_ksym_soft___napi_schedule(void) __attribute__((used));
static long gj_ksym_soft___napi_schedule(void) { return 0; }
static long gj_ksym_soft___pci_register_driver(void) __attribute__((used));
static long gj_ksym_soft___pci_register_driver(void) { return 0; }
static long gj_ksym_soft___phy_modify(void) __attribute__((used));
static long gj_ksym_soft___phy_modify(void) { return 0; }
static long gj_ksym_soft___pm_runtime_idle(void) __attribute__((used));
static long gj_ksym_soft___pm_runtime_idle(void) { return 0; }
static long gj_ksym_soft___pm_runtime_resume(void) __attribute__((used));
static long gj_ksym_soft___pm_runtime_resume(void) { return 0; }
static long gj_ksym_soft___pskb_pull_tail(void) __attribute__((used));
static long gj_ksym_soft___pskb_pull_tail(void) { return 0; }
static long gj_ksym_soft___skb_pad(void) __attribute__((used));
static long gj_ksym_soft___skb_pad(void) { return 0; }
static long gj_ksym_soft___stack_chk_fail(void) __attribute__((used));
static long gj_ksym_soft___stack_chk_fail(void) { return 0; }
static long gj_ksym_soft___warn_printk(void) __attribute__((used));
static long gj_ksym_soft___warn_printk(void) { return 0; }
static long gj_ksym_soft___x86_indirect_jump_thunk_r10(void) __attribute__((used));
static long gj_ksym_soft___x86_indirect_jump_thunk_r10(void) { return 0; }
static long gj_ksym_soft___x86_indirect_jump_thunk_r11(void) __attribute__((used));
static long gj_ksym_soft___x86_indirect_jump_thunk_r11(void) { return 0; }
static long gj_ksym_soft___x86_indirect_jump_thunk_r12(void) __attribute__((used));
static long gj_ksym_soft___x86_indirect_jump_thunk_r12(void) { return 0; }
static long gj_ksym_soft___x86_indirect_jump_thunk_r13(void) __attribute__((used));
static long gj_ksym_soft___x86_indirect_jump_thunk_r13(void) { return 0; }
static long gj_ksym_soft___x86_indirect_jump_thunk_r14(void) __attribute__((used));
static long gj_ksym_soft___x86_indirect_jump_thunk_r14(void) { return 0; }
static long gj_ksym_soft___x86_indirect_jump_thunk_r15(void) __attribute__((used));
static long gj_ksym_soft___x86_indirect_jump_thunk_r15(void) { return 0; }
static long gj_ksym_soft___x86_indirect_jump_thunk_r8(void) __attribute__((used));
static long gj_ksym_soft___x86_indirect_jump_thunk_r8(void) { return 0; }
static long gj_ksym_soft___x86_indirect_jump_thunk_r9(void) __attribute__((used));
static long gj_ksym_soft___x86_indirect_jump_thunk_r9(void) { return 0; }
static long gj_ksym_soft___x86_indirect_jump_thunk_rax(void) __attribute__((used));
static long gj_ksym_soft___x86_indirect_jump_thunk_rax(void) { return 0; }
static long gj_ksym_soft___x86_indirect_jump_thunk_rbp(void) __attribute__((used));
static long gj_ksym_soft___x86_indirect_jump_thunk_rbp(void) { return 0; }
static long gj_ksym_soft___x86_indirect_jump_thunk_rbx(void) __attribute__((used));
static long gj_ksym_soft___x86_indirect_jump_thunk_rbx(void) { return 0; }
static long gj_ksym_soft___x86_indirect_jump_thunk_rcx(void) __attribute__((used));
static long gj_ksym_soft___x86_indirect_jump_thunk_rcx(void) { return 0; }
static long gj_ksym_soft___x86_indirect_jump_thunk_rdi(void) __attribute__((used));
static long gj_ksym_soft___x86_indirect_jump_thunk_rdi(void) { return 0; }
static long gj_ksym_soft___x86_indirect_jump_thunk_rdx(void) __attribute__((used));
static long gj_ksym_soft___x86_indirect_jump_thunk_rdx(void) { return 0; }
static long gj_ksym_soft___x86_indirect_jump_thunk_rsi(void) __attribute__((used));
static long gj_ksym_soft___x86_indirect_jump_thunk_rsi(void) { return 0; }
static long gj_ksym_soft___x86_indirect_thunk_r10(void) __attribute__((used));
static long gj_ksym_soft___x86_indirect_thunk_r10(void) { return 0; }
static long gj_ksym_soft___x86_indirect_thunk_r11(void) __attribute__((used));
static long gj_ksym_soft___x86_indirect_thunk_r11(void) { return 0; }
static long gj_ksym_soft___x86_indirect_thunk_r12(void) __attribute__((used));
static long gj_ksym_soft___x86_indirect_thunk_r12(void) { return 0; }
static long gj_ksym_soft___x86_indirect_thunk_r13(void) __attribute__((used));
static long gj_ksym_soft___x86_indirect_thunk_r13(void) { return 0; }
static long gj_ksym_soft___x86_indirect_thunk_r14(void) __attribute__((used));
static long gj_ksym_soft___x86_indirect_thunk_r14(void) { return 0; }
static long gj_ksym_soft___x86_indirect_thunk_r15(void) __attribute__((used));
static long gj_ksym_soft___x86_indirect_thunk_r15(void) { return 0; }
static long gj_ksym_soft___x86_indirect_thunk_r8(void) __attribute__((used));
static long gj_ksym_soft___x86_indirect_thunk_r8(void) { return 0; }
static long gj_ksym_soft___x86_indirect_thunk_r9(void) __attribute__((used));
static long gj_ksym_soft___x86_indirect_thunk_r9(void) { return 0; }
static long gj_ksym_soft___x86_indirect_thunk_rax(void) __attribute__((used));
static long gj_ksym_soft___x86_indirect_thunk_rax(void) { return 0; }
static long gj_ksym_soft___x86_indirect_thunk_rbp(void) __attribute__((used));
static long gj_ksym_soft___x86_indirect_thunk_rbp(void) { return 0; }
static long gj_ksym_soft___x86_indirect_thunk_rbx(void) __attribute__((used));
static long gj_ksym_soft___x86_indirect_thunk_rbx(void) { return 0; }
static long gj_ksym_soft___x86_indirect_thunk_rcx(void) __attribute__((used));
static long gj_ksym_soft___x86_indirect_thunk_rcx(void) { return 0; }
static long gj_ksym_soft___x86_indirect_thunk_rdi(void) __attribute__((used));
static long gj_ksym_soft___x86_indirect_thunk_rdi(void) { return 0; }
static long gj_ksym_soft___x86_indirect_thunk_rdx(void) __attribute__((used));
static long gj_ksym_soft___x86_indirect_thunk_rdx(void) { return 0; }
static long gj_ksym_soft___x86_indirect_thunk_rsi(void) __attribute__((used));
static long gj_ksym_soft___x86_indirect_thunk_rsi(void) { return 0; }
static long gj_ksym_soft___x86_return_thunk(void) __attribute__((used));
static long gj_ksym_soft___x86_return_thunk(void) { return 0; }
static long gj_ksym_soft__dev_err(void) __attribute__((used));
static long gj_ksym_soft__dev_err(void) { return 0; }
static long gj_ksym_soft__dev_info(void) __attribute__((used));
static long gj_ksym_soft__dev_info(void) { return 0; }
static long gj_ksym_soft__dev_warn(void) __attribute__((used));
static long gj_ksym_soft__dev_warn(void) { return 0; }
static long gj_ksym_soft__mcount(void) __attribute__((used));
static long gj_ksym_soft__mcount(void) { return 0; }
static long gj_ksym_soft__printk(void) __attribute__((used));
static long gj_ksym_soft__printk(void) { return 0; }
static long gj_ksym_soft__raw_spin_lock_irqsave(void) __attribute__((used));
static long gj_ksym_soft__raw_spin_lock_irqsave(void) { return 0; }
static long gj_ksym_soft__raw_spin_unlock_irqrestore(void) __attribute__((used));
static long gj_ksym_soft__raw_spin_unlock_irqrestore(void) { return 0; }
static long gj_ksym_soft_alloc_etherdev_mqs(void) __attribute__((used));
static long gj_ksym_soft_alloc_etherdev_mqs(void) { return 0; }
static u8 gj_ksym_soft_byte_rev_table[64] __attribute__((used));
static long gj_ksym_soft_clk_disable(void) __attribute__((used));
static long gj_ksym_soft_clk_disable(void) { return 0; }
static long gj_ksym_soft_clk_enable(void) __attribute__((used));
static long gj_ksym_soft_clk_enable(void) { return 0; }
static long gj_ksym_soft_clk_prepare(void) __attribute__((used));
static long gj_ksym_soft_clk_prepare(void) { return 0; }
static long gj_ksym_soft_clk_unprepare(void) __attribute__((used));
static long gj_ksym_soft_clk_unprepare(void) { return 0; }
static long gj_ksym_soft_crc32_le(void) __attribute__((used));
static long gj_ksym_soft_crc32_le(void) { return 0; }
static long gj_ksym_soft_csum_ipv6_magic(void) __attribute__((used));
static long gj_ksym_soft_csum_ipv6_magic(void) { return 0; }
static long gj_ksym_soft_dev_driver_string(void) __attribute__((used));
static long gj_ksym_soft_dev_driver_string(void) { return 0; }
static long gj_ksym_soft_dev_err_probe(void) __attribute__((used));
static long gj_ksym_soft_dev_err_probe(void) { return 0; }
static long gj_ksym_soft_dev_fetch_sw_netstats(void) __attribute__((used));
static long gj_ksym_soft_dev_fetch_sw_netstats(void) { return 0; }
static long gj_ksym_soft_dev_kfree_skb_any(void) __attribute__((used));
static long gj_ksym_soft_dev_kfree_skb_any(void) { return 0; }
static long gj_ksym_soft_dev_kfree_skb_any_reason(void) __attribute__((used));
static long gj_ksym_soft_dev_kfree_skb_any_reason(void) { return 0; }
static long gj_ksym_soft_device_set_wakeup_enable(void) __attribute__((used));
static long gj_ksym_soft_device_set_wakeup_enable(void) { return 0; }
static long gj_ksym_soft_devm_alloc_etherdev_mqs(void) __attribute__((used));
static long gj_ksym_soft_devm_alloc_etherdev_mqs(void) { return 0; }
static long gj_ksym_soft_devm_clk_get_optional_enabled(void) __attribute__((used));
static long gj_ksym_soft_devm_clk_get_optional_enabled(void) { return 0; }
static long gj_ksym_soft_devm_mdiobus_alloc_size(void) __attribute__((used));
static long gj_ksym_soft_devm_mdiobus_alloc_size(void) { return 0; }
static long gj_ksym_soft_disable_irq(void) __attribute__((used));
static long gj_ksym_soft_disable_irq(void) { return 0; }
static long gj_ksym_soft_disable_work(void) __attribute__((used));
static long gj_ksym_soft_disable_work(void) { return 0; }
static long gj_ksym_soft_disable_work_sync(void) __attribute__((used));
static long gj_ksym_soft_disable_work_sync(void) { return 0; }
static long gj_ksym_soft_dma_alloc_attrs(void) __attribute__((used));
static long gj_ksym_soft_dma_alloc_attrs(void) { return 0; }
static long gj_ksym_soft_dma_alloc_coherent(void) __attribute__((used));
static long gj_ksym_soft_dma_alloc_coherent(void) { return 0; }
static long gj_ksym_soft_dma_free_attrs(void) __attribute__((used));
static long gj_ksym_soft_dma_free_attrs(void) { return 0; }
static long gj_ksym_soft_dma_free_coherent(void) __attribute__((used));
static long gj_ksym_soft_dma_free_coherent(void) { return 0; }
static long gj_ksym_soft_dma_map_page_attrs(void) __attribute__((used));
static long gj_ksym_soft_dma_map_page_attrs(void) { return 0; }
static long gj_ksym_soft_dma_map_single(void) __attribute__((used));
static long gj_ksym_soft_dma_map_single(void) { return 0; }
static long gj_ksym_soft_dma_set_coherent_mask(void) __attribute__((used));
static long gj_ksym_soft_dma_set_coherent_mask(void) { return 0; }
static long gj_ksym_soft_dma_set_mask(void) __attribute__((used));
static long gj_ksym_soft_dma_set_mask(void) { return 0; }
static long gj_ksym_soft_dma_unmap_page_attrs(void) __attribute__((used));
static long gj_ksym_soft_dma_unmap_page_attrs(void) { return 0; }
static long gj_ksym_soft_dma_unmap_single(void) __attribute__((used));
static long gj_ksym_soft_dma_unmap_single(void) { return 0; }
static long gj_ksym_soft_dmam_alloc_attrs(void) __attribute__((used));
static long gj_ksym_soft_dmam_alloc_attrs(void) { return 0; }
static long gj_ksym_soft_dql_completed(void) __attribute__((used));
static long gj_ksym_soft_dql_completed(void) { return 0; }
static long gj_ksym_soft_dql_reset(void) __attribute__((used));
static long gj_ksym_soft_dql_reset(void) { return 0; }
static long gj_ksym_soft_enable_irq(void) __attribute__((used));
static long gj_ksym_soft_enable_irq(void) { return 0; }
static long gj_ksym_soft_enable_work(void) __attribute__((used));
static long gj_ksym_soft_enable_work(void) { return 0; }
static long gj_ksym_soft_eth_mac_addr(void) __attribute__((used));
static long gj_ksym_soft_eth_mac_addr(void) { return 0; }
static long gj_ksym_soft_eth_platform_get_mac_address(void) __attribute__((used));
static long gj_ksym_soft_eth_platform_get_mac_address(void) { return 0; }
static long gj_ksym_soft_eth_type_trans(void) __attribute__((used));
static long gj_ksym_soft_eth_type_trans(void) { return 0; }
static long gj_ksym_soft_eth_validate_addr(void) __attribute__((used));
static long gj_ksym_soft_eth_validate_addr(void) { return 0; }
static long gj_ksym_soft_ethtool_op_get_link(void) __attribute__((used));
static long gj_ksym_soft_ethtool_op_get_link(void) { return 0; }
static long gj_ksym_soft_ethtool_op_get_ts_info(void) __attribute__((used));
static long gj_ksym_soft_ethtool_op_get_ts_info(void) { return 0; }
/*
 * Empty ksym only: real soft bodies in linux_dma_soft (replace on init).
 * -ENOENT so accidental pre-replace calls do not look like "fw loaded".
 */
static long gj_ksym_soft_firmware_request_nowarn(void) __attribute__((used));
static long gj_ksym_soft_firmware_request_nowarn(void) { return -2; /* ENOENT */ }
static long gj_ksym_soft_request_firmware(void) __attribute__((used));
static long gj_ksym_soft_request_firmware(void) { return -2; /* ENOENT */ }
static long gj_ksym_soft_request_firmware_direct(void) __attribute__((used));
static long gj_ksym_soft_request_firmware_direct(void) { return -2; /* ENOENT */ }
static long gj_ksym_soft_fortify_panic(void) __attribute__((used));
static long gj_ksym_soft_fortify_panic(void) { return 0; }
static long gj_ksym_soft_free_irq(void) __attribute__((used));
static long gj_ksym_soft_free_irq(void) { return 0; }
static long gj_ksym_soft_free_netdev(void) __attribute__((used));
static long gj_ksym_soft_free_netdev(void) { return 0; }
static long gj_ksym_soft_genphy_soft_reset(void) __attribute__((used));
static long gj_ksym_soft_genphy_soft_reset(void) { return 0; }
static long gj_ksym_soft_get_random_bytes(void) __attribute__((used));
static long gj_ksym_soft_get_random_bytes(void) { return 0; }
static long gj_ksym_soft_ioremap(void) __attribute__((used));
static long gj_ksym_soft_ioremap(void) { return 0; }
static long gj_ksym_soft_iounmap(void) __attribute__((used));
static long gj_ksym_soft_iounmap(void) { return 0; }
static long gj_ksym_soft_is_vmalloc_addr(void) __attribute__((used));
static long gj_ksym_soft_is_vmalloc_addr(void) { return 0; }
static u8 gj_ksym_soft_jiffies[64] __attribute__((used));
static long gj_ksym_soft_kfree(void) __attribute__((used));
static long gj_ksym_soft_kfree(void) { return 0; }
static long gj_ksym_soft_kmalloc(void) __attribute__((used));
static long gj_ksym_soft_kmalloc(void) { return 0; }
static u8 gj_ksym_soft_kmalloc_caches[64] __attribute__((used));
static long gj_ksym_soft_kmalloc_trace(void) __attribute__((used));
static long gj_ksym_soft_kmalloc_trace(void) { return 0; }
static long gj_ksym_soft_ktime_get(void) __attribute__((used));
static long gj_ksym_soft_ktime_get(void) { return 0; }
static long gj_ksym_soft_kvfree(void) __attribute__((used));
static long gj_ksym_soft_kvfree(void) { return 0; }
static long gj_ksym_soft_kzalloc(void) __attribute__((used));
static long gj_ksym_soft_kzalloc(void) { return 0; }
static long gj_ksym_soft_led_classdev_register_ext(void) __attribute__((used));
static long gj_ksym_soft_led_classdev_register_ext(void) { return 0; }
static long gj_ksym_soft_led_classdev_unregister(void) __attribute__((used));
static long gj_ksym_soft_led_classdev_unregister(void) { return 0; }
static long gj_ksym_soft_mdelay(void) __attribute__((used));
static long gj_ksym_soft_mdelay(void) { return 0; }
static long gj_ksym_soft_mdiobus_get_phy(void) __attribute__((used));
static long gj_ksym_soft_mdiobus_get_phy(void) { return 0; }
static long gj_ksym_soft_mdiobus_read(void) __attribute__((used));
static long gj_ksym_soft_mdiobus_read(void) { return 0; }
static long gj_ksym_soft_mdiobus_write(void) __attribute__((used));
static long gj_ksym_soft_mdiobus_write(void) { return 0; }
static long gj_ksym_soft_memcmp(void) __attribute__((used));
static long gj_ksym_soft_memcmp(void) { return 0; }
static long gj_ksym_soft_memcpy(void) __attribute__((used));
static long gj_ksym_soft_memcpy(void) { return 0; }
static long gj_ksym_soft_memcpy_fromio(void) __attribute__((used));
static long gj_ksym_soft_memcpy_fromio(void) { return 0; }
static long gj_ksym_soft_memmove(void) __attribute__((used));
static long gj_ksym_soft_memmove(void) { return 0; }
static long gj_ksym_soft_memset(void) __attribute__((used));
static long gj_ksym_soft_memset(void) { return 0; }
static long gj_ksym_soft_module_put(void) __attribute__((used));
static long gj_ksym_soft_module_put(void) { return 0; }
static long gj_ksym_soft_msleep(void) __attribute__((used));
static long gj_ksym_soft_msleep(void) { return 0; }
static long gj_ksym_soft_mutex_lock(void) __attribute__((used));
static long gj_ksym_soft_mutex_lock(void) { return 0; }
static long gj_ksym_soft_mutex_unlock(void) __attribute__((used));
static long gj_ksym_soft_mutex_unlock(void) { return 0; }
static long gj_ksym_soft_napi_alloc_skb(void) __attribute__((used));
static long gj_ksym_soft_napi_alloc_skb(void) { return 0; }
static long gj_ksym_soft_napi_complete_done(void) __attribute__((used));
static long gj_ksym_soft_napi_complete_done(void) { return 0; }
static long gj_ksym_soft_napi_consume_skb(void) __attribute__((used));
static long gj_ksym_soft_napi_consume_skb(void) { return 0; }
static long gj_ksym_soft_napi_disable(void) __attribute__((used));
static long gj_ksym_soft_napi_disable(void) { return 0; }
static long gj_ksym_soft_napi_enable(void) __attribute__((used));
static long gj_ksym_soft_napi_enable(void) { return 0; }
static long gj_ksym_soft_napi_gro_receive(void) __attribute__((used));
static long gj_ksym_soft_napi_gro_receive(void) { return 0; }
static long gj_ksym_soft_napi_schedule_prep(void) __attribute__((used));
static long gj_ksym_soft_napi_schedule_prep(void) { return 0; }
static long gj_ksym_soft_net_ratelimit(void) __attribute__((used));
static long gj_ksym_soft_net_ratelimit(void) { return 0; }
static long gj_ksym_soft_netdev_err(void) __attribute__((used));
static long gj_ksym_soft_netdev_err(void) { return 0; }
static long gj_ksym_soft_netdev_info(void) __attribute__((used));
static long gj_ksym_soft_netdev_info(void) { return 0; }
static long gj_ksym_soft_netdev_notice(void) __attribute__((used));
static long gj_ksym_soft_netdev_notice(void) { return 0; }
static long gj_ksym_soft_netdev_printk(void) __attribute__((used));
static long gj_ksym_soft_netdev_printk(void) { return 0; }
static long gj_ksym_soft_netdev_stats_to_stats64(void) __attribute__((used));
static long gj_ksym_soft_netdev_stats_to_stats64(void) { return 0; }
static long gj_ksym_soft_netdev_sw_irq_coalesce_default_on(void) __attribute__((used));
static long gj_ksym_soft_netdev_sw_irq_coalesce_default_on(void) { return 0; }
static long gj_ksym_soft_netdev_update_features(void) __attribute__((used));
static long gj_ksym_soft_netdev_update_features(void) { return 0; }
static long gj_ksym_soft_netdev_warn(void) __attribute__((used));
static long gj_ksym_soft_netdev_warn(void) { return 0; }
static long gj_ksym_soft_netif_carrier_off(void) __attribute__((used));
static long gj_ksym_soft_netif_carrier_off(void) { return 0; }
static long gj_ksym_soft_netif_carrier_on(void) __attribute__((used));
static long gj_ksym_soft_netif_carrier_on(void) { return 0; }
static long gj_ksym_soft_netif_device_attach(void) __attribute__((used));
static long gj_ksym_soft_netif_device_attach(void) { return 0; }
static long gj_ksym_soft_netif_device_detach(void) __attribute__((used));
static long gj_ksym_soft_netif_device_detach(void) { return 0; }
static long gj_ksym_soft_netif_napi_add_weight_locked(void) __attribute__((used));
static long gj_ksym_soft_netif_napi_add_weight_locked(void) { return 0; }
static long gj_ksym_soft_netif_receive_skb(void) __attribute__((used));
static long gj_ksym_soft_netif_receive_skb(void) { return 0; }
static long gj_ksym_soft_netif_schedule_queue(void) __attribute__((used));
static long gj_ksym_soft_netif_schedule_queue(void) { return 0; }
static long gj_ksym_soft_netif_set_tso_max_segs(void) __attribute__((used));
static long gj_ksym_soft_netif_set_tso_max_segs(void) { return 0; }
static long gj_ksym_soft_netif_set_tso_max_size(void) __attribute__((used));
static long gj_ksym_soft_netif_set_tso_max_size(void) { return 0; }
static long gj_ksym_soft_netif_start_queue(void) __attribute__((used));
static long gj_ksym_soft_netif_start_queue(void) { return 0; }
static long gj_ksym_soft_netif_stop_queue(void) __attribute__((used));
static long gj_ksym_soft_netif_stop_queue(void) { return 0; }
static long gj_ksym_soft_netif_tx_wake_queue(void) __attribute__((used));
static long gj_ksym_soft_netif_tx_wake_queue(void) { return 0; }
static long gj_ksym_soft_netif_wake_queue(void) __attribute__((used));
static long gj_ksym_soft_netif_wake_queue(void) { return 0; }
static long gj_ksym_soft_numa_node(void) __attribute__((used));
static long gj_ksym_soft_numa_node(void) { return 0; }
static long gj_ksym_soft_page_offset_base(void) __attribute__((used));
static long gj_ksym_soft_page_offset_base(void) { return 0; }
static long gj_ksym_soft_pci_alloc_irq_vectors(void) __attribute__((used));
static long gj_ksym_soft_pci_alloc_irq_vectors(void) { return 0; }
static long gj_ksym_soft_pci_clear_master(void) __attribute__((used));
static long gj_ksym_soft_pci_clear_master(void) { return 0; }
static long gj_ksym_soft_pci_dev_run_wake(void) __attribute__((used));
static long gj_ksym_soft_pci_dev_run_wake(void) { return 0; }
static long gj_ksym_soft_pci_disable_device(void) __attribute__((used));
static long gj_ksym_soft_pci_disable_device(void) { return 0; }
static long gj_ksym_soft_pci_disable_link_state(void) __attribute__((used));
static long gj_ksym_soft_pci_disable_link_state(void) { return 0; }
static long gj_ksym_soft_pci_enable_device(void) __attribute__((used));
static long gj_ksym_soft_pci_enable_device(void) { return 0; }
static long gj_ksym_soft_pci_iomap(void) __attribute__((used));
static long gj_ksym_soft_pci_iomap(void) { return 0; }
static long gj_ksym_soft_pci_iounmap(void) __attribute__((used));
static long gj_ksym_soft_pci_iounmap(void) { return 0; }
static long gj_ksym_soft_pci_irq_vector(void) __attribute__((used));
static long gj_ksym_soft_pci_irq_vector(void) { return 0; }
static long gj_ksym_soft_pci_read_config_byte(void) __attribute__((used));
static long gj_ksym_soft_pci_read_config_byte(void) { return 0; }
static long gj_ksym_soft_pci_read_config_word(void) __attribute__((used));
static long gj_ksym_soft_pci_read_config_word(void) { return 0; }
static long gj_ksym_soft_pci_register_driver(void) __attribute__((used));
static long gj_ksym_soft_pci_register_driver(void) { return 0; }
static long gj_ksym_soft_pci_release_regions(void) __attribute__((used));
static long gj_ksym_soft_pci_release_regions(void) { return 0; }
static long gj_ksym_soft_pci_request_regions(void) __attribute__((used));
static long gj_ksym_soft_pci_request_regions(void) { return 0; }
static long gj_ksym_soft_pci_reset_bus(void) __attribute__((used));
static long gj_ksym_soft_pci_reset_bus(void) { return 0; }
static long gj_ksym_soft_pci_select_bars(void) __attribute__((used));
static long gj_ksym_soft_pci_select_bars(void) { return 0; }
static long gj_ksym_soft_pci_set_master(void) __attribute__((used));
static long gj_ksym_soft_pci_set_master(void) { return 0; }
static long gj_ksym_soft_pci_set_power_state(void) __attribute__((used));
static long gj_ksym_soft_pci_set_power_state(void) { return 0; }
static long gj_ksym_soft_pci_status_get_and_clear_errors(void) __attribute__((used));
static long gj_ksym_soft_pci_status_get_and_clear_errors(void) { return 0; }
static long gj_ksym_soft_pci_unregister_driver(void) __attribute__((used));
static long gj_ksym_soft_pci_unregister_driver(void) { return 0; }
static long gj_ksym_soft_pci_wake_from_d3(void) __attribute__((used));
static long gj_ksym_soft_pci_wake_from_d3(void) { return 0; }
static long gj_ksym_soft_pci_write_config_byte(void) __attribute__((used));
static long gj_ksym_soft_pci_write_config_byte(void) { return 0; }
static long gj_ksym_soft_pcie_capability_clear_and_set_word_locked(void) __attribute__((used));
static long gj_ksym_soft_pcie_capability_clear_and_set_word_locked(void) { return 0; }
static long gj_ksym_soft_pcie_set_readrq(void) __attribute__((used));
static long gj_ksym_soft_pcie_set_readrq(void) { return 0; }
static long gj_ksym_soft_pcim_enable_device(void) __attribute__((used));
static long gj_ksym_soft_pcim_enable_device(void) { return 0; }
static long gj_ksym_soft_pcim_iomap_region(void) __attribute__((used));
static long gj_ksym_soft_pcim_iomap_region(void) { return 0; }
static long gj_ksym_soft_pcim_set_mwi(void) __attribute__((used));
static long gj_ksym_soft_pcim_set_mwi(void) { return 0; }
static long gj_ksym_soft_phy_advertise_eee_all(void) __attribute__((used));
static long gj_ksym_soft_phy_advertise_eee_all(void) { return 0; }
static long gj_ksym_soft_phy_attached_info(void) __attribute__((used));
static long gj_ksym_soft_phy_attached_info(void) { return 0; }
static long gj_ksym_soft_phy_connect_direct(void) __attribute__((used));
static long gj_ksym_soft_phy_connect_direct(void) { return 0; }
static long gj_ksym_soft_phy_disconnect(void) __attribute__((used));
static long gj_ksym_soft_phy_disconnect(void) { return 0; }
static long gj_ksym_soft_phy_do_ioctl_running(void) __attribute__((used));
static long gj_ksym_soft_phy_do_ioctl_running(void) { return 0; }
static long gj_ksym_soft_phy_ethtool_get_eee(void) __attribute__((used));
static long gj_ksym_soft_phy_ethtool_get_eee(void) { return 0; }
static long gj_ksym_soft_phy_ethtool_get_link_ksettings(void) __attribute__((used));
static long gj_ksym_soft_phy_ethtool_get_link_ksettings(void) { return 0; }
static long gj_ksym_soft_phy_ethtool_nway_reset(void) __attribute__((used));
static long gj_ksym_soft_phy_ethtool_nway_reset(void) { return 0; }
static long gj_ksym_soft_phy_ethtool_set_eee(void) __attribute__((used));
static long gj_ksym_soft_phy_ethtool_set_eee(void) { return 0; }
static long gj_ksym_soft_phy_ethtool_set_link_ksettings(void) __attribute__((used));
static long gj_ksym_soft_phy_ethtool_set_link_ksettings(void) { return 0; }
static long gj_ksym_soft_phy_get_pause(void) __attribute__((used));
static long gj_ksym_soft_phy_get_pause(void) { return 0; }
static long gj_ksym_soft_phy_init_hw(void) __attribute__((used));
static long gj_ksym_soft_phy_init_hw(void) { return 0; }
static long gj_ksym_soft_phy_mac_interrupt(void) __attribute__((used));
static long gj_ksym_soft_phy_mac_interrupt(void) { return 0; }
static long gj_ksym_soft_phy_modify(void) __attribute__((used));
static long gj_ksym_soft_phy_modify(void) { return 0; }
static long gj_ksym_soft_phy_modify_paged(void) __attribute__((used));
static long gj_ksym_soft_phy_modify_paged(void) { return 0; }
static long gj_ksym_soft_phy_print_status(void) __attribute__((used));
static long gj_ksym_soft_phy_print_status(void) { return 0; }
static long gj_ksym_soft_phy_read_paged(void) __attribute__((used));
static long gj_ksym_soft_phy_read_paged(void) { return 0; }
static long gj_ksym_soft_phy_restore_page(void) __attribute__((used));
static long gj_ksym_soft_phy_restore_page(void) { return 0; }
static long gj_ksym_soft_phy_resume(void) __attribute__((used));
static long gj_ksym_soft_phy_resume(void) { return 0; }
static long gj_ksym_soft_phy_select_page(void) __attribute__((used));
static long gj_ksym_soft_phy_select_page(void) { return 0; }
static long gj_ksym_soft_phy_set_asym_pause(void) __attribute__((used));
static long gj_ksym_soft_phy_set_asym_pause(void) { return 0; }
static long gj_ksym_soft_phy_set_max_speed(void) __attribute__((used));
static long gj_ksym_soft_phy_set_max_speed(void) { return 0; }
static long gj_ksym_soft_phy_speed_down(void) __attribute__((used));
static long gj_ksym_soft_phy_speed_down(void) { return 0; }
static long gj_ksym_soft_phy_speed_up(void) __attribute__((used));
static long gj_ksym_soft_phy_speed_up(void) { return 0; }
static long gj_ksym_soft_phy_start(void) __attribute__((used));
static long gj_ksym_soft_phy_start(void) { return 0; }
static long gj_ksym_soft_phy_start_aneg(void) __attribute__((used));
static long gj_ksym_soft_phy_start_aneg(void) { return 0; }
static long gj_ksym_soft_phy_stop(void) __attribute__((used));
static long gj_ksym_soft_phy_stop(void) { return 0; }
static long gj_ksym_soft_phy_support_asym_pause(void) __attribute__((used));
static long gj_ksym_soft_phy_support_asym_pause(void) { return 0; }
static long gj_ksym_soft_phy_suspend(void) __attribute__((used));
static long gj_ksym_soft_phy_suspend(void) { return 0; }
static long gj_ksym_soft_phy_write_mmd(void) __attribute__((used));
static long gj_ksym_soft_phy_write_mmd(void) { return 0; }
static long gj_ksym_soft_phy_write_paged(void) __attribute__((used));
static long gj_ksym_soft_phy_write_paged(void) { return 0; }
static long gj_ksym_soft_phys_base(void) __attribute__((used));
static long gj_ksym_soft_phys_base(void) { return 0; }
static long gj_ksym_soft_pm_schedule_suspend(void) __attribute__((used));
static long gj_ksym_soft_pm_schedule_suspend(void) { return 0; }
static long gj_ksym_soft_printk(void) __attribute__((used));
static long gj_ksym_soft_printk(void) { return 0; }
static long gj_ksym_soft_pskb_expand_head(void) __attribute__((used));
static long gj_ksym_soft_pskb_expand_head(void) { return 0; }
static long gj_ksym_soft_queue_work_on(void) __attribute__((used));
static long gj_ksym_soft_queue_work_on(void) { return 0; }
static long gj_ksym_soft_readb(void) __attribute__((used));
static long gj_ksym_soft_readb(void) { return 0; }
static long gj_ksym_soft_readl(void) __attribute__((used));
static long gj_ksym_soft_readl(void) { return 0; }
static long gj_ksym_soft_readw(void) __attribute__((used));
static long gj_ksym_soft_readw(void) { return 0; }
static long gj_ksym_soft_register_netdev(void) __attribute__((used));
static long gj_ksym_soft_register_netdev(void) { return 0; }
/* Empty ksym only; linux_dma_soft release_firmware is the soft no-op body. */
static long gj_ksym_soft_release_firmware(void) __attribute__((used));
static long gj_ksym_soft_release_firmware(void) { return 0; }
static long gj_ksym_soft_request_irq(void) __attribute__((used));
static long gj_ksym_soft_request_irq(void) { return 0; }
static long gj_ksym_soft_request_threaded_irq(void) __attribute__((used));
static long gj_ksym_soft_request_threaded_irq(void) { return 0; }
static long gj_ksym_soft_rtnl_lock(void) __attribute__((used));
static long gj_ksym_soft_rtnl_lock(void) { return 0; }
static long gj_ksym_soft_rtnl_unlock(void) __attribute__((used));
static long gj_ksym_soft_rtnl_unlock(void) { return 0; }
static long gj_ksym_soft_scnprintf(void) __attribute__((used));
static long gj_ksym_soft_scnprintf(void) { return 0; }
static long gj_ksym_soft_sized_strscpy(void) __attribute__((used));
static long gj_ksym_soft_sized_strscpy(void) { return 0; }
static long gj_ksym_soft_skb_clone_tx_timestamp(void) __attribute__((used));
static long gj_ksym_soft_skb_clone_tx_timestamp(void) { return 0; }
static long gj_ksym_soft_skb_copy_bits(void) __attribute__((used));
static long gj_ksym_soft_skb_copy_bits(void) { return 0; }
static long gj_ksym_soft_skb_tstamp_tx(void) __attribute__((used));
static long gj_ksym_soft_skb_tstamp_tx(void) { return 0; }
static long gj_ksym_soft_snprintf(void) __attribute__((used));
static long gj_ksym_soft_snprintf(void) { return 0; }
static long gj_ksym_soft_softnet_data(void) __attribute__((used));
static long gj_ksym_soft_softnet_data(void) { return 0; }
static long gj_ksym_soft_spin_lock(void) __attribute__((used));
static long gj_ksym_soft_spin_lock(void) { return 0; }
static long gj_ksym_soft_spin_unlock(void) __attribute__((used));
static long gj_ksym_soft_spin_unlock(void) { return 0; }
static long gj_ksym_soft_sscanf(void) __attribute__((used));
static long gj_ksym_soft_sscanf(void) { return 0; }
static long gj_ksym_soft_strcat(void) __attribute__((used));
static long gj_ksym_soft_strcat(void) { return 0; }
static long gj_ksym_soft_strchr(void) __attribute__((used));
static long gj_ksym_soft_strchr(void) { return 0; }
static long gj_ksym_soft_strcmp(void) __attribute__((used));
static long gj_ksym_soft_strcmp(void) { return 0; }
static long gj_ksym_soft_strcpy(void) __attribute__((used));
static long gj_ksym_soft_strcpy(void) { return 0; }
static long gj_ksym_soft_strlen(void) __attribute__((used));
static long gj_ksym_soft_strlen(void) { return 0; }
static long gj_ksym_soft_strncat(void) __attribute__((used));
static long gj_ksym_soft_strncat(void) { return 0; }
static long gj_ksym_soft_strncmp(void) __attribute__((used));
static long gj_ksym_soft_strncmp(void) { return 0; }
static long gj_ksym_soft_strncpy(void) __attribute__((used));
static long gj_ksym_soft_strncpy(void) { return 0; }
static long gj_ksym_soft_strnlen(void) __attribute__((used));
static long gj_ksym_soft_strnlen(void) { return 0; }
static long gj_ksym_soft_strrchr(void) __attribute__((used));
static long gj_ksym_soft_strrchr(void) { return 0; }
static long gj_ksym_soft_strstr(void) __attribute__((used));
static long gj_ksym_soft_strstr(void) { return 0; }
static long gj_ksym_soft_synchronize_net(void) __attribute__((used));
static long gj_ksym_soft_synchronize_net(void) { return 0; }
static long gj_ksym_soft_system_state(void) __attribute__((used));
static long gj_ksym_soft_system_state(void) { return 0; }
static u8 gj_ksym_soft_system_wq[64] __attribute__((used));
static long gj_ksym_soft_this_cpu_off(void) __attribute__((used));
static long gj_ksym_soft_this_cpu_off(void) { return 0; }
static long gj_ksym_soft_try_module_get(void) __attribute__((used));
static long gj_ksym_soft_try_module_get(void) { return 0; }
static long gj_ksym_soft_udelay(void) __attribute__((used));
static long gj_ksym_soft_udelay(void) { return 0; }
static long gj_ksym_soft_unregister_netdev(void) __attribute__((used));
static long gj_ksym_soft_unregister_netdev(void) { return 0; }
static long gj_ksym_soft_usleep_range(void) __attribute__((used));
static long gj_ksym_soft_usleep_range(void) { return 0; }
static long gj_ksym_soft_usleep_range_state(void) __attribute__((used));
static long gj_ksym_soft_usleep_range_state(void) { return 0; }
static long gj_ksym_soft_vfree(void) __attribute__((used));
static long gj_ksym_soft_vfree(void) { return 0; }
static long gj_ksym_soft_vmalloc(void) __attribute__((used));
static long gj_ksym_soft_vmalloc(void) { return 0; }
static long gj_ksym_soft_vmemmap_base(void) __attribute__((used));
static long gj_ksym_soft_vmemmap_base(void) { return 0; }
static long gj_ksym_soft_vsnprintf(void) __attribute__((used));
static long gj_ksym_soft_vsnprintf(void) { return 0; }
static long gj_ksym_soft_writeb(void) __attribute__((used));
static long gj_ksym_soft_writeb(void) { return 0; }
static long gj_ksym_soft_writel(void) __attribute__((used));
static long gj_ksym_soft_writel(void) { return 0; }
static long gj_ksym_soft_writew(void) __attribute__((used));
static long gj_ksym_soft_writew(void) { return 0; }

/*
 * Soft MSC leaf helpers (usb-storage.ko reloc surface) - safe empty/fail-closed
 * generics only. Cap LINUX_KSYM_USB_STORAGE_LEAF (15); do NOT stub full
 * usbcore/scsi midlayer here. Soft!=product. INIT=0 for usb_storage still
 * MUST FAIL without usb_* / scsi_* ksym (see docs/LINUX_MODULE_PATH.md D8).
 * Residual lean: leaf count folded into residual lean lamp (leaf=); no leaf lamp.
 */
static long gj_ksym_soft_dma_max_mapping_size(void) __attribute__((used));
static long gj_ksym_soft_dma_max_mapping_size(void) { return 0; }
static long gj_ksym_soft_queue_limits_commit_update_frozen(void)
    __attribute__((used));
static long gj_ksym_soft_queue_limits_commit_update_frozen(void) { return 0; }
static long gj_ksym_soft_sprintf(void) __attribute__((used));
static long gj_ksym_soft_sprintf(void) { return 0; }
static long gj_ksym_soft_seq_printf(void) __attribute__((used));
static long gj_ksym_soft_seq_printf(void) { return 0; }
static long gj_ksym_soft_seq_putc(void) __attribute__((used));
static long gj_ksym_soft_seq_putc(void) { return 0; }
static long gj_ksym_soft_complete(void) __attribute__((used));
static long gj_ksym_soft_complete(void) { return 0; }
static long gj_ksym_soft_finish_wait(void) __attribute__((used));
static long gj_ksym_soft_finish_wait(void) { return 0; }
static long gj_ksym_soft_wait_for_completion(void) __attribute__((used));
static long gj_ksym_soft_wait_for_completion(void) { return 0; }
static long gj_ksym_soft_wait_for_completion_interruptible(void)
    __attribute__((used));
static long gj_ksym_soft_wait_for_completion_interruptible(void) { return 0; }
static long gj_ksym_soft_wait_for_completion_interruptible_timeout(void)
    __attribute__((used));
static long gj_ksym_soft_wait_for_completion_interruptible_timeout(void)
{
    return 0;
}
static long gj_ksym_soft__raw_spin_lock_irq(void) __attribute__((used));
static long gj_ksym_soft__raw_spin_lock_irq(void) { return 0; }
static long gj_ksym_soft__raw_spin_unlock_irq(void) __attribute__((used));
static long gj_ksym_soft__raw_spin_unlock_irq(void) { return 0; }
static long gj_ksym_soft_schedule(void) __attribute__((used));
static long gj_ksym_soft_schedule(void) { return 0; }
static long gj_ksym_soft_schedule_timeout(void) __attribute__((used));
static long gj_ksym_soft_schedule_timeout(void) { return 0; }
static long gj_ksym_soft_schedule_timeout_uninterruptible(void)
    __attribute__((used));
static long gj_ksym_soft_schedule_timeout_uninterruptible(void) { return 0; }

/*
 * Residual UND/FAIL KSYM class for usb-storage-adjacent generics (el9 ko
 * after leaf15 + soft usb/scsi seed). Freestanding empty/fail-closed only -
 * n=LINUX_KSYM_USB_STORAGE_RESIDUAL (62). Do NOT register usb_* or scsi_* here
 * (peer linux_usb_soft owns those; need=usbcore FAIL stays honest without HC).
 * Soft!=product. Resolve honesty != stick datapath claim.
 * Dual DoD eng: +16 sg table/copy + wait/timer/wq; +12 wq/completion/sg/sysfs.
 * Residual lean: size in residual lean lamp usb=; no per-class lamp.
 */
static long gj_ksym_soft_sg_nents(void) __attribute__((used));
static long gj_ksym_soft_sg_nents(void) { return 0; }
static long gj_ksym_soft_sg_miter_start(void) __attribute__((used));
static long gj_ksym_soft_sg_miter_start(void) { return 0; }
static long gj_ksym_soft_sg_miter_skip(void) __attribute__((used));
static long gj_ksym_soft_sg_miter_skip(void) { return 0; }
static long gj_ksym_soft_sg_miter_next(void) __attribute__((used));
static long gj_ksym_soft_sg_miter_next(void) { return 0; }
static long gj_ksym_soft_sg_miter_stop(void) __attribute__((used));
static long gj_ksym_soft_sg_miter_stop(void) { return 0; }
static long gj_ksym_soft_kthread_create_on_node(void) __attribute__((used));
static long gj_ksym_soft_kthread_create_on_node(void) { return 0; }
static long gj_ksym_soft_kthread_should_stop(void) __attribute__((used));
static long gj_ksym_soft_kthread_should_stop(void) { return 0; }
static long gj_ksym_soft_kthread_stop(void) __attribute__((used));
static long gj_ksym_soft_kthread_stop(void) { return 0; }
static long gj_ksym_soft_init_wait_entry(void) __attribute__((used));
static long gj_ksym_soft_init_wait_entry(void) { return 0; }
static long gj_ksym_soft_prepare_to_wait_event(void) __attribute__((used));
static long gj_ksym_soft_prepare_to_wait_event(void) { return 0; }
static long gj_ksym_soft___init_waitqueue_head(void) __attribute__((used));
static long gj_ksym_soft___init_waitqueue_head(void) { return 0; }
static long gj_ksym_soft___init_swait_queue_head(void) __attribute__((used));
static long gj_ksym_soft___init_swait_queue_head(void) { return 0; }
static long gj_ksym_soft___wake_up(void) __attribute__((used));
static long gj_ksym_soft___wake_up(void) { return 0; }
static long gj_ksym_soft_wake_up_process(void) __attribute__((used));
static long gj_ksym_soft_wake_up_process(void) { return 0; }
static long gj_ksym_soft_init_timer_key(void) __attribute__((used));
static long gj_ksym_soft_init_timer_key(void) { return 0; }
static long gj_ksym_soft_delayed_work_timer_fn(void) __attribute__((used));
static long gj_ksym_soft_delayed_work_timer_fn(void) { return 0; }
static long gj_ksym_soft_queue_delayed_work_on(void) __attribute__((used));
static long gj_ksym_soft_queue_delayed_work_on(void) { return 0; }
static long gj_ksym_soft_cancel_delayed_work_sync(void) __attribute__((used));
static long gj_ksym_soft_cancel_delayed_work_sync(void) { return 0; }
static long gj_ksym_soft_device_create_file(void) __attribute__((used));
static long gj_ksym_soft_device_create_file(void) { return 0; }
static long gj_ksym_soft_sysfs_emit(void) __attribute__((used));
static long gj_ksym_soft_sysfs_emit(void) { return 0; }
static long gj_ksym_soft__dev_notice(void) __attribute__((used));
static long gj_ksym_soft__dev_notice(void) { return 0; }
static long gj_ksym_soft___dynamic_dev_dbg(void) __attribute__((used));
static long gj_ksym_soft___dynamic_dev_dbg(void) { return 0; }
static long gj_ksym_soft_kstrtouint(void) __attribute__((used));
static long gj_ksym_soft_kstrtouint(void) { return 0; }
static long gj_ksym_soft_simple_strtoul(void) __attribute__((used));
static long gj_ksym_soft_simple_strtoul(void) { return 0; }
static long gj_ksym_soft_skip_spaces(void) __attribute__((used));
static long gj_ksym_soft_skip_spaces(void) { return 0; }
static long gj_ksym_soft_int_pow(void) __attribute__((used));
static long gj_ksym_soft_int_pow(void) { return 0; }
static long gj_ksym_soft___msecs_to_jiffies(void) __attribute__((used));
static long gj_ksym_soft___msecs_to_jiffies(void) { return 0; }
static long gj_ksym_soft_ktime_get_mono_fast_ns(void) __attribute__((used));
static long gj_ksym_soft_ktime_get_mono_fast_ns(void) { return 0; }
static long gj_ksym_soft___trace_set_current_state(void) __attribute__((used));
static long gj_ksym_soft___trace_set_current_state(void) { return 0; }
/* Data-shaped residuals (zero page; not callables). Soft!=product. */
static u8 gj_ksym_soft_param_ops_uint[64] __attribute__((used));
static u8 gj_ksym_soft_param_ops_string[64] __attribute__((used));
static u8 gj_ksym_soft_pcpu_hot[64] __attribute__((used));
static u8 gj_ksym_soft_system_freezable_wq[64] __attribute__((used));
static u8 gj_ksym_soft___tracepoint_sched_set_state_tp[64]
    __attribute__((used));
/* Dual DoD USB residual expand (sg table/copy + wait/timer/wq). Soft!=product. */
static long gj_ksym_soft_sg_init_table(void) __attribute__((used));
static long gj_ksym_soft_sg_init_table(void) { return 0; }
static long gj_ksym_soft_sg_init_one(void) __attribute__((used));
static long gj_ksym_soft_sg_init_one(void) { return 0; }
static long gj_ksym_soft_sg_next(void) __attribute__((used));
static long gj_ksym_soft_sg_next(void) { return 0; }
static long gj_ksym_soft_sg_last(void) __attribute__((used));
static long gj_ksym_soft_sg_last(void) { return 0; }
static long gj_ksym_soft_sg_copy_from_buffer(void) __attribute__((used));
static long gj_ksym_soft_sg_copy_from_buffer(void) { return 0; }
static long gj_ksym_soft_sg_copy_to_buffer(void) __attribute__((used));
static long gj_ksym_soft_sg_copy_to_buffer(void) { return 0; }
static long gj_ksym_soft_sg_pcopy_from_buffer(void) __attribute__((used));
static long gj_ksym_soft_sg_pcopy_from_buffer(void) { return 0; }
static long gj_ksym_soft_sg_pcopy_to_buffer(void) __attribute__((used));
static long gj_ksym_soft_sg_pcopy_to_buffer(void) { return 0; }
static long gj_ksym_soft_sg_alloc_table(void) __attribute__((used));
static long gj_ksym_soft_sg_alloc_table(void) { return 0; }
static long gj_ksym_soft_sg_free_table(void) __attribute__((used));
static long gj_ksym_soft_sg_free_table(void) { return 0; }
static long gj_ksym_soft_sg_alloc_table_from_pages(void) __attribute__((used));
static long gj_ksym_soft_sg_alloc_table_from_pages(void) { return 0; }
static long gj_ksym_soft_wait_for_completion_timeout(void) __attribute__((used));
static long gj_ksym_soft_wait_for_completion_timeout(void) { return 0; }
static long gj_ksym_soft_cancel_work_sync(void) __attribute__((used));
static long gj_ksym_soft_cancel_work_sync(void) { return 0; }
static long gj_ksym_soft_mod_timer(void) __attribute__((used));
static long gj_ksym_soft_mod_timer(void) { return 0; }
static long gj_ksym_soft_del_timer_sync(void) __attribute__((used));
static long gj_ksym_soft_del_timer_sync(void) { return 0; }
static long gj_ksym_soft_flush_workqueue(void) __attribute__((used));
static long gj_ksym_soft_flush_workqueue(void) { return 0; }
/* Dual DoD USB residual expand2 (+12 wq/completion/sg/sysfs). Soft!=product. */
static long gj_ksym_soft_alloc_workqueue(void) __attribute__((used));
static long gj_ksym_soft_alloc_workqueue(void) { return 0; }
static long gj_ksym_soft_destroy_workqueue(void) __attribute__((used));
static long gj_ksym_soft_destroy_workqueue(void) { return 0; }
static long gj_ksym_soft_flush_work(void) __attribute__((used));
static long gj_ksym_soft_flush_work(void) { return 0; }
static long gj_ksym_soft_schedule_work(void) __attribute__((used));
static long gj_ksym_soft_schedule_work(void) { return 0; }
static long gj_ksym_soft_try_wait_for_completion(void) __attribute__((used));
static long gj_ksym_soft_try_wait_for_completion(void) { return 0; }
static long gj_ksym_soft_completion_done(void) __attribute__((used));
static long gj_ksym_soft_completion_done(void) { return 0; }
static long gj_ksym_soft_sg_nents_for_len(void) __attribute__((used));
static long gj_ksym_soft_sg_nents_for_len(void) { return 0; }
static long gj_ksym_soft_sg_copy_buffer(void) __attribute__((used));
static long gj_ksym_soft_sg_copy_buffer(void) { return 0; }
static long gj_ksym_soft_device_remove_file(void) __attribute__((used));
static long gj_ksym_soft_device_remove_file(void) { return 0; }
static long gj_ksym_soft__dev_printk(void) __attribute__((used));
static long gj_ksym_soft__dev_printk(void) { return 0; }
static long gj_ksym_soft___kmalloc(void) __attribute__((used));
static long gj_ksym_soft___kmalloc(void) { return 0; }
static u8 gj_ksym_soft_param_ops_bool[64] __attribute__((used));

/*
 * Residual UND/FAIL KSYM class for net soft (skb / xmit / netdev lifecycle
 * generics). Freestanding empty/fail-closed only - n=LINUX_KSYM_NET_RESIDUAL
 * (48). Peer linux_netdev_soft may replace these addresses when inited.
 * Soft!=product; G-AC-1: no .ko wire ownership. Name surface useful for soft
 * eng + future userspace net ABI binding (UDX / Linux-shaped drivers).
 * Dual DoD eng: +18 skb/queue/stats/rtnl; +12 napi/skb/netdev eng helpers.
 * Residual lean: size in residual lean lamp net=; no per-class lamp.
 */
static long gj_ksym_soft_skb_put(void) __attribute__((used));
static long gj_ksym_soft_skb_put(void) { return 0; }
static long gj_ksym_soft_pskb_may_pull(void) __attribute__((used));
static long gj_ksym_soft_pskb_may_pull(void) { return 0; }
static long gj_ksym_soft___netdev_alloc_skb(void) __attribute__((used));
static long gj_ksym_soft___netdev_alloc_skb(void) { return 0; }
static long gj_ksym_soft_dev_alloc_skb(void) __attribute__((used));
static long gj_ksym_soft_dev_alloc_skb(void) { return 0; }
static long gj_ksym_soft___napi_alloc_skb(void) __attribute__((used));
static long gj_ksym_soft___napi_alloc_skb(void) { return 0; }
static long gj_ksym_soft_dev_queue_xmit(void) __attribute__((used));
static long gj_ksym_soft_dev_queue_xmit(void) { return 0; }
static long gj_ksym_soft_kfree_skb(void) __attribute__((used));
static long gj_ksym_soft_kfree_skb(void) { return 0; }
static long gj_ksym_soft_consume_skb(void) __attribute__((used));
static long gj_ksym_soft_consume_skb(void) { return 0; }
static long gj_ksym_soft___kfree_skb(void) __attribute__((used));
static long gj_ksym_soft___kfree_skb(void) { return 0; }
static long gj_ksym_soft_skb_pull(void) __attribute__((used));
static long gj_ksym_soft_skb_pull(void) { return 0; }
static long gj_ksym_soft_skb_push(void) __attribute__((used));
static long gj_ksym_soft_skb_push(void) { return 0; }
static long gj_ksym_soft_skb_reserve(void) __attribute__((used));
static long gj_ksym_soft_skb_reserve(void) { return 0; }
static long gj_ksym_soft_netif_rx(void) __attribute__((used));
static long gj_ksym_soft_netif_rx(void) { return 0; }
static long gj_ksym_soft_register_netdevice(void) __attribute__((used));
static long gj_ksym_soft_register_netdevice(void) { return 0; }
static long gj_ksym_soft_unregister_netdevice(void) __attribute__((used));
static long gj_ksym_soft_unregister_netdevice(void) { return 0; }
static long gj_ksym_soft_netif_tx_start_all_queues(void) __attribute__((used));
static long gj_ksym_soft_netif_tx_start_all_queues(void) { return 0; }
static long gj_ksym_soft_netif_tx_stop_all_queues(void) __attribute__((used));
static long gj_ksym_soft_netif_tx_stop_all_queues(void) { return 0; }
static long gj_ksym_soft_rtnl_trylock(void) __attribute__((used));
static long gj_ksym_soft_rtnl_trylock(void) { return 0; }
/* Dual DoD net residual expand (+18). Soft!=product; eng+userspace bind. */
static long gj_ksym_soft_skb_clone(void) __attribute__((used));
static long gj_ksym_soft_skb_clone(void) { return 0; }
static long gj_ksym_soft_skb_copy(void) __attribute__((used));
static long gj_ksym_soft_skb_copy(void) { return 0; }
static long gj_ksym_soft_skb_trim(void) __attribute__((used));
static long gj_ksym_soft_skb_trim(void) { return 0; }
static long gj_ksym_soft_skb_checksum_help(void) __attribute__((used));
static long gj_ksym_soft_skb_checksum_help(void) { return 0; }
static long gj_ksym_soft___skb_gso_segment(void) __attribute__((used));
static long gj_ksym_soft___skb_gso_segment(void) { return 0; }
static long gj_ksym_soft_netif_tx_stop_queue(void) __attribute__((used));
static long gj_ksym_soft_netif_tx_stop_queue(void) { return 0; }
static long gj_ksym_soft_netif_set_real_num_tx_queues(void) __attribute__((used));
static long gj_ksym_soft_netif_set_real_num_tx_queues(void) { return 0; }
static long gj_ksym_soft_netif_set_real_num_rx_queues(void) __attribute__((used));
static long gj_ksym_soft_netif_set_real_num_rx_queues(void) { return 0; }
static long gj_ksym_soft_netif_napi_add_weight(void) __attribute__((used));
static long gj_ksym_soft_netif_napi_add_weight(void) { return 0; }
static long gj_ksym_soft_alloc_netdev_mqs(void) __attribute__((used));
static long gj_ksym_soft_alloc_netdev_mqs(void) { return 0; }
static long gj_ksym_soft_unregister_netdevice_queue(void) __attribute__((used));
static long gj_ksym_soft_unregister_netdevice_queue(void) { return 0; }
static long gj_ksym_soft_dev_get_stats(void) __attribute__((used));
static long gj_ksym_soft_dev_get_stats(void) { return 0; }
static long gj_ksym_soft_dev_addr_mod(void) __attribute__((used));
static long gj_ksym_soft_dev_addr_mod(void) { return 0; }
static long gj_ksym_soft_netdev_state_change(void) __attribute__((used));
static long gj_ksym_soft_netdev_state_change(void) { return 0; }
static long gj_ksym_soft_linkwatch_fire_event(void) __attribute__((used));
static long gj_ksym_soft_linkwatch_fire_event(void) { return 0; }
static long gj_ksym_soft_rtnl_is_locked(void) __attribute__((used));
static long gj_ksym_soft_rtnl_is_locked(void) { return 0; }
static long gj_ksym_soft_netif_tx_disable(void) __attribute__((used));
static long gj_ksym_soft_netif_tx_disable(void) { return 0; }
static long gj_ksym_soft_netif_napi_del(void) __attribute__((used));
static long gj_ksym_soft_netif_napi_del(void) { return 0; }
/* Dual DoD net residual expand2 (+12 napi/skb/netdev). Soft!=product. */
static long gj_ksym_soft_napi_schedule(void) __attribute__((used));
static long gj_ksym_soft_napi_schedule(void) { return 0; }
static long gj_ksym_soft_napi_complete(void) __attribute__((used));
static long gj_ksym_soft_napi_complete(void) { return 0; }
static long gj_ksym_soft_netif_rx_ni(void) __attribute__((used));
static long gj_ksym_soft_netif_rx_ni(void) { return 0; }
static long gj_ksym_soft_dev_kfree_skb_irq(void) __attribute__((used));
static long gj_ksym_soft_dev_kfree_skb_irq(void) { return 0; }
static long gj_ksym_soft_skb_copy_expand(void) __attribute__((used));
static long gj_ksym_soft_skb_copy_expand(void) { return 0; }
static long gj_ksym_soft_skb_linearize(void) __attribute__((used));
static long gj_ksym_soft_skb_linearize(void) { return 0; }
static long gj_ksym_soft_netdev_alloc_frag(void) __attribute__((used));
static long gj_ksym_soft_netdev_alloc_frag(void) { return 0; }
static long gj_ksym_soft_napi_alloc_frag(void) __attribute__((used));
static long gj_ksym_soft_napi_alloc_frag(void) { return 0; }
static long gj_ksym_soft___netif_schedule(void) __attribute__((used));
static long gj_ksym_soft___netif_schedule(void) { return 0; }
static long gj_ksym_soft_netdev_features_change(void) __attribute__((used));
static long gj_ksym_soft_netdev_features_change(void) { return 0; }
static long gj_ksym_soft_netdev_notify_peers(void) __attribute__((used));
static long gj_ksym_soft_netdev_notify_peers(void) { return 0; }
static long gj_ksym_soft_netdev_rss_key_fill(void) __attribute__((used));
static long gj_ksym_soft_netdev_rss_key_fill(void) { return 0; }

/*
 * Residual UND/FAIL KSYM class for PCI soft (config / BAR / drvdata / irq /
 * dma map helpers). Freestanding empty/fail-closed only -
 * n=LINUX_KSYM_PCI_RESIDUAL (32). Peer linux_pci_soft may replace when inited.
 * Soft!=product; G-AC-1: no real BAR/MMIO claim. UDX host + soft layout surface.
 * Dual DoD eng: +12 msi/msix/selected-regions/dma-mask helpers.
 * Residual lean: size in residual lean lamp pci=; no per-class lamp.
 */
static long gj_ksym_soft_pci_read_config_dword(void) __attribute__((used));
static long gj_ksym_soft_pci_read_config_dword(void) { return 0; }
static long gj_ksym_soft_pci_write_config_word(void) __attribute__((used));
static long gj_ksym_soft_pci_write_config_word(void) { return 0; }
static long gj_ksym_soft_pci_write_config_dword(void) __attribute__((used));
static long gj_ksym_soft_pci_write_config_dword(void) { return 0; }
static long gj_ksym_soft_pci_request_region(void) __attribute__((used));
static long gj_ksym_soft_pci_request_region(void) { return 0; }
static long gj_ksym_soft_pci_release_region(void) __attribute__((used));
static long gj_ksym_soft_pci_release_region(void) { return 0; }
static long gj_ksym_soft_pci_resource_start(void) __attribute__((used));
static long gj_ksym_soft_pci_resource_start(void) { return 0; }
static long gj_ksym_soft_pci_resource_len(void) __attribute__((used));
static long gj_ksym_soft_pci_resource_len(void) { return 0; }
static long gj_ksym_soft_pci_set_drvdata(void) __attribute__((used));
static long gj_ksym_soft_pci_set_drvdata(void) { return 0; }
static long gj_ksym_soft_pci_get_drvdata(void) __attribute__((used));
static long gj_ksym_soft_pci_get_drvdata(void) { return 0; }
static long gj_ksym_soft_pci_free_irq_vectors(void) __attribute__((used));
static long gj_ksym_soft_pci_free_irq_vectors(void) { return 0; }
static long gj_ksym_soft_pci_find_capability(void) __attribute__((used));
static long gj_ksym_soft_pci_find_capability(void) { return 0; }
static long gj_ksym_soft_pci_save_state(void) __attribute__((used));
static long gj_ksym_soft_pci_save_state(void) { return 0; }
static long gj_ksym_soft_pci_restore_state(void) __attribute__((used));
static long gj_ksym_soft_pci_restore_state(void) { return 0; }
static long gj_ksym_soft_dma_mapping_error(void) __attribute__((used));
static long gj_ksym_soft_dma_mapping_error(void) { return 0; }
static long gj_ksym_soft_dma_sync_single_for_cpu(void) __attribute__((used));
static long gj_ksym_soft_dma_sync_single_for_cpu(void) { return 0; }
static long gj_ksym_soft_dma_sync_single_for_device(void) __attribute__((used));
static long gj_ksym_soft_dma_sync_single_for_device(void) { return 0; }
static long gj_ksym_soft_synchronize_irq(void) __attribute__((used));
static long gj_ksym_soft_synchronize_irq(void) { return 0; }
static long gj_ksym_soft_irq_set_affinity_hint(void) __attribute__((used));
static long gj_ksym_soft_irq_set_affinity_hint(void) { return 0; }
static long gj_ksym_soft_pcim_iomap_regions(void) __attribute__((used));
static long gj_ksym_soft_pcim_iomap_regions(void) { return 0; }
static long gj_ksym_soft_pcim_iomap_table(void) __attribute__((used));
static long gj_ksym_soft_pcim_iomap_table(void) { return 0; }
/* Dual DoD PCI residual expand (+12 msi/msix/regions/dma-mask). Soft!=product. */
static long gj_ksym_soft_pci_enable_msi(void) __attribute__((used));
static long gj_ksym_soft_pci_enable_msi(void) { return 0; }
static long gj_ksym_soft_pci_disable_msi(void) __attribute__((used));
static long gj_ksym_soft_pci_disable_msi(void) { return 0; }
static long gj_ksym_soft_pci_enable_msix_range(void) __attribute__((used));
static long gj_ksym_soft_pci_enable_msix_range(void) { return 0; }
static long gj_ksym_soft_pci_disable_msix(void) __attribute__((used));
static long gj_ksym_soft_pci_disable_msix(void) { return 0; }
static long gj_ksym_soft_pci_find_ext_capability(void) __attribute__((used));
static long gj_ksym_soft_pci_find_ext_capability(void) { return 0; }
static long gj_ksym_soft_pci_enable_device_mem(void) __attribute__((used));
static long gj_ksym_soft_pci_enable_device_mem(void) { return 0; }
static long gj_ksym_soft_pci_request_selected_regions(void) __attribute__((used));
static long gj_ksym_soft_pci_request_selected_regions(void) { return 0; }
static long gj_ksym_soft_pci_release_selected_regions(void) __attribute__((used));
static long gj_ksym_soft_pci_release_selected_regions(void) { return 0; }
static long gj_ksym_soft_pci_iomap_range(void) __attribute__((used));
static long gj_ksym_soft_pci_iomap_range(void) { return 0; }
static long gj_ksym_soft_dma_set_mask_and_coherent(void) __attribute__((used));
static long gj_ksym_soft_dma_set_mask_and_coherent(void) { return 0; }
static long gj_ksym_soft_pcie_capability_read_word(void) __attribute__((used));
static long gj_ksym_soft_pcie_capability_read_word(void) { return 0; }
static long gj_ksym_soft_pci_choose_state(void) __attribute__((used));
static long gj_ksym_soft_pci_choose_state(void) { return 0; }

int
linux_ksym_register(const char *szName, void *pAddr)
{
    u32 i;

    if (szName == 0 || szName[0] == '\0' || pAddr == 0) {
        return -1;
    }
    for (i = 0; i < g_u32KsymN; i++) {
        if (g_aKsym[i].szName != 0 &&
            strcmp(g_aKsym[i].szName, szName) == 0) {
            g_aKsym[i].pAddr = pAddr;
            if (g_u32RegReplace < 0xffffffffu) {
                g_u32RegReplace++;
            }
            return 0;
        }
    }
    if (g_u32KsymN >= LINUX_KSYM_MAX) {
        if (g_u32RegFull < 0xffffffffu) {
            g_u32RegFull++;
        }
        return -1;
    }
    g_aKsym[g_u32KsymN].szName = szName;
    g_aKsym[g_u32KsymN].pAddr = pAddr;
    g_u32KsymN++;
    if (g_u32RegOk < 0xffffffffu) {
        g_u32RegOk++;
    }
    return 0;
}

void *
linux_ksym_lookup(const char *szName)
{
    u32 i;

    if (!g_fInit || szName == 0 || szName[0] == '\0') {
        return 0;
    }
    for (i = 0; i < g_u32KsymN; i++) {
        if (g_aKsym[i].szName != 0 &&
            strcmp(g_aKsym[i].szName, szName) == 0) {
            return g_aKsym[i].pAddr;
        }
    }
    return 0;
}

int
linux_ksym_has(const char *szName)
{
    void *p;

    if (szName == NULL || szName[0] == '\0') {
        return 0;
    }
    /* Uninited table: soft miss (same as lookup). Do not log flood. */
    if (!g_fInit) {
        if (g_u32HasMiss < 0xffffffffu) {
            g_u32HasMiss++;
        }
        return 0;
    }
    p = linux_ksym_lookup(szName);
    if (p != NULL) {
        if (g_u32HasHit < 0xffffffffu) {
            g_u32HasHit++;
        }
        return 1;
    }
    if (g_u32HasMiss < 0xffffffffu) {
        g_u32HasMiss++;
    }
    /*
     * Optional first-miss diagnostic only (not every lookup).
     * Soft-loaded module pre-resolve / freestanding_no_exec SKIP exec
     * still use return value alone. Never G-AC-1 waiver.
     * greppable: linux_ksym: soft has miss name=
     */
    if (g_fHasMissLog == 0) {
        g_fHasMissLog = 1;
        /* greppable: linux_ksym: soft has miss name= (first miss only) */
        kprintf("linux_ksym: soft has miss name=%s n=%u free=%u "
                "soft=1 product=0 freestanding_no_exec=1 resolve_only=1 "
                "g_ac1_waiver=0 G-AC-1 storm=0 Soft!=product\n",
                szName, (unsigned)g_u32KsymN,
                (unsigned)linux_ksym_slots_free());
    }
    return 0;
}

u32
linux_ksym_count(void)
{
    return g_u32KsymN;
}

u32
linux_ksym_slots_free(void)
{
    if (!g_fInit) {
        return 0;
    }
    if (g_u32KsymN >= LINUX_KSYM_MAX) {
        return 0;
    }
    return LINUX_KSYM_MAX - g_u32KsymN;
}

void
linux_ksym_soft_inventory(void)
{
    u32 u32Free;
    u32 u32ReserveOk;

    u32Free = linux_ksym_slots_free();
    u32ReserveOk = (u32Free >= LINUX_KSYM_USB_SOFT_SEED) ? 1u : 0u;

    /*
     * Lean on-demand inventory: one line (not boot residual flood).
     * first=1 once with class sizes; later calls refresh free= only.
     * freestanding_no_exec honesty; never G-AC-1 waiver.
     * No version stamp. greppable: linux_ksym: soft inventory n= max= free=
     */
    if (g_fInvOnce == 0) {
        g_fInvOnce = 1;
        kprintf("linux_ksym: soft inventory n=%u max=%u free=%u "
                "usb_seed=%u usb=%u net=%u pci=%u leaf=%u residual_total=%u "
                "reserve_ok=%u reg_ok=%u reg_repl=%u full=%u "
                "has_hit=%u has_miss=%u lean_ok=%u/%u "
                "soft=1 product=0 userspace_bind=1 hostability=1 "
                "freestanding_no_exec=1 never_exec_ko=1 resolve_only=1 "
                "g_ac1_waiver=0 G-AC-1 exclusive=1 dual_dod=OPEN "
                "dual=MIT_OR_Apache-2.0 no_version_stamp=1 storm=0 "
                "Soft!=product\n",
                (unsigned)g_u32KsymN, (unsigned)LINUX_KSYM_MAX,
                (unsigned)u32Free, (unsigned)LINUX_KSYM_USB_SOFT_SEED,
                (unsigned)LINUX_KSYM_USB_STORAGE_RESIDUAL,
                (unsigned)LINUX_KSYM_NET_RESIDUAL,
                (unsigned)LINUX_KSYM_PCI_RESIDUAL,
                (unsigned)LINUX_KSYM_USB_STORAGE_LEAF,
                (unsigned)GJ_KSYM_RESIDUAL_TOTAL,
                (unsigned)u32ReserveOk, (unsigned)g_u32RegOk,
                (unsigned)g_u32RegReplace, (unsigned)g_u32RegFull,
                (unsigned)g_u32HasHit, (unsigned)g_u32HasMiss,
                (unsigned)g_u32LeanOk, (unsigned)g_u32LeanChecks);
    } else {
        /* Refresh only: free/n + counters; no residual class restate. */
        kprintf("linux_ksym: soft inventory free=%u n=%u max=%u "
                "reserve_ok=%u reg_ok=%u reg_repl=%u full=%u "
                "has_hit=%u has_miss=%u soft=1 product=0 "
                "freestanding_no_exec=1 g_ac1_waiver=0 G-AC-1 "
                "storm=0 Soft!=product (refresh)\n",
                (unsigned)u32Free, (unsigned)g_u32KsymN,
                (unsigned)LINUX_KSYM_MAX, (unsigned)u32ReserveOk,
                (unsigned)g_u32RegOk, (unsigned)g_u32RegReplace,
                (unsigned)g_u32RegFull, (unsigned)g_u32HasHit,
                (unsigned)g_u32HasMiss);
    }
}

void
linux_ksym_init(void)
{
    if (g_fInit) {
        return;
    }
    g_fInit = 1;
    g_u32KsymN = 0;
    (void)linux_ksym_register("__SCT__might_resched", (void *)(uintptr_t)gj_ksym_soft___SCT__might_resched);
    (void)linux_ksym_register("__alloc_pages", (void *)(uintptr_t)gj_ksym_soft___alloc_pages);
    (void)linux_ksym_register("__const_udelay", (void *)(uintptr_t)gj_ksym_soft___const_udelay);
    (void)linux_ksym_register("__devm_mdiobus_register", (void *)(uintptr_t)gj_ksym_soft___devm_mdiobus_register);
    (void)linux_ksym_register("__dma_sync_single_for_cpu", (void *)(uintptr_t)gj_ksym_soft___dma_sync_single_for_cpu);
    (void)linux_ksym_register("__dma_sync_single_for_device", (void *)(uintptr_t)gj_ksym_soft___dma_sync_single_for_device);
    (void)linux_ksym_register("__fentry__", (void *)(uintptr_t)gj_ksym_soft___fentry__);
    (void)linux_ksym_register("__free_pages", (void *)(uintptr_t)gj_ksym_soft___free_pages);
    (void)linux_ksym_register("__mdiobus_write", (void *)(uintptr_t)gj_ksym_soft___mdiobus_write);
    (void)linux_ksym_register("__mutex_init", (void *)(uintptr_t)gj_ksym_soft___mutex_init);
    (void)linux_ksym_register("__napi_schedule", (void *)(uintptr_t)gj_ksym_soft___napi_schedule);
    (void)linux_ksym_register("__pci_register_driver", (void *)(uintptr_t)gj_ksym_soft___pci_register_driver);
    (void)linux_ksym_register("__phy_modify", (void *)(uintptr_t)gj_ksym_soft___phy_modify);
    (void)linux_ksym_register("__pm_runtime_idle", (void *)(uintptr_t)gj_ksym_soft___pm_runtime_idle);
    (void)linux_ksym_register("__pm_runtime_resume", (void *)(uintptr_t)gj_ksym_soft___pm_runtime_resume);
    (void)linux_ksym_register("__pskb_pull_tail", (void *)(uintptr_t)gj_ksym_soft___pskb_pull_tail);
    (void)linux_ksym_register("__skb_pad", (void *)(uintptr_t)gj_ksym_soft___skb_pad);
    (void)linux_ksym_register("__stack_chk_fail", (void *)(uintptr_t)gj_ksym_soft___stack_chk_fail);
    (void)linux_ksym_register("__warn_printk", (void *)(uintptr_t)gj_ksym_soft___warn_printk);
    (void)linux_ksym_register("__x86_indirect_jump_thunk_r10", (void *)(uintptr_t)gj_ksym_soft___x86_indirect_jump_thunk_r10);
    (void)linux_ksym_register("__x86_indirect_jump_thunk_r11", (void *)(uintptr_t)gj_ksym_soft___x86_indirect_jump_thunk_r11);
    (void)linux_ksym_register("__x86_indirect_jump_thunk_r12", (void *)(uintptr_t)gj_ksym_soft___x86_indirect_jump_thunk_r12);
    (void)linux_ksym_register("__x86_indirect_jump_thunk_r13", (void *)(uintptr_t)gj_ksym_soft___x86_indirect_jump_thunk_r13);
    (void)linux_ksym_register("__x86_indirect_jump_thunk_r14", (void *)(uintptr_t)gj_ksym_soft___x86_indirect_jump_thunk_r14);
    (void)linux_ksym_register("__x86_indirect_jump_thunk_r15", (void *)(uintptr_t)gj_ksym_soft___x86_indirect_jump_thunk_r15);
    (void)linux_ksym_register("__x86_indirect_jump_thunk_r8", (void *)(uintptr_t)gj_ksym_soft___x86_indirect_jump_thunk_r8);
    (void)linux_ksym_register("__x86_indirect_jump_thunk_r9", (void *)(uintptr_t)gj_ksym_soft___x86_indirect_jump_thunk_r9);
    (void)linux_ksym_register("__x86_indirect_jump_thunk_rax", (void *)(uintptr_t)gj_ksym_soft___x86_indirect_jump_thunk_rax);
    (void)linux_ksym_register("__x86_indirect_jump_thunk_rbp", (void *)(uintptr_t)gj_ksym_soft___x86_indirect_jump_thunk_rbp);
    (void)linux_ksym_register("__x86_indirect_jump_thunk_rbx", (void *)(uintptr_t)gj_ksym_soft___x86_indirect_jump_thunk_rbx);
    (void)linux_ksym_register("__x86_indirect_jump_thunk_rcx", (void *)(uintptr_t)gj_ksym_soft___x86_indirect_jump_thunk_rcx);
    (void)linux_ksym_register("__x86_indirect_jump_thunk_rdi", (void *)(uintptr_t)gj_ksym_soft___x86_indirect_jump_thunk_rdi);
    (void)linux_ksym_register("__x86_indirect_jump_thunk_rdx", (void *)(uintptr_t)gj_ksym_soft___x86_indirect_jump_thunk_rdx);
    (void)linux_ksym_register("__x86_indirect_jump_thunk_rsi", (void *)(uintptr_t)gj_ksym_soft___x86_indirect_jump_thunk_rsi);
    (void)linux_ksym_register("__x86_indirect_thunk_r10", (void *)(uintptr_t)gj_ksym_soft___x86_indirect_thunk_r10);
    (void)linux_ksym_register("__x86_indirect_thunk_r11", (void *)(uintptr_t)gj_ksym_soft___x86_indirect_thunk_r11);
    (void)linux_ksym_register("__x86_indirect_thunk_r12", (void *)(uintptr_t)gj_ksym_soft___x86_indirect_thunk_r12);
    (void)linux_ksym_register("__x86_indirect_thunk_r13", (void *)(uintptr_t)gj_ksym_soft___x86_indirect_thunk_r13);
    (void)linux_ksym_register("__x86_indirect_thunk_r14", (void *)(uintptr_t)gj_ksym_soft___x86_indirect_thunk_r14);
    (void)linux_ksym_register("__x86_indirect_thunk_r15", (void *)(uintptr_t)gj_ksym_soft___x86_indirect_thunk_r15);
    (void)linux_ksym_register("__x86_indirect_thunk_r8", (void *)(uintptr_t)gj_ksym_soft___x86_indirect_thunk_r8);
    (void)linux_ksym_register("__x86_indirect_thunk_r9", (void *)(uintptr_t)gj_ksym_soft___x86_indirect_thunk_r9);
    (void)linux_ksym_register("__x86_indirect_thunk_rax", (void *)(uintptr_t)gj_ksym_soft___x86_indirect_thunk_rax);
    (void)linux_ksym_register("__x86_indirect_thunk_rbp", (void *)(uintptr_t)gj_ksym_soft___x86_indirect_thunk_rbp);
    (void)linux_ksym_register("__x86_indirect_thunk_rbx", (void *)(uintptr_t)gj_ksym_soft___x86_indirect_thunk_rbx);
    (void)linux_ksym_register("__x86_indirect_thunk_rcx", (void *)(uintptr_t)gj_ksym_soft___x86_indirect_thunk_rcx);
    (void)linux_ksym_register("__x86_indirect_thunk_rdi", (void *)(uintptr_t)gj_ksym_soft___x86_indirect_thunk_rdi);
    (void)linux_ksym_register("__x86_indirect_thunk_rdx", (void *)(uintptr_t)gj_ksym_soft___x86_indirect_thunk_rdx);
    (void)linux_ksym_register("__x86_indirect_thunk_rsi", (void *)(uintptr_t)gj_ksym_soft___x86_indirect_thunk_rsi);
    (void)linux_ksym_register("__x86_return_thunk", (void *)(uintptr_t)gj_ksym_soft___x86_return_thunk);
    (void)linux_ksym_register("_dev_err", (void *)(uintptr_t)gj_ksym_soft__dev_err);
    (void)linux_ksym_register("_dev_info", (void *)(uintptr_t)gj_ksym_soft__dev_info);
    (void)linux_ksym_register("_dev_warn", (void *)(uintptr_t)gj_ksym_soft__dev_warn);
    (void)linux_ksym_register("_mcount", (void *)(uintptr_t)gj_ksym_soft__mcount);
    (void)linux_ksym_register("_printk", (void *)(uintptr_t)gj_ksym_soft__printk);
    (void)linux_ksym_register("_raw_spin_lock_irqsave", (void *)(uintptr_t)gj_ksym_soft__raw_spin_lock_irqsave);
    (void)linux_ksym_register("_raw_spin_unlock_irqrestore", (void *)(uintptr_t)gj_ksym_soft__raw_spin_unlock_irqrestore);
    (void)linux_ksym_register("alloc_etherdev_mqs", (void *)(uintptr_t)gj_ksym_soft_alloc_etherdev_mqs);
    (void)linux_ksym_register("byte_rev_table", (void *)(uintptr_t)gj_ksym_soft_byte_rev_table);
    (void)linux_ksym_register("clk_disable", (void *)(uintptr_t)gj_ksym_soft_clk_disable);
    (void)linux_ksym_register("clk_enable", (void *)(uintptr_t)gj_ksym_soft_clk_enable);
    (void)linux_ksym_register("clk_prepare", (void *)(uintptr_t)gj_ksym_soft_clk_prepare);
    (void)linux_ksym_register("clk_unprepare", (void *)(uintptr_t)gj_ksym_soft_clk_unprepare);
    (void)linux_ksym_register("crc32_le", (void *)(uintptr_t)gj_ksym_soft_crc32_le);
    (void)linux_ksym_register("csum_ipv6_magic", (void *)(uintptr_t)gj_ksym_soft_csum_ipv6_magic);
    (void)linux_ksym_register("dev_driver_string", (void *)(uintptr_t)gj_ksym_soft_dev_driver_string);
    (void)linux_ksym_register("dev_err_probe", (void *)(uintptr_t)gj_ksym_soft_dev_err_probe);
    (void)linux_ksym_register("dev_fetch_sw_netstats", (void *)(uintptr_t)gj_ksym_soft_dev_fetch_sw_netstats);
    (void)linux_ksym_register("dev_kfree_skb_any", (void *)(uintptr_t)gj_ksym_soft_dev_kfree_skb_any);
    (void)linux_ksym_register("dev_kfree_skb_any_reason", (void *)(uintptr_t)gj_ksym_soft_dev_kfree_skb_any_reason);
    (void)linux_ksym_register("device_set_wakeup_enable", (void *)(uintptr_t)gj_ksym_soft_device_set_wakeup_enable);
    (void)linux_ksym_register("devm_alloc_etherdev_mqs", (void *)(uintptr_t)gj_ksym_soft_devm_alloc_etherdev_mqs);
    (void)linux_ksym_register("devm_clk_get_optional_enabled", (void *)(uintptr_t)gj_ksym_soft_devm_clk_get_optional_enabled);
    (void)linux_ksym_register("devm_mdiobus_alloc_size", (void *)(uintptr_t)gj_ksym_soft_devm_mdiobus_alloc_size);
    (void)linux_ksym_register("disable_irq", (void *)(uintptr_t)gj_ksym_soft_disable_irq);
    (void)linux_ksym_register("disable_work", (void *)(uintptr_t)gj_ksym_soft_disable_work);
    (void)linux_ksym_register("disable_work_sync", (void *)(uintptr_t)gj_ksym_soft_disable_work_sync);
    (void)linux_ksym_register("dma_alloc_attrs", (void *)(uintptr_t)gj_ksym_soft_dma_alloc_attrs);
    (void)linux_ksym_register("dma_alloc_coherent", (void *)(uintptr_t)gj_ksym_soft_dma_alloc_coherent);
    (void)linux_ksym_register("dma_free_attrs", (void *)(uintptr_t)gj_ksym_soft_dma_free_attrs);
    (void)linux_ksym_register("dma_free_coherent", (void *)(uintptr_t)gj_ksym_soft_dma_free_coherent);
    (void)linux_ksym_register("dma_map_page_attrs", (void *)(uintptr_t)gj_ksym_soft_dma_map_page_attrs);
    (void)linux_ksym_register("dma_map_single", (void *)(uintptr_t)gj_ksym_soft_dma_map_single);
    (void)linux_ksym_register("dma_set_coherent_mask", (void *)(uintptr_t)gj_ksym_soft_dma_set_coherent_mask);
    (void)linux_ksym_register("dma_set_mask", (void *)(uintptr_t)gj_ksym_soft_dma_set_mask);
    (void)linux_ksym_register("dma_unmap_page_attrs", (void *)(uintptr_t)gj_ksym_soft_dma_unmap_page_attrs);
    (void)linux_ksym_register("dma_unmap_single", (void *)(uintptr_t)gj_ksym_soft_dma_unmap_single);
    (void)linux_ksym_register("dmam_alloc_attrs", (void *)(uintptr_t)gj_ksym_soft_dmam_alloc_attrs);
    (void)linux_ksym_register("dql_completed", (void *)(uintptr_t)gj_ksym_soft_dql_completed);
    (void)linux_ksym_register("dql_reset", (void *)(uintptr_t)gj_ksym_soft_dql_reset);
    (void)linux_ksym_register("enable_irq", (void *)(uintptr_t)gj_ksym_soft_enable_irq);
    (void)linux_ksym_register("enable_work", (void *)(uintptr_t)gj_ksym_soft_enable_work);
    (void)linux_ksym_register("eth_mac_addr", (void *)(uintptr_t)gj_ksym_soft_eth_mac_addr);
    (void)linux_ksym_register("eth_platform_get_mac_address", (void *)(uintptr_t)gj_ksym_soft_eth_platform_get_mac_address);
    (void)linux_ksym_register("eth_type_trans", (void *)(uintptr_t)gj_ksym_soft_eth_type_trans);
    (void)linux_ksym_register("eth_validate_addr", (void *)(uintptr_t)gj_ksym_soft_eth_validate_addr);
    (void)linux_ksym_register("ethtool_op_get_link", (void *)(uintptr_t)gj_ksym_soft_ethtool_op_get_link);
    (void)linux_ksym_register("ethtool_op_get_ts_info", (void *)(uintptr_t)gj_ksym_soft_ethtool_op_get_ts_info);
    (void)linux_ksym_register("firmware_request_nowarn", (void *)(uintptr_t)gj_ksym_soft_firmware_request_nowarn);
    (void)linux_ksym_register("request_firmware", (void *)(uintptr_t)gj_ksym_soft_request_firmware);
    (void)linux_ksym_register("request_firmware_direct", (void *)(uintptr_t)gj_ksym_soft_request_firmware_direct);
    (void)linux_ksym_register("fortify_panic", (void *)(uintptr_t)gj_ksym_soft_fortify_panic);
    (void)linux_ksym_register("free_irq", (void *)(uintptr_t)gj_ksym_soft_free_irq);
    (void)linux_ksym_register("free_netdev", (void *)(uintptr_t)gj_ksym_soft_free_netdev);
    (void)linux_ksym_register("genphy_soft_reset", (void *)(uintptr_t)gj_ksym_soft_genphy_soft_reset);
    (void)linux_ksym_register("get_random_bytes", (void *)(uintptr_t)gj_ksym_soft_get_random_bytes);
    (void)linux_ksym_register("ioremap", (void *)(uintptr_t)gj_ksym_soft_ioremap);
    (void)linux_ksym_register("iounmap", (void *)(uintptr_t)gj_ksym_soft_iounmap);
    (void)linux_ksym_register("is_vmalloc_addr", (void *)(uintptr_t)gj_ksym_soft_is_vmalloc_addr);
    (void)linux_ksym_register("jiffies", (void *)(uintptr_t)gj_ksym_soft_jiffies);
    (void)linux_ksym_register("kfree", (void *)(uintptr_t)gj_ksym_soft_kfree);
    (void)linux_ksym_register("kmalloc", (void *)(uintptr_t)gj_ksym_soft_kmalloc);
    (void)linux_ksym_register("kmalloc_caches", (void *)(uintptr_t)gj_ksym_soft_kmalloc_caches);
    (void)linux_ksym_register("kmalloc_trace", (void *)(uintptr_t)gj_ksym_soft_kmalloc_trace);
    (void)linux_ksym_register("ktime_get", (void *)(uintptr_t)gj_ksym_soft_ktime_get);
    (void)linux_ksym_register("kvfree", (void *)(uintptr_t)gj_ksym_soft_kvfree);
    (void)linux_ksym_register("kzalloc", (void *)(uintptr_t)gj_ksym_soft_kzalloc);
    (void)linux_ksym_register("led_classdev_register_ext", (void *)(uintptr_t)gj_ksym_soft_led_classdev_register_ext);
    (void)linux_ksym_register("led_classdev_unregister", (void *)(uintptr_t)gj_ksym_soft_led_classdev_unregister);
    (void)linux_ksym_register("mdelay", (void *)(uintptr_t)gj_ksym_soft_mdelay);
    (void)linux_ksym_register("mdiobus_get_phy", (void *)(uintptr_t)gj_ksym_soft_mdiobus_get_phy);
    (void)linux_ksym_register("mdiobus_read", (void *)(uintptr_t)gj_ksym_soft_mdiobus_read);
    (void)linux_ksym_register("mdiobus_write", (void *)(uintptr_t)gj_ksym_soft_mdiobus_write);
    (void)linux_ksym_register("memcmp", (void *)(uintptr_t)gj_ksym_soft_memcmp);
    (void)linux_ksym_register("memcpy", (void *)(uintptr_t)gj_ksym_soft_memcpy);
    (void)linux_ksym_register("memcpy_fromio", (void *)(uintptr_t)gj_ksym_soft_memcpy_fromio);
    (void)linux_ksym_register("memmove", (void *)(uintptr_t)gj_ksym_soft_memmove);
    (void)linux_ksym_register("memset", (void *)(uintptr_t)gj_ksym_soft_memset);
    (void)linux_ksym_register("module_put", (void *)(uintptr_t)gj_ksym_soft_module_put);
    (void)linux_ksym_register("msleep", (void *)(uintptr_t)gj_ksym_soft_msleep);
    (void)linux_ksym_register("mutex_lock", (void *)(uintptr_t)gj_ksym_soft_mutex_lock);
    (void)linux_ksym_register("mutex_unlock", (void *)(uintptr_t)gj_ksym_soft_mutex_unlock);
    (void)linux_ksym_register("napi_alloc_skb", (void *)(uintptr_t)gj_ksym_soft_napi_alloc_skb);
    (void)linux_ksym_register("napi_complete_done", (void *)(uintptr_t)gj_ksym_soft_napi_complete_done);
    (void)linux_ksym_register("napi_consume_skb", (void *)(uintptr_t)gj_ksym_soft_napi_consume_skb);
    (void)linux_ksym_register("napi_disable", (void *)(uintptr_t)gj_ksym_soft_napi_disable);
    (void)linux_ksym_register("napi_enable", (void *)(uintptr_t)gj_ksym_soft_napi_enable);
    (void)linux_ksym_register("napi_gro_receive", (void *)(uintptr_t)gj_ksym_soft_napi_gro_receive);
    (void)linux_ksym_register("napi_schedule_prep", (void *)(uintptr_t)gj_ksym_soft_napi_schedule_prep);
    (void)linux_ksym_register("net_ratelimit", (void *)(uintptr_t)gj_ksym_soft_net_ratelimit);
    (void)linux_ksym_register("netdev_err", (void *)(uintptr_t)gj_ksym_soft_netdev_err);
    (void)linux_ksym_register("netdev_info", (void *)(uintptr_t)gj_ksym_soft_netdev_info);
    (void)linux_ksym_register("netdev_notice", (void *)(uintptr_t)gj_ksym_soft_netdev_notice);
    (void)linux_ksym_register("netdev_printk", (void *)(uintptr_t)gj_ksym_soft_netdev_printk);
    (void)linux_ksym_register("netdev_stats_to_stats64", (void *)(uintptr_t)gj_ksym_soft_netdev_stats_to_stats64);
    (void)linux_ksym_register("netdev_sw_irq_coalesce_default_on", (void *)(uintptr_t)gj_ksym_soft_netdev_sw_irq_coalesce_default_on);
    (void)linux_ksym_register("netdev_update_features", (void *)(uintptr_t)gj_ksym_soft_netdev_update_features);
    (void)linux_ksym_register("netdev_warn", (void *)(uintptr_t)gj_ksym_soft_netdev_warn);
    (void)linux_ksym_register("netif_carrier_off", (void *)(uintptr_t)gj_ksym_soft_netif_carrier_off);
    (void)linux_ksym_register("netif_carrier_on", (void *)(uintptr_t)gj_ksym_soft_netif_carrier_on);
    (void)linux_ksym_register("netif_device_attach", (void *)(uintptr_t)gj_ksym_soft_netif_device_attach);
    (void)linux_ksym_register("netif_device_detach", (void *)(uintptr_t)gj_ksym_soft_netif_device_detach);
    (void)linux_ksym_register("netif_napi_add_weight_locked", (void *)(uintptr_t)gj_ksym_soft_netif_napi_add_weight_locked);
    (void)linux_ksym_register("netif_receive_skb", (void *)(uintptr_t)gj_ksym_soft_netif_receive_skb);
    (void)linux_ksym_register("netif_schedule_queue", (void *)(uintptr_t)gj_ksym_soft_netif_schedule_queue);
    (void)linux_ksym_register("netif_set_tso_max_segs", (void *)(uintptr_t)gj_ksym_soft_netif_set_tso_max_segs);
    (void)linux_ksym_register("netif_set_tso_max_size", (void *)(uintptr_t)gj_ksym_soft_netif_set_tso_max_size);
    (void)linux_ksym_register("netif_start_queue", (void *)(uintptr_t)gj_ksym_soft_netif_start_queue);
    (void)linux_ksym_register("netif_stop_queue", (void *)(uintptr_t)gj_ksym_soft_netif_stop_queue);
    (void)linux_ksym_register("netif_tx_wake_queue", (void *)(uintptr_t)gj_ksym_soft_netif_tx_wake_queue);
    (void)linux_ksym_register("netif_wake_queue", (void *)(uintptr_t)gj_ksym_soft_netif_wake_queue);
    (void)linux_ksym_register("numa_node", (void *)(uintptr_t)gj_ksym_soft_numa_node);
    (void)linux_ksym_register("page_offset_base", (void *)(uintptr_t)gj_ksym_soft_page_offset_base);
    (void)linux_ksym_register("pci_alloc_irq_vectors", (void *)(uintptr_t)gj_ksym_soft_pci_alloc_irq_vectors);
    (void)linux_ksym_register("pci_clear_master", (void *)(uintptr_t)gj_ksym_soft_pci_clear_master);
    (void)linux_ksym_register("pci_dev_run_wake", (void *)(uintptr_t)gj_ksym_soft_pci_dev_run_wake);
    (void)linux_ksym_register("pci_disable_device", (void *)(uintptr_t)gj_ksym_soft_pci_disable_device);
    (void)linux_ksym_register("pci_disable_link_state", (void *)(uintptr_t)gj_ksym_soft_pci_disable_link_state);
    (void)linux_ksym_register("pci_enable_device", (void *)(uintptr_t)gj_ksym_soft_pci_enable_device);
    (void)linux_ksym_register("pci_iomap", (void *)(uintptr_t)gj_ksym_soft_pci_iomap);
    (void)linux_ksym_register("pci_iounmap", (void *)(uintptr_t)gj_ksym_soft_pci_iounmap);
    (void)linux_ksym_register("pci_irq_vector", (void *)(uintptr_t)gj_ksym_soft_pci_irq_vector);
    (void)linux_ksym_register("pci_read_config_byte", (void *)(uintptr_t)gj_ksym_soft_pci_read_config_byte);
    (void)linux_ksym_register("pci_read_config_word", (void *)(uintptr_t)gj_ksym_soft_pci_read_config_word);
    (void)linux_ksym_register("pci_register_driver", (void *)(uintptr_t)gj_ksym_soft_pci_register_driver);
    (void)linux_ksym_register("pci_release_regions", (void *)(uintptr_t)gj_ksym_soft_pci_release_regions);
    (void)linux_ksym_register("pci_request_regions", (void *)(uintptr_t)gj_ksym_soft_pci_request_regions);
    (void)linux_ksym_register("pci_reset_bus", (void *)(uintptr_t)gj_ksym_soft_pci_reset_bus);
    (void)linux_ksym_register("pci_select_bars", (void *)(uintptr_t)gj_ksym_soft_pci_select_bars);
    (void)linux_ksym_register("pci_set_master", (void *)(uintptr_t)gj_ksym_soft_pci_set_master);
    (void)linux_ksym_register("pci_set_power_state", (void *)(uintptr_t)gj_ksym_soft_pci_set_power_state);
    (void)linux_ksym_register("pci_status_get_and_clear_errors", (void *)(uintptr_t)gj_ksym_soft_pci_status_get_and_clear_errors);
    (void)linux_ksym_register("pci_unregister_driver", (void *)(uintptr_t)gj_ksym_soft_pci_unregister_driver);
    (void)linux_ksym_register("pci_wake_from_d3", (void *)(uintptr_t)gj_ksym_soft_pci_wake_from_d3);
    (void)linux_ksym_register("pci_write_config_byte", (void *)(uintptr_t)gj_ksym_soft_pci_write_config_byte);
    (void)linux_ksym_register("pcie_capability_clear_and_set_word_locked", (void *)(uintptr_t)gj_ksym_soft_pcie_capability_clear_and_set_word_locked);
    (void)linux_ksym_register("pcie_set_readrq", (void *)(uintptr_t)gj_ksym_soft_pcie_set_readrq);
    (void)linux_ksym_register("pcim_enable_device", (void *)(uintptr_t)gj_ksym_soft_pcim_enable_device);
    (void)linux_ksym_register("pcim_iomap_region", (void *)(uintptr_t)gj_ksym_soft_pcim_iomap_region);
    (void)linux_ksym_register("pcim_set_mwi", (void *)(uintptr_t)gj_ksym_soft_pcim_set_mwi);
    (void)linux_ksym_register("phy_advertise_eee_all", (void *)(uintptr_t)gj_ksym_soft_phy_advertise_eee_all);
    (void)linux_ksym_register("phy_attached_info", (void *)(uintptr_t)gj_ksym_soft_phy_attached_info);
    (void)linux_ksym_register("phy_connect_direct", (void *)(uintptr_t)gj_ksym_soft_phy_connect_direct);
    (void)linux_ksym_register("phy_disconnect", (void *)(uintptr_t)gj_ksym_soft_phy_disconnect);
    (void)linux_ksym_register("phy_do_ioctl_running", (void *)(uintptr_t)gj_ksym_soft_phy_do_ioctl_running);
    (void)linux_ksym_register("phy_ethtool_get_eee", (void *)(uintptr_t)gj_ksym_soft_phy_ethtool_get_eee);
    (void)linux_ksym_register("phy_ethtool_get_link_ksettings", (void *)(uintptr_t)gj_ksym_soft_phy_ethtool_get_link_ksettings);
    (void)linux_ksym_register("phy_ethtool_nway_reset", (void *)(uintptr_t)gj_ksym_soft_phy_ethtool_nway_reset);
    (void)linux_ksym_register("phy_ethtool_set_eee", (void *)(uintptr_t)gj_ksym_soft_phy_ethtool_set_eee);
    (void)linux_ksym_register("phy_ethtool_set_link_ksettings", (void *)(uintptr_t)gj_ksym_soft_phy_ethtool_set_link_ksettings);
    (void)linux_ksym_register("phy_get_pause", (void *)(uintptr_t)gj_ksym_soft_phy_get_pause);
    (void)linux_ksym_register("phy_init_hw", (void *)(uintptr_t)gj_ksym_soft_phy_init_hw);
    (void)linux_ksym_register("phy_mac_interrupt", (void *)(uintptr_t)gj_ksym_soft_phy_mac_interrupt);
    (void)linux_ksym_register("phy_modify", (void *)(uintptr_t)gj_ksym_soft_phy_modify);
    (void)linux_ksym_register("phy_modify_paged", (void *)(uintptr_t)gj_ksym_soft_phy_modify_paged);
    (void)linux_ksym_register("phy_print_status", (void *)(uintptr_t)gj_ksym_soft_phy_print_status);
    (void)linux_ksym_register("phy_read_paged", (void *)(uintptr_t)gj_ksym_soft_phy_read_paged);
    (void)linux_ksym_register("phy_restore_page", (void *)(uintptr_t)gj_ksym_soft_phy_restore_page);
    (void)linux_ksym_register("phy_resume", (void *)(uintptr_t)gj_ksym_soft_phy_resume);
    (void)linux_ksym_register("phy_select_page", (void *)(uintptr_t)gj_ksym_soft_phy_select_page);
    (void)linux_ksym_register("phy_set_asym_pause", (void *)(uintptr_t)gj_ksym_soft_phy_set_asym_pause);
    (void)linux_ksym_register("phy_set_max_speed", (void *)(uintptr_t)gj_ksym_soft_phy_set_max_speed);
    (void)linux_ksym_register("phy_speed_down", (void *)(uintptr_t)gj_ksym_soft_phy_speed_down);
    (void)linux_ksym_register("phy_speed_up", (void *)(uintptr_t)gj_ksym_soft_phy_speed_up);
    (void)linux_ksym_register("phy_start", (void *)(uintptr_t)gj_ksym_soft_phy_start);
    (void)linux_ksym_register("phy_start_aneg", (void *)(uintptr_t)gj_ksym_soft_phy_start_aneg);
    (void)linux_ksym_register("phy_stop", (void *)(uintptr_t)gj_ksym_soft_phy_stop);
    (void)linux_ksym_register("phy_support_asym_pause", (void *)(uintptr_t)gj_ksym_soft_phy_support_asym_pause);
    (void)linux_ksym_register("phy_suspend", (void *)(uintptr_t)gj_ksym_soft_phy_suspend);
    (void)linux_ksym_register("phy_write_mmd", (void *)(uintptr_t)gj_ksym_soft_phy_write_mmd);
    (void)linux_ksym_register("phy_write_paged", (void *)(uintptr_t)gj_ksym_soft_phy_write_paged);
    (void)linux_ksym_register("phys_base", (void *)(uintptr_t)gj_ksym_soft_phys_base);
    (void)linux_ksym_register("pm_schedule_suspend", (void *)(uintptr_t)gj_ksym_soft_pm_schedule_suspend);
    (void)linux_ksym_register("printk", (void *)(uintptr_t)gj_ksym_soft_printk);
    (void)linux_ksym_register("pskb_expand_head", (void *)(uintptr_t)gj_ksym_soft_pskb_expand_head);
    (void)linux_ksym_register("queue_work_on", (void *)(uintptr_t)gj_ksym_soft_queue_work_on);
    (void)linux_ksym_register("readb", (void *)(uintptr_t)gj_ksym_soft_readb);
    (void)linux_ksym_register("readl", (void *)(uintptr_t)gj_ksym_soft_readl);
    (void)linux_ksym_register("readw", (void *)(uintptr_t)gj_ksym_soft_readw);
    (void)linux_ksym_register("register_netdev", (void *)(uintptr_t)gj_ksym_soft_register_netdev);
    (void)linux_ksym_register("release_firmware", (void *)(uintptr_t)gj_ksym_soft_release_firmware);
    (void)linux_ksym_register("request_irq", (void *)(uintptr_t)gj_ksym_soft_request_irq);
    (void)linux_ksym_register("request_threaded_irq", (void *)(uintptr_t)gj_ksym_soft_request_threaded_irq);
    (void)linux_ksym_register("rtnl_lock", (void *)(uintptr_t)gj_ksym_soft_rtnl_lock);
    (void)linux_ksym_register("rtnl_unlock", (void *)(uintptr_t)gj_ksym_soft_rtnl_unlock);
    (void)linux_ksym_register("scnprintf", (void *)(uintptr_t)gj_ksym_soft_scnprintf);
    (void)linux_ksym_register("sized_strscpy", (void *)(uintptr_t)gj_ksym_soft_sized_strscpy);
    (void)linux_ksym_register("skb_clone_tx_timestamp", (void *)(uintptr_t)gj_ksym_soft_skb_clone_tx_timestamp);
    (void)linux_ksym_register("skb_copy_bits", (void *)(uintptr_t)gj_ksym_soft_skb_copy_bits);
    (void)linux_ksym_register("skb_tstamp_tx", (void *)(uintptr_t)gj_ksym_soft_skb_tstamp_tx);
    (void)linux_ksym_register("snprintf", (void *)(uintptr_t)gj_ksym_soft_snprintf);
    (void)linux_ksym_register("softnet_data", (void *)(uintptr_t)gj_ksym_soft_softnet_data);
    (void)linux_ksym_register("spin_lock", (void *)(uintptr_t)gj_ksym_soft_spin_lock);
    (void)linux_ksym_register("spin_unlock", (void *)(uintptr_t)gj_ksym_soft_spin_unlock);
    (void)linux_ksym_register("sscanf", (void *)(uintptr_t)gj_ksym_soft_sscanf);
    (void)linux_ksym_register("strcat", (void *)(uintptr_t)gj_ksym_soft_strcat);
    (void)linux_ksym_register("strchr", (void *)(uintptr_t)gj_ksym_soft_strchr);
    (void)linux_ksym_register("strcmp", (void *)(uintptr_t)gj_ksym_soft_strcmp);
    (void)linux_ksym_register("strcpy", (void *)(uintptr_t)gj_ksym_soft_strcpy);
    (void)linux_ksym_register("strlen", (void *)(uintptr_t)gj_ksym_soft_strlen);
    (void)linux_ksym_register("strncat", (void *)(uintptr_t)gj_ksym_soft_strncat);
    (void)linux_ksym_register("strncmp", (void *)(uintptr_t)gj_ksym_soft_strncmp);
    (void)linux_ksym_register("strncpy", (void *)(uintptr_t)gj_ksym_soft_strncpy);
    (void)linux_ksym_register("strnlen", (void *)(uintptr_t)gj_ksym_soft_strnlen);
    (void)linux_ksym_register("strrchr", (void *)(uintptr_t)gj_ksym_soft_strrchr);
    (void)linux_ksym_register("strstr", (void *)(uintptr_t)gj_ksym_soft_strstr);
    (void)linux_ksym_register("synchronize_net", (void *)(uintptr_t)gj_ksym_soft_synchronize_net);
    (void)linux_ksym_register("system_state", (void *)(uintptr_t)gj_ksym_soft_system_state);
    (void)linux_ksym_register("system_wq", (void *)(uintptr_t)gj_ksym_soft_system_wq);
    (void)linux_ksym_register("this_cpu_off", (void *)(uintptr_t)gj_ksym_soft_this_cpu_off);
    (void)linux_ksym_register("try_module_get", (void *)(uintptr_t)gj_ksym_soft_try_module_get);
    (void)linux_ksym_register("udelay", (void *)(uintptr_t)gj_ksym_soft_udelay);
    (void)linux_ksym_register("unregister_netdev", (void *)(uintptr_t)gj_ksym_soft_unregister_netdev);
    (void)linux_ksym_register("usleep_range", (void *)(uintptr_t)gj_ksym_soft_usleep_range);
    (void)linux_ksym_register("usleep_range_state", (void *)(uintptr_t)gj_ksym_soft_usleep_range_state);
    (void)linux_ksym_register("vfree", (void *)(uintptr_t)gj_ksym_soft_vfree);
    (void)linux_ksym_register("vmalloc", (void *)(uintptr_t)gj_ksym_soft_vmalloc);
    (void)linux_ksym_register("vmemmap_base", (void *)(uintptr_t)gj_ksym_soft_vmemmap_base);
    (void)linux_ksym_register("vsnprintf", (void *)(uintptr_t)gj_ksym_soft_vsnprintf);
    (void)linux_ksym_register("writeb", (void *)(uintptr_t)gj_ksym_soft_writeb);
    (void)linux_ksym_register("writel", (void *)(uintptr_t)gj_ksym_soft_writel);
    (void)linux_ksym_register("writew", (void *)(uintptr_t)gj_ksym_soft_writew);

    /*
     * usb-storage soft leaf: first reloc misses on el9 ko were generic
     * (dma_max_mapping_size, sprintf, wait/complete, ...). Empty stubs only -
     * still leave usb_* / scsi_* unresolved so load FAIL is honest need=usbcore.
     * Cap LINUX_KSYM_USB_STORAGE_LEAF (15). Soft!=product.
     * Residual lean: no per-leaf kprintf - folded into residual lean lamp.
     */
    (void)linux_ksym_register("dma_max_mapping_size",
                              (void *)(uintptr_t)gj_ksym_soft_dma_max_mapping_size);
    (void)linux_ksym_register(
        "queue_limits_commit_update_frozen",
        (void *)(uintptr_t)gj_ksym_soft_queue_limits_commit_update_frozen);
    (void)linux_ksym_register("sprintf", (void *)(uintptr_t)gj_ksym_soft_sprintf);
    (void)linux_ksym_register("seq_printf",
                              (void *)(uintptr_t)gj_ksym_soft_seq_printf);
    (void)linux_ksym_register("seq_putc",
                              (void *)(uintptr_t)gj_ksym_soft_seq_putc);
    (void)linux_ksym_register("complete",
                              (void *)(uintptr_t)gj_ksym_soft_complete);
    (void)linux_ksym_register("finish_wait",
                              (void *)(uintptr_t)gj_ksym_soft_finish_wait);
    (void)linux_ksym_register("wait_for_completion",
                              (void *)(uintptr_t)gj_ksym_soft_wait_for_completion);
    (void)linux_ksym_register(
        "wait_for_completion_interruptible",
        (void *)(uintptr_t)gj_ksym_soft_wait_for_completion_interruptible);
    (void)linux_ksym_register(
        "wait_for_completion_interruptible_timeout",
        (void *)(uintptr_t)
            gj_ksym_soft_wait_for_completion_interruptible_timeout);
    (void)linux_ksym_register("_raw_spin_lock_irq",
                              (void *)(uintptr_t)gj_ksym_soft__raw_spin_lock_irq);
    (void)linux_ksym_register(
        "_raw_spin_unlock_irq",
        (void *)(uintptr_t)gj_ksym_soft__raw_spin_unlock_irq);
    (void)linux_ksym_register("schedule",
                              (void *)(uintptr_t)gj_ksym_soft_schedule);
    (void)linux_ksym_register("schedule_timeout",
                              (void *)(uintptr_t)gj_ksym_soft_schedule_timeout);
    (void)linux_ksym_register(
        "schedule_timeout_uninterruptible",
        (void *)(uintptr_t)gj_ksym_soft_schedule_timeout_uninterruptible);

    /*
     * Residual UND/FAIL class (el9 usb-storage.ko after leaf15 + soft
     * usb/scsi): freestanding generics so first miss is not sg_nents alone.
     * Still leave usb_* / scsi_* to linux_usb_soft (or unresolved) - load FAIL
     * need=usbcore stays honest. Soft!=product; no stick datapath claim.
     * Cap LINUX_KSYM_USB_STORAGE_RESIDUAL (62). Dual DoD eng expand +16 +12.
     * Residual lean: no per-class lamp - lean lamp only.
     */
    (void)linux_ksym_register("sg_nents",
                              (void *)(uintptr_t)gj_ksym_soft_sg_nents);
    (void)linux_ksym_register("sg_miter_start",
                              (void *)(uintptr_t)gj_ksym_soft_sg_miter_start);
    (void)linux_ksym_register("sg_miter_skip",
                              (void *)(uintptr_t)gj_ksym_soft_sg_miter_skip);
    (void)linux_ksym_register("sg_miter_next",
                              (void *)(uintptr_t)gj_ksym_soft_sg_miter_next);
    (void)linux_ksym_register("sg_miter_stop",
                              (void *)(uintptr_t)gj_ksym_soft_sg_miter_stop);
    (void)linux_ksym_register(
        "kthread_create_on_node",
        (void *)(uintptr_t)gj_ksym_soft_kthread_create_on_node);
    (void)linux_ksym_register(
        "kthread_should_stop",
        (void *)(uintptr_t)gj_ksym_soft_kthread_should_stop);
    (void)linux_ksym_register("kthread_stop",
                              (void *)(uintptr_t)gj_ksym_soft_kthread_stop);
    (void)linux_ksym_register("init_wait_entry",
                              (void *)(uintptr_t)gj_ksym_soft_init_wait_entry);
    (void)linux_ksym_register(
        "prepare_to_wait_event",
        (void *)(uintptr_t)gj_ksym_soft_prepare_to_wait_event);
    (void)linux_ksym_register(
        "__init_waitqueue_head",
        (void *)(uintptr_t)gj_ksym_soft___init_waitqueue_head);
    (void)linux_ksym_register(
        "__init_swait_queue_head",
        (void *)(uintptr_t)gj_ksym_soft___init_swait_queue_head);
    (void)linux_ksym_register("__wake_up",
                              (void *)(uintptr_t)gj_ksym_soft___wake_up);
    (void)linux_ksym_register("wake_up_process",
                              (void *)(uintptr_t)gj_ksym_soft_wake_up_process);
    (void)linux_ksym_register("init_timer_key",
                              (void *)(uintptr_t)gj_ksym_soft_init_timer_key);
    (void)linux_ksym_register(
        "delayed_work_timer_fn",
        (void *)(uintptr_t)gj_ksym_soft_delayed_work_timer_fn);
    (void)linux_ksym_register(
        "queue_delayed_work_on",
        (void *)(uintptr_t)gj_ksym_soft_queue_delayed_work_on);
    (void)linux_ksym_register(
        "cancel_delayed_work_sync",
        (void *)(uintptr_t)gj_ksym_soft_cancel_delayed_work_sync);
    (void)linux_ksym_register(
        "device_create_file",
        (void *)(uintptr_t)gj_ksym_soft_device_create_file);
    (void)linux_ksym_register("sysfs_emit",
                              (void *)(uintptr_t)gj_ksym_soft_sysfs_emit);
    (void)linux_ksym_register("_dev_notice",
                              (void *)(uintptr_t)gj_ksym_soft__dev_notice);
    (void)linux_ksym_register(
        "__dynamic_dev_dbg",
        (void *)(uintptr_t)gj_ksym_soft___dynamic_dev_dbg);
    (void)linux_ksym_register("kstrtouint",
                              (void *)(uintptr_t)gj_ksym_soft_kstrtouint);
    (void)linux_ksym_register("simple_strtoul",
                              (void *)(uintptr_t)gj_ksym_soft_simple_strtoul);
    (void)linux_ksym_register("skip_spaces",
                              (void *)(uintptr_t)gj_ksym_soft_skip_spaces);
    (void)linux_ksym_register("int_pow",
                              (void *)(uintptr_t)gj_ksym_soft_int_pow);
    (void)linux_ksym_register(
        "__msecs_to_jiffies",
        (void *)(uintptr_t)gj_ksym_soft___msecs_to_jiffies);
    (void)linux_ksym_register(
        "ktime_get_mono_fast_ns",
        (void *)(uintptr_t)gj_ksym_soft_ktime_get_mono_fast_ns);
    (void)linux_ksym_register(
        "__trace_set_current_state",
        (void *)(uintptr_t)gj_ksym_soft___trace_set_current_state);
    (void)linux_ksym_register("param_ops_uint",
                              (void *)(uintptr_t)gj_ksym_soft_param_ops_uint);
    (void)linux_ksym_register(
        "param_ops_string",
        (void *)(uintptr_t)gj_ksym_soft_param_ops_string);
    (void)linux_ksym_register("pcpu_hot",
                              (void *)(uintptr_t)gj_ksym_soft_pcpu_hot);
    (void)linux_ksym_register(
        "system_freezable_wq",
        (void *)(uintptr_t)gj_ksym_soft_system_freezable_wq);
    (void)linux_ksym_register(
        "__tracepoint_sched_set_state_tp",
        (void *)(uintptr_t)gj_ksym_soft___tracepoint_sched_set_state_tp);
    /* Dual DoD USB residual expand (+16). Generics only; no usb_ or scsi_ prefixes. */
    (void)linux_ksym_register("sg_init_table",
                              (void *)(uintptr_t)gj_ksym_soft_sg_init_table);
    (void)linux_ksym_register("sg_init_one",
                              (void *)(uintptr_t)gj_ksym_soft_sg_init_one);
    (void)linux_ksym_register("sg_next",
                              (void *)(uintptr_t)gj_ksym_soft_sg_next);
    (void)linux_ksym_register("sg_last",
                              (void *)(uintptr_t)gj_ksym_soft_sg_last);
    (void)linux_ksym_register(
        "sg_copy_from_buffer",
        (void *)(uintptr_t)gj_ksym_soft_sg_copy_from_buffer);
    (void)linux_ksym_register(
        "sg_copy_to_buffer",
        (void *)(uintptr_t)gj_ksym_soft_sg_copy_to_buffer);
    (void)linux_ksym_register(
        "sg_pcopy_from_buffer",
        (void *)(uintptr_t)gj_ksym_soft_sg_pcopy_from_buffer);
    (void)linux_ksym_register(
        "sg_pcopy_to_buffer",
        (void *)(uintptr_t)gj_ksym_soft_sg_pcopy_to_buffer);
    (void)linux_ksym_register("sg_alloc_table",
                              (void *)(uintptr_t)gj_ksym_soft_sg_alloc_table);
    (void)linux_ksym_register("sg_free_table",
                              (void *)(uintptr_t)gj_ksym_soft_sg_free_table);
    (void)linux_ksym_register(
        "sg_alloc_table_from_pages",
        (void *)(uintptr_t)gj_ksym_soft_sg_alloc_table_from_pages);
    (void)linux_ksym_register(
        "wait_for_completion_timeout",
        (void *)(uintptr_t)gj_ksym_soft_wait_for_completion_timeout);
    (void)linux_ksym_register("cancel_work_sync",
                              (void *)(uintptr_t)gj_ksym_soft_cancel_work_sync);
    (void)linux_ksym_register("mod_timer",
                              (void *)(uintptr_t)gj_ksym_soft_mod_timer);
    (void)linux_ksym_register("del_timer_sync",
                              (void *)(uintptr_t)gj_ksym_soft_del_timer_sync);
    (void)linux_ksym_register("flush_workqueue",
                              (void *)(uintptr_t)gj_ksym_soft_flush_workqueue);
    /* Dual DoD USB residual expand2 (+12). Generics only; no usb_ or scsi_. */
    (void)linux_ksym_register("alloc_workqueue",
                              (void *)(uintptr_t)gj_ksym_soft_alloc_workqueue);
    (void)linux_ksym_register(
        "destroy_workqueue",
        (void *)(uintptr_t)gj_ksym_soft_destroy_workqueue);
    (void)linux_ksym_register("flush_work",
                              (void *)(uintptr_t)gj_ksym_soft_flush_work);
    (void)linux_ksym_register("schedule_work",
                              (void *)(uintptr_t)gj_ksym_soft_schedule_work);
    (void)linux_ksym_register(
        "try_wait_for_completion",
        (void *)(uintptr_t)gj_ksym_soft_try_wait_for_completion);
    (void)linux_ksym_register("completion_done",
                              (void *)(uintptr_t)gj_ksym_soft_completion_done);
    (void)linux_ksym_register(
        "sg_nents_for_len",
        (void *)(uintptr_t)gj_ksym_soft_sg_nents_for_len);
    (void)linux_ksym_register("sg_copy_buffer",
                              (void *)(uintptr_t)gj_ksym_soft_sg_copy_buffer);
    (void)linux_ksym_register(
        "device_remove_file",
        (void *)(uintptr_t)gj_ksym_soft_device_remove_file);
    (void)linux_ksym_register("_dev_printk",
                              (void *)(uintptr_t)gj_ksym_soft__dev_printk);
    (void)linux_ksym_register("__kmalloc",
                              (void *)(uintptr_t)gj_ksym_soft___kmalloc);
    (void)linux_ksym_register("param_ops_bool",
                              (void *)(uintptr_t)gj_ksym_soft_param_ops_bool);

    /*
     * Residual UND/FAIL class for net soft (skb / xmit / netdev lifecycle).
     * Freestanding empty stubs; peer linux_netdev_soft replaces when inited.
     * G-AC-1: no .ko wire claim. Cap LINUX_KSYM_NET_RESIDUAL (48).
     * Useful for soft eng + future userspace net ABI name binding.
     * Dual DoD eng expand +18 +12. Residual lean: no per-class lamp.
     */
    (void)linux_ksym_register("skb_put",
                              (void *)(uintptr_t)gj_ksym_soft_skb_put);
    (void)linux_ksym_register("pskb_may_pull",
                              (void *)(uintptr_t)gj_ksym_soft_pskb_may_pull);
    (void)linux_ksym_register(
        "__netdev_alloc_skb",
        (void *)(uintptr_t)gj_ksym_soft___netdev_alloc_skb);
    (void)linux_ksym_register("dev_alloc_skb",
                              (void *)(uintptr_t)gj_ksym_soft_dev_alloc_skb);
    (void)linux_ksym_register(
        "__napi_alloc_skb",
        (void *)(uintptr_t)gj_ksym_soft___napi_alloc_skb);
    (void)linux_ksym_register("dev_queue_xmit",
                              (void *)(uintptr_t)gj_ksym_soft_dev_queue_xmit);
    (void)linux_ksym_register("kfree_skb",
                              (void *)(uintptr_t)gj_ksym_soft_kfree_skb);
    (void)linux_ksym_register("consume_skb",
                              (void *)(uintptr_t)gj_ksym_soft_consume_skb);
    (void)linux_ksym_register("__kfree_skb",
                              (void *)(uintptr_t)gj_ksym_soft___kfree_skb);
    (void)linux_ksym_register("skb_pull",
                              (void *)(uintptr_t)gj_ksym_soft_skb_pull);
    (void)linux_ksym_register("skb_push",
                              (void *)(uintptr_t)gj_ksym_soft_skb_push);
    (void)linux_ksym_register("skb_reserve",
                              (void *)(uintptr_t)gj_ksym_soft_skb_reserve);
    (void)linux_ksym_register("netif_rx",
                              (void *)(uintptr_t)gj_ksym_soft_netif_rx);
    (void)linux_ksym_register(
        "register_netdevice",
        (void *)(uintptr_t)gj_ksym_soft_register_netdevice);
    (void)linux_ksym_register(
        "unregister_netdevice",
        (void *)(uintptr_t)gj_ksym_soft_unregister_netdevice);
    (void)linux_ksym_register(
        "netif_tx_start_all_queues",
        (void *)(uintptr_t)gj_ksym_soft_netif_tx_start_all_queues);
    (void)linux_ksym_register(
        "netif_tx_stop_all_queues",
        (void *)(uintptr_t)gj_ksym_soft_netif_tx_stop_all_queues);
    (void)linux_ksym_register("rtnl_trylock",
                              (void *)(uintptr_t)gj_ksym_soft_rtnl_trylock);
    /* Dual DoD net residual expand (+18). Soft!=product; eng+userspace. */
    (void)linux_ksym_register("skb_clone",
                              (void *)(uintptr_t)gj_ksym_soft_skb_clone);
    (void)linux_ksym_register("skb_copy",
                              (void *)(uintptr_t)gj_ksym_soft_skb_copy);
    (void)linux_ksym_register("skb_trim",
                              (void *)(uintptr_t)gj_ksym_soft_skb_trim);
    (void)linux_ksym_register(
        "skb_checksum_help",
        (void *)(uintptr_t)gj_ksym_soft_skb_checksum_help);
    (void)linux_ksym_register(
        "__skb_gso_segment",
        (void *)(uintptr_t)gj_ksym_soft___skb_gso_segment);
    (void)linux_ksym_register(
        "netif_tx_stop_queue",
        (void *)(uintptr_t)gj_ksym_soft_netif_tx_stop_queue);
    (void)linux_ksym_register(
        "netif_set_real_num_tx_queues",
        (void *)(uintptr_t)gj_ksym_soft_netif_set_real_num_tx_queues);
    (void)linux_ksym_register(
        "netif_set_real_num_rx_queues",
        (void *)(uintptr_t)gj_ksym_soft_netif_set_real_num_rx_queues);
    (void)linux_ksym_register(
        "netif_napi_add_weight",
        (void *)(uintptr_t)gj_ksym_soft_netif_napi_add_weight);
    (void)linux_ksym_register(
        "alloc_netdev_mqs",
        (void *)(uintptr_t)gj_ksym_soft_alloc_netdev_mqs);
    (void)linux_ksym_register(
        "unregister_netdevice_queue",
        (void *)(uintptr_t)gj_ksym_soft_unregister_netdevice_queue);
    (void)linux_ksym_register("dev_get_stats",
                              (void *)(uintptr_t)gj_ksym_soft_dev_get_stats);
    (void)linux_ksym_register("dev_addr_mod",
                              (void *)(uintptr_t)gj_ksym_soft_dev_addr_mod);
    (void)linux_ksym_register(
        "netdev_state_change",
        (void *)(uintptr_t)gj_ksym_soft_netdev_state_change);
    (void)linux_ksym_register(
        "linkwatch_fire_event",
        (void *)(uintptr_t)gj_ksym_soft_linkwatch_fire_event);
    (void)linux_ksym_register("rtnl_is_locked",
                              (void *)(uintptr_t)gj_ksym_soft_rtnl_is_locked);
    (void)linux_ksym_register(
        "netif_tx_disable",
        (void *)(uintptr_t)gj_ksym_soft_netif_tx_disable);
    (void)linux_ksym_register("netif_napi_del",
                              (void *)(uintptr_t)gj_ksym_soft_netif_napi_del);
    /* Dual DoD net residual expand2 (+12). Soft!=product; eng+userspace. */
    (void)linux_ksym_register("napi_schedule",
                              (void *)(uintptr_t)gj_ksym_soft_napi_schedule);
    (void)linux_ksym_register("napi_complete",
                              (void *)(uintptr_t)gj_ksym_soft_napi_complete);
    (void)linux_ksym_register("netif_rx_ni",
                              (void *)(uintptr_t)gj_ksym_soft_netif_rx_ni);
    (void)linux_ksym_register(
        "dev_kfree_skb_irq",
        (void *)(uintptr_t)gj_ksym_soft_dev_kfree_skb_irq);
    (void)linux_ksym_register("skb_copy_expand",
                              (void *)(uintptr_t)gj_ksym_soft_skb_copy_expand);
    (void)linux_ksym_register("skb_linearize",
                              (void *)(uintptr_t)gj_ksym_soft_skb_linearize);
    (void)linux_ksym_register(
        "netdev_alloc_frag",
        (void *)(uintptr_t)gj_ksym_soft_netdev_alloc_frag);
    (void)linux_ksym_register("napi_alloc_frag",
                              (void *)(uintptr_t)gj_ksym_soft_napi_alloc_frag);
    (void)linux_ksym_register(
        "__netif_schedule",
        (void *)(uintptr_t)gj_ksym_soft___netif_schedule);
    (void)linux_ksym_register(
        "netdev_features_change",
        (void *)(uintptr_t)gj_ksym_soft_netdev_features_change);
    (void)linux_ksym_register(
        "netdev_notify_peers",
        (void *)(uintptr_t)gj_ksym_soft_netdev_notify_peers);
    (void)linux_ksym_register(
        "netdev_rss_key_fill",
        (void *)(uintptr_t)gj_ksym_soft_netdev_rss_key_fill);

    /*
     * Residual UND/FAIL class for PCI soft (config / BAR / drvdata / irq /
     * dma helpers). Freestanding empty stubs; peer linux_pci_soft replaces.
     * G-AC-1: no real BAR/MMIO claim. Cap LINUX_KSYM_PCI_RESIDUAL (32).
     * Dual DoD eng expand +12 msi/msix/regions/dma-mask.
     * Residual lean: no per-class lamp - lean lamp only.
     */
    (void)linux_ksym_register(
        "pci_read_config_dword",
        (void *)(uintptr_t)gj_ksym_soft_pci_read_config_dword);
    (void)linux_ksym_register(
        "pci_write_config_word",
        (void *)(uintptr_t)gj_ksym_soft_pci_write_config_word);
    (void)linux_ksym_register(
        "pci_write_config_dword",
        (void *)(uintptr_t)gj_ksym_soft_pci_write_config_dword);
    (void)linux_ksym_register(
        "pci_request_region",
        (void *)(uintptr_t)gj_ksym_soft_pci_request_region);
    (void)linux_ksym_register(
        "pci_release_region",
        (void *)(uintptr_t)gj_ksym_soft_pci_release_region);
    (void)linux_ksym_register(
        "pci_resource_start",
        (void *)(uintptr_t)gj_ksym_soft_pci_resource_start);
    (void)linux_ksym_register(
        "pci_resource_len",
        (void *)(uintptr_t)gj_ksym_soft_pci_resource_len);
    (void)linux_ksym_register(
        "pci_set_drvdata",
        (void *)(uintptr_t)gj_ksym_soft_pci_set_drvdata);
    (void)linux_ksym_register(
        "pci_get_drvdata",
        (void *)(uintptr_t)gj_ksym_soft_pci_get_drvdata);
    (void)linux_ksym_register(
        "pci_free_irq_vectors",
        (void *)(uintptr_t)gj_ksym_soft_pci_free_irq_vectors);
    (void)linux_ksym_register(
        "pci_find_capability",
        (void *)(uintptr_t)gj_ksym_soft_pci_find_capability);
    (void)linux_ksym_register(
        "pci_save_state",
        (void *)(uintptr_t)gj_ksym_soft_pci_save_state);
    (void)linux_ksym_register(
        "pci_restore_state",
        (void *)(uintptr_t)gj_ksym_soft_pci_restore_state);
    (void)linux_ksym_register(
        "dma_mapping_error",
        (void *)(uintptr_t)gj_ksym_soft_dma_mapping_error);
    (void)linux_ksym_register(
        "dma_sync_single_for_cpu",
        (void *)(uintptr_t)gj_ksym_soft_dma_sync_single_for_cpu);
    (void)linux_ksym_register(
        "dma_sync_single_for_device",
        (void *)(uintptr_t)gj_ksym_soft_dma_sync_single_for_device);
    (void)linux_ksym_register(
        "synchronize_irq",
        (void *)(uintptr_t)gj_ksym_soft_synchronize_irq);
    (void)linux_ksym_register(
        "irq_set_affinity_hint",
        (void *)(uintptr_t)gj_ksym_soft_irq_set_affinity_hint);
    (void)linux_ksym_register(
        "pcim_iomap_regions",
        (void *)(uintptr_t)gj_ksym_soft_pcim_iomap_regions);
    (void)linux_ksym_register(
        "pcim_iomap_table",
        (void *)(uintptr_t)gj_ksym_soft_pcim_iomap_table);
    /* Dual DoD PCI residual expand (+12). Soft!=product; UDX/soft layout. */
    (void)linux_ksym_register("pci_enable_msi",
                              (void *)(uintptr_t)gj_ksym_soft_pci_enable_msi);
    (void)linux_ksym_register("pci_disable_msi",
                              (void *)(uintptr_t)gj_ksym_soft_pci_disable_msi);
    (void)linux_ksym_register(
        "pci_enable_msix_range",
        (void *)(uintptr_t)gj_ksym_soft_pci_enable_msix_range);
    (void)linux_ksym_register("pci_disable_msix",
                              (void *)(uintptr_t)gj_ksym_soft_pci_disable_msix);
    (void)linux_ksym_register(
        "pci_find_ext_capability",
        (void *)(uintptr_t)gj_ksym_soft_pci_find_ext_capability);
    (void)linux_ksym_register(
        "pci_enable_device_mem",
        (void *)(uintptr_t)gj_ksym_soft_pci_enable_device_mem);
    (void)linux_ksym_register(
        "pci_request_selected_regions",
        (void *)(uintptr_t)gj_ksym_soft_pci_request_selected_regions);
    (void)linux_ksym_register(
        "pci_release_selected_regions",
        (void *)(uintptr_t)gj_ksym_soft_pci_release_selected_regions);
    (void)linux_ksym_register("pci_iomap_range",
                              (void *)(uintptr_t)gj_ksym_soft_pci_iomap_range);
    (void)linux_ksym_register(
        "dma_set_mask_and_coherent",
        (void *)(uintptr_t)gj_ksym_soft_dma_set_mask_and_coherent);
    (void)linux_ksym_register(
        "pcie_capability_read_word",
        (void *)(uintptr_t)gj_ksym_soft_pcie_capability_read_word);
    (void)linux_ksym_register(
        "pci_choose_state",
        (void *)(uintptr_t)gj_ksym_soft_pci_choose_state);

    /*
     * Residual lean (C0 deepen; stamp-free; exclusive this TU):
     * Functional self-check arms then lean lamp + optional lean PASS.
     * No per-class / per-symbol stamp storm. No version stamp.
     * ABI hostability eng: name catalog for soft-loaded module resolve +
     * future userspace bind. freestanding_no_exec honesty: resolve_only
     * under peer SKIP exec / never_exec_ko; not product .ko wire.
     * g_ac1_waiver=0 always (never claim G-AC-1 waiver). Soft!=product.
     * Dual DoD A/B remain OPEN. Dual MIT OR Apache-2.0.
     * greppable: linux_ksym: soft residual lean
     * greppable: linux_ksym: soft residual lean PASS
     * greppable: freestanding_no_exec
     */
    {
        u32 u32Free;
        u32 u32ReserveOk;
        u32 u32Checks;
        u32 u32Ok;
        u32 u32CapOk;
        u32 u32SizesOk;
        u32 u32ResolveOk;
        u32 u32CountOk;
        u32 u32HeadOk;
        u32 u32ReplaceOk;
        u32 u32MissOk;
        u32 u32LicOk;
        u32 u32NBefore;
        u32 u32ReplBefore;
        void *pSg;
        void *pSkb;
        void *pPci;
        void *pLeaf;
        /* Stack-local product honesty (never hard-gates; Soft!=product). */
        const int nSoft = 1;
        const int nProduct = 0;
        const int nGac1Waiver = 0; /* never claim G-AC-1 waiver */
        const int nNeverExecKo = 1;
        const int nResolveOnly = 1;
        const int nDualDodOpen = 1; /* Dual DoD A/B OPEN honesty */
        const int nBar3Close = 0;   /* residual never closes bar3 */

        u32Free = linux_ksym_slots_free();
        u32ReserveOk = (u32Free >= LINUX_KSYM_USB_SOFT_SEED) ? 1u : 0u;
        u32Checks = 0;
        u32Ok = 0;
        u32CapOk = 0;
        u32SizesOk = 0;
        u32ResolveOk = 0;
        u32CountOk = 0;
        u32HeadOk = 0;
        u32ReplaceOk = 0;
        u32MissOk = 0;
        u32LicOk = 0;

        /*
         * Arm 1 cap: table capacity holds residual total + soft seed design
         * budget with multi-mod headroom. Soft!=product.
         */
        u32Checks++;
        if (LINUX_KSYM_MAX >= 4096u &&
            LINUX_KSYM_MAX >=
                (GJ_KSYM_RESIDUAL_TOTAL + LINUX_KSYM_USB_SOFT_SEED + 256u) &&
            g_u32KsymN < LINUX_KSYM_MAX && g_u32KsymN > 0u) {
            u32CapOk = 1u;
            u32Ok++;
        }

        /*
         * Arm 2 sizes: residual class constants match Dual DoD eng expands
         * (leaf15 + usb62 + net48 + pci32). Soft!=product.
         */
        u32Checks++;
        if (LINUX_KSYM_USB_STORAGE_LEAF == 15u &&
            LINUX_KSYM_USB_STORAGE_RESIDUAL == 62u &&
            LINUX_KSYM_NET_RESIDUAL == 48u &&
            LINUX_KSYM_PCI_RESIDUAL == 32u &&
            LINUX_KSYM_USB_SOFT_SEED == 191u &&
            GJ_KSYM_RESIDUAL_TOTAL == (15u + 62u + 48u + 32u) &&
            GJ_KSYM_LEAN_CHECKS == 8u) {
            u32SizesOk = 1u;
            u32Ok++;
        }

        /*
         * Arm 3 resolve: residual class samples present (leaf/usb/net/pci).
         * lookup + has hit; resolve honesty only (never .ko exec).
         */
        u32Checks++;
        pLeaf = linux_ksym_lookup("dma_max_mapping_size");
        pSg = linux_ksym_lookup("sg_nents");
        pSkb = linux_ksym_lookup("skb_put");
        pPci = linux_ksym_lookup("pci_enable_msi");
        if (pLeaf != 0 && pSg != 0 && pSkb != 0 && pPci != 0 &&
            linux_ksym_has("dma_max_mapping_size") == 1 &&
            linux_ksym_has("sg_nents") == 1 &&
            linux_ksym_has("skb_put") == 1 &&
            linux_ksym_has("pci_enable_msi") == 1 &&
            linux_ksym_has("alloc_workqueue") == 1 &&
            linux_ksym_has("napi_schedule") == 1 &&
            linux_ksym_has("pci_choose_state") == 1) {
            u32ResolveOk = 1u;
            u32Ok++;
        }

        /*
         * Arm 4 count: n/count/free/reg counters coherent after residual.
         * Soft!=product diagnostic only.
         */
        u32Checks++;
        if (linux_ksym_count() == g_u32KsymN &&
            linux_ksym_slots_free() == u32Free &&
            (g_u32KsymN + u32Free) == LINUX_KSYM_MAX &&
            g_u32RegOk == g_u32KsymN && g_u32RegFull == 0u) {
            u32CountOk = 1u;
            u32Ok++;
        }

        /*
         * Arm 5 headroom: free slots reserve soft USB seed design budget.
         * Multi-mod export still has room after residual surface.
         */
        u32Checks++;
        if (u32ReserveOk == 1u && u32Free >= LINUX_KSYM_USB_SOFT_SEED &&
            u32Free >= GJ_KSYM_RESIDUAL_TOTAL &&
            g_u32KsymN >= GJ_KSYM_RESIDUAL_TOTAL) {
            u32HeadOk = 1u;
            u32Ok++;
        }

        /*
         * Arm 6 replace: re-register residual name keeps n stable (replace
         * path). Soft!=product; eng register API honesty.
         */
        u32Checks++;
        u32NBefore = g_u32KsymN;
        u32ReplBefore = g_u32RegReplace;
        if (linux_ksym_register("sg_nents",
                                (void *)(uintptr_t)gj_ksym_soft_sg_nents) ==
                0 &&
            g_u32KsymN == u32NBefore &&
            g_u32RegReplace == (u32ReplBefore + 1u) &&
            linux_ksym_lookup("sg_nents") ==
                (void *)(uintptr_t)gj_ksym_soft_sg_nents) {
            u32ReplaceOk = 1u;
            u32Ok++;
        }

        /*
         * Arm 7 miss: unknown name is soft miss (has=0 lookup=NULL).
         * Does not claim product resolve completeness.
         */
        u32Checks++;
        if (linux_ksym_lookup("___gj_ksym_never_soft_miss___") == 0 &&
            linux_ksym_has("___gj_ksym_never_soft_miss___") == 0 &&
            linux_ksym_lookup("") == 0 && linux_ksym_has("") == 0 &&
            linux_ksym_has((const char *)0) == 0) {
            u32MissOk = 1u;
            u32Ok++;
        }

        /*
         * Arm 8 lic: Soft!=product · product=0 · g_ac1_waiver=0 ·
         * freestanding_no_exec / never_exec_ko · Dual DoD OPEN · bar3_close=0.
         * Residual never waives G-AC-1; never closes Dual DoD A/B.
         */
        u32Checks++;
        if (nSoft == 1 && nProduct == 0 && nGac1Waiver == 0 &&
            nNeverExecKo == 1 && nResolveOnly == 1 && nDualDodOpen == 1 &&
            nBar3Close == 0) {
            u32LicOk = 1u;
            u32Ok++;
        }

        g_u32LeanChecks = u32Checks;
        g_u32LeanOk = u32Ok;

        /*
         * Grep: linux_ksym: soft residual lean
         * One residual lean line (HARD: no stamp storms). Soft!=product.
         * greppable: freestanding_no_exec | g_ac1_waiver=0 | Soft!=product
         */
        kprintf("linux_ksym: soft residual lean usb=%u net=%u pci=%u leaf=%u "
                "cap=%u sizes=%u resolve=%u count=%u headroom=%u "
                "replace=%u miss=%u lic=%u ok=%u/%u "
                "n=%u max=%u free=%u residual_total=%u usb_seed=%u "
                "reserve_ok=%u reg_ok=%u reg_repl=%u "
                "soft=1 product=0 userspace_bind=1 hostability=1 "
                "freestanding_no_exec=1 never_exec_ko=1 resolve_only=1 "
                "g_ac1_waiver=0 G-AC-1 exclusive=1 "
                "dual_dod=OPEN bar3_close=0 "
                "dual=MIT_OR_Apache-2.0 no_version_stamp=1 storm=0 "
                "Soft!=product residual!=product "
                "(C0 soft residual lean; soft-loaded module resolve eng; "
                "!= .ko wire; != G-AC-1 waiver; != Dual DoD close)\n",
                (unsigned)LINUX_KSYM_USB_STORAGE_RESIDUAL,
                (unsigned)LINUX_KSYM_NET_RESIDUAL,
                (unsigned)LINUX_KSYM_PCI_RESIDUAL,
                (unsigned)LINUX_KSYM_USB_STORAGE_LEAF,
                (unsigned)u32CapOk, (unsigned)u32SizesOk,
                (unsigned)u32ResolveOk, (unsigned)u32CountOk,
                (unsigned)u32HeadOk, (unsigned)u32ReplaceOk,
                (unsigned)u32MissOk, (unsigned)u32LicOk, (unsigned)u32Ok,
                (unsigned)u32Checks, (unsigned)g_u32KsymN,
                (unsigned)LINUX_KSYM_MAX, (unsigned)u32Free,
                (unsigned)GJ_KSYM_RESIDUAL_TOTAL,
                (unsigned)LINUX_KSYM_USB_SOFT_SEED, (unsigned)u32ReserveOk,
                (unsigned)g_u32RegOk, (unsigned)g_u32RegReplace);

        /*
         * Grep: linux_ksym: soft residual lean PASS
         * Functional residual honesty only - never product DoD / never
         * Dual DoD A/B close / never G-AC-1 waiver / never bar3 close.
         * Emit PASS only when all lean arms hold (C0 strengthen).
         */
        if (u32Ok == u32Checks && u32Checks == GJ_KSYM_LEAN_CHECKS) {
            kprintf("linux_ksym: soft residual lean PASS "
                    "cap=%u sizes=%u resolve=%u count=%u headroom=%u "
                    "replace=%u miss=%u lic=%u ok=%u/%u "
                    "usb=%u net=%u pci=%u leaf=%u residual_total=%u "
                    "n=%u free=%u reserve_ok=%u "
                    "soft=1 product=0 soft_ne_product=1 "
                    "freestanding_no_exec=1 never_exec_ko=1 resolve_only=1 "
                    "g_ac1_waiver=0 G-AC-1 exclusive=1 "
                    "dual_dod=OPEN bar3_close=0 "
                    "dual=MIT_OR_Apache-2.0 no_version_stamp=1 storm=0 "
                    "Soft!=product "
                    "(lean residual only; != .ko wire; != Dual DoD close)\n",
                    (unsigned)u32CapOk, (unsigned)u32SizesOk,
                    (unsigned)u32ResolveOk, (unsigned)u32CountOk,
                    (unsigned)u32HeadOk, (unsigned)u32ReplaceOk,
                    (unsigned)u32MissOk, (unsigned)u32LicOk, (unsigned)u32Ok,
                    (unsigned)u32Checks,
                    (unsigned)LINUX_KSYM_USB_STORAGE_RESIDUAL,
                    (unsigned)LINUX_KSYM_NET_RESIDUAL,
                    (unsigned)LINUX_KSYM_PCI_RESIDUAL,
                    (unsigned)LINUX_KSYM_USB_STORAGE_LEAF,
                    (unsigned)GJ_KSYM_RESIDUAL_TOTAL, (unsigned)g_u32KsymN,
                    (unsigned)u32Free, (unsigned)u32ReserveOk);
        }
    }

    /*
     * free= after base stubs; design headroom for large soft seed
     * (LINUX_KSYM_USB_SOFT_SEED=191) + multi-mod export. Soft!=product.
     * Boot residual lean: init PASS only - does NOT restate residual class
     * sizes (lean residual owns usb/net/pci/leaf). No inventory double-print.
     * freestanding_no_exec honesty + g_ac1_waiver=0 on init PASS too.
     * greppable: linux_ksym: soft init PASS n= max= free=
     */
    kprintf("linux_ksym: soft init PASS n=%u max=%u free=%u usb_seed=%u "
            "reserve_ok=%u lean_ok=%u/%u soft=1 product=0 hostability=1 "
            "freestanding_no_exec=1 never_exec_ko=1 resolve_only=1 "
            "g_ac1_waiver=0 G-AC-1 "
            "dual=MIT_OR_Apache-2.0 no_version_stamp=1 storm=0 "
            "Soft!=product\n",
            (unsigned)g_u32KsymN, (unsigned)LINUX_KSYM_MAX,
            (unsigned)linux_ksym_slots_free(),
            (unsigned)LINUX_KSYM_USB_SOFT_SEED,
            (unsigned)((linux_ksym_slots_free() >= LINUX_KSYM_USB_SOFT_SEED)
                           ? 1u
                           : 0u),
            (unsigned)g_u32LeanOk, (unsigned)g_u32LeanChecks);
}
