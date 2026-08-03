/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft Linux ksym table for .ko unresolved resolve (clean-room stubs).
 * Soft≠product Linux ABI. Dual MIT OR Apache-2.0. No GPL source.
 * greppable: linux_ksym: soft init PASS | linux_ksym: soft inventory n=
 */
#include <gj/linux_ksym.h>
#include <gj/klog.h>
#include <gj/string.h>
#include <gj/types.h>

#ifndef LINUX_KSYM_MAX
#define LINUX_KSYM_MAX 1024u
#endif

struct gj_ksym_ent {
    const char *szName;
    void *pAddr;
};

static struct gj_ksym_ent g_aKsym[LINUX_KSYM_MAX];
static u32 g_u32KsymN;
static u8 g_fInit;
static u8 g_fInvOnce;

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
static long gj_ksym_soft_firmware_request_nowarn(void) __attribute__((used));
static long gj_ksym_soft_firmware_request_nowarn(void) { return 0; }
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

int
linux_ksym_register(const char *name, void *addr)
{
    u32 i;

    if (name == 0 || name[0] == '\0' || addr == 0) {
        return -1;
    }
    for (i = 0; i < g_u32KsymN; i++) {
        if (g_aKsym[i].szName != 0 && strcmp(g_aKsym[i].szName, name) == 0) {
            g_aKsym[i].pAddr = addr;
            return 0;
        }
    }
    if (g_u32KsymN >= LINUX_KSYM_MAX) {
        return -1;
    }
    g_aKsym[g_u32KsymN].szName = name;
    g_aKsym[g_u32KsymN].pAddr = addr;
    g_u32KsymN++;
    return 0;
}

void *
linux_ksym_lookup(const char *name)
{
    u32 i;

    if (!g_fInit || name == 0) {
        return 0;
    }
    for (i = 0; i < g_u32KsymN; i++) {
        if (g_aKsym[i].szName != 0 && strcmp(g_aKsym[i].szName, name) == 0) {
            return g_aKsym[i].pAddr;
        }
    }
    return 0;
}

u32
linux_ksym_count(void)
{
    return g_u32KsymN;
}

void
linux_ksym_soft_inventory(void)
{
    if (g_fInvOnce) {
        return;
    }
    g_fInvOnce = 1;
    kprintf("linux_ksym: soft inventory n=%u max=%u soft=1 product=0\n",
            (unsigned)g_u32KsymN, (unsigned)LINUX_KSYM_MAX);
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

    kprintf("linux_ksym: soft init PASS n=%u max=%u soft=1 product=0\n",
            (unsigned)g_u32KsymN, (unsigned)LINUX_KSYM_MAX);
    linux_ksym_soft_inventory();
}
