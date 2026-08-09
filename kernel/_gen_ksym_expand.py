#!/usr/bin/env python3
"""Generate expanded kernel/mm/linux_ksym.c for r8169 soft resolve."""
from __future__ import annotations

from pathlib import Path

ROOT = Path(__file__).resolve().parent
OUT = ROOT / "mm" / "linux_ksym.c"

# (kind, name) kind: I=return0 V=void Z=zero-data
# Deduped; order preserved for greppable inventory stability.
ENTRIES: list[tuple[str, str]] = []


def add(kind: str, *names: str) -> None:
    seen = {n for _, n in ENTRIES}
    for n in names:
        if n not in seen:
            ENTRIES.append((kind, n))
            seen.add(n)


# --- print / format / mem / str (printk handled specially) ---
add("I", "snprintf", "scnprintf", "vsnprintf", "sscanf")
add("I", "memcpy", "memset", "memcmp", "memmove", "memchr_inv")
add("I", "strlen", "strcmp", "strncmp", "strcpy", "strncpy", "strscpy")
add("I", "strchr", "strrchr", "strstr", "strpbrk", "strncpy_from_user", "_ctype")

# --- heap / vm ---
add("I", "kmalloc", "kzalloc", "kmalloc_trace", "kmalloc_caches")
add("I", "__kmalloc_noprof", "__kmalloc_cache_noprof", "kmalloc_trace_noprof")
add("I", "kfree", "kvfree", "kfree_sensitive", "kmem_cache_alloc_noprof", "kmem_cache_free")
add("I", "vmalloc", "vfree", "__vmalloc_node_range_noprof", "__alloc_pages_noprof", "__free_pages")
add("I", "page_offset_base", "phys_base", "vmemmap_base")
add("I", "___preempt_schedule", "___preempt_schedule_notrace")

# --- locks / RCU ---
add("V", "mutex_lock", "mutex_unlock", "mutex_destroy")
add("I", "mutex_lock_interruptible", "mutex_trylock", "__mutex_init", "mutex_lock_nested", "_mutex_lock_nest_lock")
add("V", "spin_lock", "spin_unlock", "spin_unlock_irqrestore")
add("I", "spin_lock_irqsave")
add("I", "_raw_spin_lock", "_raw_spin_lock_bh", "_raw_spin_lock_irq", "_raw_spin_lock_irqsave", "_raw_spin_trylock")
add("V", "_raw_spin_unlock", "_raw_spin_unlock_bh", "_raw_spin_unlock_irq", "_raw_spin_unlock_irqrestore")
add("I", "__refcount_add_not_zero", "__refcount_dec_and_test", "refcount_warn_saturate")
add("I", "__rcu_read_lock", "__rcu_read_unlock")
add("V", "synchronize_rcu", "call_rcu")
add("I", "do_trace_rcu_torture_read")
add("Z", "rcu_callback_map")
add("V", "local_bh_disable", "local_bh_enable")
add("I", "__local_bh_enable_ip", "preempt_count", "__preempt_count", "this_cpu_off", "__per_cpu_offset")
add("Z", "cpu_number")
add("I", "__cpu_online_mask", "__cpu_possible_mask")

# --- delay / time ---
add("V", "msleep", "mdelay", "udelay", "usleep_range")
add("I", "__const_udelay", "__udelay", "__ndelay")
add("Z", "jiffies")
add("I", "jiffies_to_usecs", "jiffies_to_msecs", "msecs_to_jiffies", "usecs_to_jiffies")
add("I", "ktime_get", "ktime_get_real_ts64", "ktime_get_with_offset", "ktime_get_mono_fast_ns")

# --- timers / wq / completions ---
add("I", "init_timer_key", "timer_init_key", "del_timer", "del_timer_sync")
add("I", "timer_delete", "timer_delete_sync", "timer_shutdown_sync", "mod_timer", "timer_reduce")
add("V", "add_timer")
add("I", "init_wait_entry", "prepare_to_wait_event", "__init_waitqueue_head", "__wake_up", "autoremove_wake_function")
add("V", "finish_wait", "complete")
add("I", "init_completion", "__init_swait_queue_head")
add("I", "wait_for_completion", "wait_for_completion_timeout", "try_wait_for_completion", "completion_done")
add("I", "__init_work", "queue_work_on", "schedule_work", "schedule_delayed_work", "queue_delayed_work_on")
add("I", "cancel_work_sync", "cancel_delayed_work", "cancel_delayed_work_sync")
add("I", "flush_work", "flush_delayed_work", "flush_workqueue", "destroy_workqueue")
add("I", "alloc_workqueue", "__alloc_workqueue")
add("Z", "system_wq")
add("I", "system_power_efficient_wq", "system_unbound_wq", "system_long_wq")
add("I", "schedule", "schedule_timeout", "io_schedule_timeout", "cond_resched")

# --- irq ---
add("I", "request_irq", "request_threaded_irq")
add("V", "free_irq", "enable_irq", "disable_irq", "disable_irq_nosync")
add("I", "irq_set_affinity_hint", "irq_get_irq_data", "irq_to_desc", "synchronize_irq", "__irq_resolve_mapping")

# --- mmio ---
add("I", "ioremap", "ioremap_noprof", "ioremap_nocache", "ioremap_wc", "ioport_map")
add("V", "iounmap", "ioport_unmap", "writel", "writeb", "writew", "writeq")
add("I", "readl", "readb", "readw", "readq")

# --- pci ---
add("I", "pci_register_driver", "__pci_register_driver")
add("V", "pci_unregister_driver", "pci_disable_device", "pci_set_master", "pci_clear_master")
add("V", "pci_release_regions", "pci_release_selected_regions", "pci_iounmap", "pci_free_irq_vectors")
add("I", "pci_enable_device", "pci_enable_device_mem", "pci_request_regions", "pci_request_selected_regions")
add("I", "pci_iomap", "pci_alloc_irq_vectors", "pci_alloc_irq_vectors_affinity", "pci_irq_vector")
add("I", "pci_find_capability")
add("I", "pci_read_config_byte", "pci_read_config_word", "pci_read_config_dword")
add("I", "pci_write_config_byte", "pci_write_config_word", "pci_write_config_dword")
add("I", "pci_set_power_state", "pci_choose_state", "pci_save_state", "pci_restore_state")
add("I", "pci_enable_wake", "pci_wake_from_d3", "pci_set_mwi", "pci_clear_mwi")
add("I", "pci_dma_sync_single_for_cpu", "pci_dma_sync_single_for_device")
add("I", "pcie_capability_read_word", "pcie_capability_write_word")
add("I", "pcie_capability_clear_and_set_word_unlocked", "pcie_capability_clear_and_set_word_locked")
add("I", "pcim_enable_device", "pcim_iomap_regions", "pcim_iomap_table")
add("I", "pci_dev_put", "pci_get_device", "pci_match_id", "pci_bus_type")

# --- dma ---
add("I", "dma_alloc_coherent", "dma_alloc_attrs", "dma_map_single", "dma_map_single_attrs", "dma_map_page")
add("V", "dma_free_coherent", "dma_free_attrs", "dma_unmap_single", "dma_unmap_single_attrs", "dma_unmap_page")
add("I", "dma_mapping_error", "dma_set_mask", "dma_set_coherent_mask", "dma_set_mask_and_coherent")
add("I", "dma_get_required_mask", "dma_sync_single_for_cpu", "dma_sync_single_for_device")
add("I", "dma_max_mapping_size", "dma_need_sync", "dma_opt_mapping_size")

# --- net ---
add("I", "alloc_etherdev_mqs", "alloc_netdev_mqs")
add("V", "free_netdev", "unregister_netdev", "netif_carrier_on", "netif_carrier_off")
add("V", "netif_start_queue", "netif_stop_queue", "netif_wake_queue", "dev_kfree_skb_any")
add("I", "register_netdev", "register_netdevice", "unregister_netdevice_queue", "unregister_netdevice_many")
add("I", "netif_carrier_event", "netif_tx_wake_queue", "netif_tx_stop_queue")
add("I", "netif_tx_start_all_queues", "netif_tx_stop_all_queues", "netif_schedule_queue")
add("I", "netif_set_real_num_tx_queues", "netif_set_real_num_rx_queues", "netif_set_tso_max_size")
add("I", "netif_set_affinity_auto", "netif_device_attach", "netif_device_detach", "netif_device_present")
add("I", "netif_napi_add_weight", "netif_receive_skb", "netif_receive_skb_list", "netif_rx")
add("I", "dev_get_stats", "dev_get_tstats64", "dev_addr_mod")
add("I", "dev_kfree_skb_any_reason", "dev_kfree_skb_irq_reason", "consume_skb", "kfree_skb_reason")
add("I", "skb_put", "skb_push", "skb_pull", "skb_copy", "skb_clone", "skb_copy_bits")
add("I", "skb_copy_and_csum_dev", "skb_checksum_help", "skb_tstamp_tx", "skb_queue_purge_reason", "skb_trim")
add("I", "__skb_pad", "__skb_gso_segment", "__netdev_alloc_skb", "__napi_alloc_skb", "__napi_schedule")
add("I", "napi_enable", "napi_disable", "napi_schedule", "napi_complete_done", "napi_gro_receive", "napi_consume_skb")
add("I", "netdev_rss_key_fill", "netdev_err", "netdev_warn", "netdev_info", "netdev_notice", "netdev_printk", "netdev_alert")
add("I", "netdev_state_change", "netdev_notify_peers", "netdev_update_features", "netdev_sw_irq_coalesce_default_on")
add("I", "eth_type_trans", "eth_validate_addr", "eth_platform_get_mac_address", "eth_hw_addr_set", "ether_setup")
add("I", "register_inetaddr_notifier", "unregister_inetaddr_notifier")
add("I", "register_netdevice_notifier", "unregister_netdevice_notifier")
add("I", "rtnl_is_locked", "rtnl_lock", "rtnl_unlock")
add("Z", "init_net")
add("I", "net_ratelimit", "linkwatch_fire_event", "__dev_kfree_skb_any", "synchronize_net")
add("I", "ethtool_op_get_link", "ethtool_op_get_ts_info", "ethtool_sprintf", "ethtool_puts")
add("I", "ethtool_convert_link_mode_to_legacy_u32", "ethtool_convert_legacy_u32_to_link_mode")
add("I", "ethtool_virtdev_set_link_ksettings", "__ethtool_get_link_ksettings", "__ethtool_get_ts_info")

# --- phy / mdio ---
add(
    "I",
    "phy_connect", "phy_connect_direct", "phy_disconnect", "phy_start", "phy_stop", "phy_start_aneg",
    "phy_init_hw", "phy_attached_info", "phy_print_status",
    "phy_ethtool_ksettings_get", "phy_ethtool_ksettings_set", "phy_ethtool_get_eee",
    "phy_ethtool_set_eee_noneg", "phy_ethtool_nway_reset", "phy_get_eee_err",
    "phy_support_eee", "phy_support_asym_pause", "phy_set_asym_pause", "phy_set_max_speed",
    "phy_set_sym_pause", "phy_resolve_aneg_pause",
    "phy_read_mmd", "phy_write_mmd", "phy_modify_mmd", "phy_modify_paged", "phy_read_paged",
    "phy_restore_page", "phy_save_page", "phy_select_page",
    "phy_drivers_register", "phy_drivers_unregister", "phy_driver_register", "phy_driver_unregister",
    "phy_mac_interrupt", "phy_mii_ioctl", "phy_speed_to_str",
    "genphy_read_status", "genphy_resume", "genphy_suspend", "genphy_soft_reset", "genphy_update_link",
    "genphy_restart_aneg", "genphy_check_and_restart_aneg", "genphy_read_lpa", "genphy_read_abilities",
    "genphy_c45_pma_resume", "genphy_c45_pma_suspend", "genphy_c45_pma_read_abilities",
    "genphy_c45_read_status", "genphy_c45_config_aneg", "genphy_c45_eee_is_active",
    "genphy_c45_an_config_eee_aneg", "genphy_c45_write_eee_adv",
    "mdiobus_alloc_size", "mdiobus_free", "mdiobus_register", "__mdiobus_register", "mdiobus_unregister",
    "mdiobus_get_phy", "mdiobus_read", "mdiobus_write", "__mdiobus_read", "__mdiobus_write",
    "mdio_device_get_optional", "mdio45_ethtool_gset_npage", "mdio45_ethtool_ksettings_get_npage", "mdio_set_flag",
    "get_phy_device", "phy_device_remove", "phy_device_register", "phy_device_free",
    "fwnode_get_phy_node", "fwnode_mdiobus_register_phy",
)

# --- device model ---
add(
    "I",
    "device_set_wakeup_enable", "device_set_wakeup_capable", "device_set_node",
    "device_get_match_data", "device_get_phy_mode", "device_get_named_child_node",
    "device_property_present", "device_property_read_u32", "device_property_read_u8_array",
    "device_property_read_string", "device_create_file", "device_remove_file",
    "device_link_add", "device_link_del", "dev_driver_string",
    "dev_err", "dev_warn", "dev_info", "dev_notice",
    "_dev_err", "_dev_warn", "_dev_info", "_dev_notice", "_dev_printk",
    "devm_kmalloc", "devm_kzalloc", "devm_kfree", "devm_request_threaded_irq", "devm_free_irq",
    "devm_ioremap", "devm_ioremap_resource", "devm_platform_ioremap_resource",
    "devm_kasprintf", "devm_kstrdup", "put_device", "get_device",
)

# --- of / fwnode ---
add(
    "I",
    "of_node_put", "of_node_get", "of_find_node_by_name", "of_get_property",
    "of_property_read_variable_u32_array", "of_property_read_string", "of_device_get_match_data",
    "fwnode_handle_put", "fwnode_handle_get", "fwnode_property_present", "fwnode_property_read_u32",
    "fwnode_get_parent", "fwnode_get_next_child_node", "fwnode_device_is_available",
)

# --- firmware / module / params ---
add(
    "I",
    "request_firmware", "request_firmware_direct", "request_firmware_nowait", "release_firmware",
    "firmware_request_nowarn", "try_module_get", "__module_get", "module_put_and_exit",
    "__request_module", "find_module", "__symbol_get", "__symbol_put",
    "param_get_int", "param_set_int", "param_get_uint", "param_set_uint",
    "param_get_ullong", "param_set_ullong", "param_get_bool", "param_set_bool",
    "param_get_charp", "param_set_charp", "param_get_string", "param_set_copystring",
    "param_array_get", "param_array_set", "kernel_param_lock", "kernel_param_unlock",
)
add("V", "module_put")
add("Z", "param_ops_int", "param_ops_uint", "param_ops_ullong", "param_ops_bool", "param_ops_charp", "param_ops_string")

# --- pm ---
add(
    "I",
    "pm_runtime_enable", "pm_runtime_disable", "pm_runtime_get_sync", "pm_runtime_put", "pm_runtime_put_sync",
    "pm_runtime_set_autosuspend_delay", "pm_runtime_use_autosuspend", "pm_schedule_suspend",
    "pm_wakeup_dev_event", "pm_suspend_default_s2idle", "dpm_suspend_start", "dpm_resume_end",
)
add("Z", "system_state")

# --- usb ---
add("I", "usb_register", "usb_submit_urb", "usb_unlink_urb", "usb_alloc_urb")
add("V", "usb_deregister", "usb_free_urb")

# --- crypto / bitops / misc ---
add(
    "I",
    "crc32_le", "crc32_le_base", "crc32_be", "__crc32c_le",
    "__sw_hweight32", "__sw_hweight64", "__sw_hweight16", "__sw_hweight8",
    "_find_first_bit", "_find_next_bit", "_find_first_zero_bit", "_find_next_zero_bit",
    "__bitmap_and", "__bitmap_or", "__bitmap_weight", "__bitmap_equal", "__bitmap_andnot",
    "bitmap_zero", "bitmap_fill", "find_next_bit", "find_first_bit",
    "cpumask_next", "cpumask_next_and", "cpumask_local_spread", "nr_cpu_ids",
    "__arch_hweight32", "__arch_hweight64",
    "kstrtoint", "kstrtouint", "kstrtoul", "kstrtoull", "kstrtobool",
    "simple_strtoul", "simple_strtol", "kasprintf", "kvasprintf", "kstrdup", "kstrndup", "match_string",
    "sysfs_create_group", "sysfs_remove_group", "sysfs_notify", "sysfs_emit",
    "device_add_groups", "device_remove_groups",
    "debugfs_create_dir", "debugfs_create_file", "debugfs_remove", "debugfs_remove_recursive",
    "proc_create_data", "proc_remove", "seq_printf", "seq_puts", "seq_putc", "seq_write",
    "single_open", "single_release", "seq_read", "seq_lseek",
    "dump_stack", "warn_slowpath_fmt", "__warn_printk",
    "__ubsan_handle_load_invalid_value", "__ubsan_handle_shift_out_of_bounds",
    "__ubsan_handle_add_overflow", "__ubsan_handle_sub_overflow", "__ubsan_handle_out_of_bounds",
    "__cant_sleep", "__cant_migrate", "might_fault", "__might_fault", "__might_sleep", "__might_resched",
    "debug_smp_processor_id", "smp_call_function_single", "on_each_cpu",
    "kernel_read_file_from_path_initns", "kernel_write", "kernel_read",
    "filp_open", "filp_close", "fsync_bdev",
    "get_random_bytes", "get_random_u32", "get_random_u64", "add_device_randomness",
    "_copy_from_user", "_copy_to_user", "copy_from_user", "copy_to_user",
    "__fortify_panic", "__stack_chk_guard",
    "__dynamic_dev_dbg", "__dynamic_netdev_dbg", "__dynamic_pr_debug",
)

# --- static keys / trace ---
add(
    "I",
    "static_key_slow_inc", "static_key_slow_dec", "static_key_count",
    "static_key_enable", "static_key_disable",
    "static_key_enable_cpuslocked", "static_key_disable_cpuslocked",
    "jump_label_update_timeout", "__static_key_slow_dec_deferred",
    "__static_key_deferred_flush", "static_key_slow_inc_cpuslocked",
    "__tracepoint_napi_poll", "__traceiter_napi_poll", "__probestub_napi_poll",
    "__SCT__tp_func_napi_poll", "__tracepoint_kfree_skb", "__tracepoint_consume_skb",
)

# --- lockdep ---
add(
    "I",
    "lockdep_init_map_type", "lockdep_set_class_and_name",
    "lock_acquire", "lock_release", "lock_contended", "lock_acquired",
    "debug_locks", "debug_lockdep_rcu_enabled",
    "rcu_read_lock_held", "rcu_read_lock_bh_held", "rcu_is_watching",
)

# --- r8169 shaped extras ---
add(
    "I",
    "capability_intersects", "capability_and", "capability_andnot",
    "linkmode_resolve_pause", "linkmode_set_pause", "linkmode_copy", "linkmode_zero",
    "linkmode_and", "linkmode_or", "linkmode_andnot", "linkmode_empty", "linkmode_equal",
    "phylink_set_port_modes", "phylink_create", "phylink_destroy",
    "phylink_connect_phy", "phylink_disconnect_phy", "phylink_start", "phylink_stop",
    "phylink_resume", "phylink_suspend",
    "phylink_ethtool_ksettings_get", "phylink_ethtool_ksettings_set",
    "phylink_ethtool_get_wol", "phylink_ethtool_set_wol",
    "phylink_ethtool_get_eee", "phylink_ethtool_set_eee",
    "phylink_mii_ioctl", "phylink_speed_down", "phylink_speed_up", "phylink_mac_change",
    "phylink_of_phy_connect", "phylink_fwnode_phy_connect",
    "pcs_get_state", "pcs_config", "pcs_an_restart", "pcs_link_up",
    "fixed_phy_register", "fixed_phy_unregister", "fixed_phy_change_carrier",
    "dsa_uses_xmit_special", "netdev_set_default_ethtool_ops", "netif_stacked_transfer_operstate",
    "dev_uc_add", "dev_uc_del", "dev_mc_add", "dev_mc_del",
    "dev_set_mtu", "dev_set_mac_address", "dev_change_flags", "dev_open", "dev_close",
    "dev_queue_xmit", "__dev_queue_xmit", "call_netdevice_notifiers",
    "netif_tx_lock", "netif_tx_unlock", "netif_addr_lock_bh", "netif_addr_unlock_bh",
    "__hw_addr_sync", "__hw_addr_unsync", "__hw_addr_sync_dev", "__hw_addr_unsync_dev",
    "page_pool_alloc_pages", "page_pool_create", "page_pool_destroy",
    "page_pool_put_page", "page_pool_release_page",
    "xdp_rxq_info_reg", "xdp_rxq_info_unreg", "xdp_rxq_info_reg_mem_model",
    "xdp_return_frame", "xdp_do_redirect", "xdp_convert_frame_to_buff",
    "bpf_warn_invalid_xdp_action", "bpf_prog_put", "bpf_prog_add", "bpf_prog_inc", "bpf_prog_sub",
    "rps_may_expire_flow", "netdev_rx_csum_fault", "skb_checksum_setup", "skb_cow_data",
    "pskb_expand_head", "__pskb_pull_tail", "skb_add_rx_frag_netmem",
    "napi_get_frags", "napi_gro_frags", "napi_alloc_skb", "build_skb", "napi_build_skb",
    "skb_free_frag", "page_frag_free", "__page_frag_cache_drain", "__alloc_skb",
    "__netdev_alloc_frag_align", "__napi_alloc_frag_align", "netdev_alloc_frag", "napi_alloc_frag",
    "csum_partial", "csum_tcpudp_nofold", "csum_ipv6_magic", "ip_send_check",
    "inet_proto_csum_replace4", "inet_proto_csum_replace16", "skb_partial_csum_set",
    "flow_keys_dissector", "skb_flow_dissect_flow_keys", "netdev_pick_tx", "skb_tx_hash",
    "netdev_features_change", "netif_set_xps_queue", "netif_set_xps_queue_locked", "__netif_set_xps_queue",
    "cpu_latency_qos_add_request", "cpu_latency_qos_remove_request", "cpu_latency_qos_update_request",
    "freq_qos_add_request", "freq_qos_remove_request",
    "pm_qos_add_request", "pm_qos_remove_request", "pm_qos_update_request",
    "clk_get", "clk_put", "clk_prepare", "clk_unprepare", "clk_enable", "clk_disable", "clk_get_rate",
    "reset_control_get", "reset_control_put", "reset_control_assert", "reset_control_deassert", "reset_control_reset",
    "gpiod_get_optional", "gpiod_put", "gpiod_set_value_cansleep", "gpiod_get_value_cansleep",
    "gpiod_direction_output", "gpiod_direction_input",
    "regulator_get_optional", "regulator_put", "regulator_enable", "regulator_disable", "regulator_set_voltage",
    "platform_get_irq", "platform_get_resource", "platform_device_register", "platform_device_unregister",
    "platform_driver_unregister", "__platform_driver_register",
    # extra r8169 unds often present
    "netif_napi_del", "netif_tx_disable", "netif_queue_set_napi",
    "netdev_reset_tc", "netdev_set_num_tc", "netdev_set_tc_queue",
    "dev_sw_netstats_rx_add", "dev_sw_netstats_tx_add",
    "page_pool_alloc_netmems", "page_pool_put_unrefed_netmem",
    "skb_mark_for_recycle", "napi_skb_free_stolen_head",
    "xdp_do_flush", "xdp_warn", "bpf_dispatcher_xdp_func",
    "net_selftest", "net_selftest_get_count", "net_selftest_get_strings",
    "phy_attached_print", "phy_detach", "phy_trigger_machine",
    "rtl_fw", "r8169_get_tx_lpi_timer_us",  # if present as und; harmless if not used
    "fs_reclaim_acquire", "fs_reclaim_release",
    "lock_is_held_type", "rcu_lockdep_current_cpu_online",
    "schedule_timeout_interruptible", "schedule_timeout_uninterruptible",
    "msleep_interruptible", "ssleep",
    "init_timer_on_stack_key", "destroy_timer_on_stack",
    "from_kuid_munged", "from_kgid_munged",
    "netlink_unicast", "netlink_broadcast",
    "nla_put", "nla_memcpy", "nla_strscpy",
    "skb_clone_tx_timestamp", "skb_defer_rx_timestamp",
    "ptp_clock_register", "ptp_clock_unregister", "ptp_clock_index",
    "timecounter_init", "timecounter_read", "timecounter_cyc2time",
    "clocksource_mmio_readl_up", "clocksource_mmio_readl_down",
)

# --- boot / pv ---
add("Z", "boot_cpu_data", "pv_ops")
add(
    "I",
    "pv_is_native_spin_unlock", "pv_native_queued_spin_unlock",
    "__pv_queued_spin_lock_slowpath", "__pv_queued_spin_unlock_slowpath",
    "native_queued_spin_lock_slowpath", "queued_spin_lock_slowpath",
    "node_states", "contig_page_data", "node_data",
)

# --- toolchain / retpoline ---
add("V", "__fentry__", "__stack_chk_fail", "_mcount", "mcount")
add("V", "__x86_return_thunk", "__x86_indirect_thunk_array", "__x86_indirect_jump_thunk_array", "__x86_clean_retpoline")

THUNK_REGS = ["rax", "rbx", "rcx", "rdx", "rsi", "rdi", "rbp", "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"]
JTHUNK_REGS = ["rax", "rcx", "rdx", "rsi", "rdi", "r8", "r9", "r10", "r11"]


def emit() -> str:
    lines: list[str] = []
    a = lines.append

    a("/*")
    a(" * SPDX-License-Identifier: MIT OR Apache-2.0")
    a(" * Copyright (c) 2026 Project GreenJade contributors")
    a(" *")
    a(" * Soft Linux-style kernel symbol table for resolving .ko undefined symbols.")
    a(" * Clean-room dual MIT OR Apache-2.0 only. Soft ≠ product Linux ABI completeness.")
    a(" * Not Linux source / not GPL. Stubs are weak empty no-ops (return 0 / void / zero data).")
    a(" *")
    a(" * Expanded for host r8169.ko unresolved surface (nm -u) + common module helpers")
    a(" * (__fentry__, __x86_return_thunk, __x86_indirect_thunk_*, pv_ops, …).")
    a(" *")
    a(" * Fixed table: LINUX_KSYM_MAX entries; names are C strings.")
    a(" *")
    a(" * Greppable:")
    a(" *   linux_ksym: soft init PASS")
    a(" *   linux_ksym: soft inventory n=…")
    a(" */")
    a("#include <gj/klog.h>")
    a("#include <gj/linux_ksym.h>")
    a("#include <gj/string.h>")
    a("#include <gj/types.h>")
    a("")
    a("/* ---- Table ---------------------------------------------------------------- */")
    a("")
    a("struct linux_ksym_ent {")
    a("    const char *szName;")
    a("    void       *pAddr;")
    a("};")
    a("")
    a("static struct linux_ksym_ent g_aKsym[LINUX_KSYM_MAX];")
    a("static u32                   g_cKsym;")
    a("static int                   g_fInited;")
    a("static int                   g_fInventoryDone;")
    a("")
    a("/* ---- Soft stubs (weak empty; return 0 / void / zero data) ----------------- */")
    a("/*")
    a(" * Unique C identifiers avoid collisions with freestanding GJ helpers")
    a(" * (memcpy/memset/…). Table maps Linux .ko symbol *strings* → these addresses.")
    a(" * Weak so a later product TU may override the C symbol if linked; registration")
    a(" * still pins the soft address at init unless re-registered.")
    a(" *")
    a(" * Clean-room only: shape-level no-ops. Do NOT copy Linux source.")
    a(" */")
    a("")
    a("#define GJ_KSYM_STUB_I(name)                                                   \\")
    a("    __attribute__((weak)) long gj_ksym_soft_##name(void);                      \\")
    a("    __attribute__((weak)) long gj_ksym_soft_##name(void)                       \\")
    a("    {                                                                          \\")
    a("        return 0;                                                              \\")
    a("    }")
    a("")
    a("#define GJ_KSYM_STUB_V(name)                                                   \\")
    a("    __attribute__((weak)) void gj_ksym_soft_##name(void);                      \\")
    a("    __attribute__((weak)) void gj_ksym_soft_##name(void)                       \\")
    a("    {                                                                          \\")
    a("    }")
    a("")
    a("/* Soft zero object for data-like symbols (jiffies, pv_ops, …). */")
    a("#define GJ_KSYM_STUB_Z(name)                                                   \\")
    a("    __attribute__((weak)) unsigned long gj_ksym_soft_##name = 0")
    a("")
    a("/*")
    a(" * printk-like: soft lamp via kprintf (kernel always has kprintf).")
    a(" * Soft ≠ product printk; no full varargs formatting.")
    a(" */")
    a("__attribute__((weak)) long gj_ksym_soft_printk(const char *fmt, ...);")
    a("__attribute__((weak)) long")
    a("gj_ksym_soft_printk(const char *fmt, ...)")
    a("{")
    a("    if (fmt != 0)")
    a('        kprintf("linux_ksym soft printk\\n");')
    a("    return 0;")
    a("}")
    a("")
    a("__attribute__((weak)) long gj_ksym_soft__printk(const char *fmt, ...);")
    a("__attribute__((weak)) long")
    a("gj_ksym_soft__printk(const char *fmt, ...)")
    a("{")
    a("    if (fmt != 0)")
    a('        kprintf("linux_ksym soft _printk\\n");')
    a("    return 0;")
    a("}")
    a("")

    for kind, name in ENTRIES:
        if kind == "I":
            a(f"GJ_KSYM_STUB_I({name})")
        elif kind == "V":
            a(f"GJ_KSYM_STUB_V({name})")
        elif kind == "Z":
            a(f"GJ_KSYM_STUB_Z({name})")
        else:
            raise SystemExit(f"bad kind {kind} for {name}")

    a("")
    a("/*")
    a(" * Retpoline soft: jmp *%reg style — soft null (no real retpoline).")
    a(" * Soft ≠ product Spectre mitigation.")
    a(" */")
    a("#define GJ_KSYM_THUNK(reg)                                                     \\")
    a("    __attribute__((weak)) void gj_ksym_soft___x86_indirect_thunk_##reg(void);  \\")
    a("    __attribute__((weak)) void                                                 \\")
    a("    gj_ksym_soft___x86_indirect_thunk_##reg(void)                              \\")
    a("    {                                                                          \\")
    a("        /* soft null — product would: jmp *%reg */                             \\")
    a("    }")
    a("")
    for reg in THUNK_REGS:
        a(f"GJ_KSYM_THUNK({reg})")
    a("#undef GJ_KSYM_THUNK")
    a("")
    a("#define GJ_KSYM_JTHUNK(reg)                                                    \\")
    a("    __attribute__((weak)) void                                                 \\")
    a("        gj_ksym_soft___x86_indirect_jump_thunk_##reg(void);                    \\")
    a("    __attribute__((weak)) void                                                 \\")
    a("    gj_ksym_soft___x86_indirect_jump_thunk_##reg(void)                        \\")
    a("    {                                                                          \\")
    a("    }")
    a("")
    for reg in JTHUNK_REGS:
        a(f"GJ_KSYM_JTHUNK({reg})")
    a("#undef GJ_KSYM_JTHUNK")
    a("")
    a("#undef GJ_KSYM_STUB_I")
    a("#undef GJ_KSYM_STUB_V")
    a("#undef GJ_KSYM_STUB_Z")
    a("")
    a("/* ---- Helpers -------------------------------------------------------------- */")
    a("")
    a("static int")
    a("ksym_name_eq(const char *szA, const char *szB)")
    a("{")
    a("    if (szA == 0 || szB == 0)")
    a("        return 0;")
    a("    return strcmp(szA, szB) == 0;")
    a("}")
    a("")
    a("static int")
    a("ksym_find_slot(const char *szName, u32 *puOut)")
    a("{")
    a("    u32 i;")
    a("")
    a("    for (i = 0; i < g_cKsym; i++) {")
    a("        if (ksym_name_eq(g_aKsym[i].szName, szName)) {")
    a("            if (puOut != 0)")
    a("                *puOut = i;")
    a("            return 1;")
    a("        }")
    a("    }")
    a("    return 0;")
    a("}")
    a("")
    a("/* Always take address: works for function and data soft stubs. */")
    a("#define GJ_KSYM_REG(name)                                                      \\")
    a("    (void)linux_ksym_register(#name, (void *)&gj_ksym_soft_##name)")
    a("")
    a("static void")
    a("ksym_register_starters(void)")
    a("{")
    a("    /* print / format */")
    a("    GJ_KSYM_REG(printk);")
    a("    GJ_KSYM_REG(_printk);")
    for _, name in ENTRIES:
        a(f"    GJ_KSYM_REG({name});")
    for reg in THUNK_REGS:
        a(f"    GJ_KSYM_REG(__x86_indirect_thunk_{reg});")
    for reg in JTHUNK_REGS:
        a(f"    GJ_KSYM_REG(__x86_indirect_jump_thunk_{reg});")
    a("}")
    a("")
    a("#undef GJ_KSYM_REG")
    a("")
    a("/* ---- Public API ----------------------------------------------------------- */")
    a("")
    a("void")
    a("linux_ksym_init(void)")
    a("{")
    a("    if (g_fInited) {")
    a('        kprintf("linux_ksym: soft init PASS (idempotent n=%u)\\n", g_cKsym);')
    a("        return;")
    a("    }")
    a("")
    a("    g_cKsym          = 0;")
    a("    g_fInventoryDone = 0;")
    a("    ksym_register_starters();")
    a("    g_fInited = 1;")
    a("")
    a("    /* Grep: linux_ksym: soft init PASS */")
    a('    kprintf("linux_ksym: soft init PASS n=%u max=%u\\n", g_cKsym,')
    a("            (u32)LINUX_KSYM_MAX);")
    a("}")
    a("")
    a("int")
    a("linux_ksym_register(const char *szName, void *pAddr)")
    a("{")
    a("    u32 uSlot;")
    a("")
    a("    if (szName == 0 || pAddr == 0 || szName[0] == '\\0')")
    a("        return -1;")
    a("")
    a("    if (ksym_find_slot(szName, &uSlot)) {")
    a("        g_aKsym[uSlot].pAddr = pAddr;")
    a("        return 0;")
    a("    }")
    a("")
    a("    if (g_cKsym >= LINUX_KSYM_MAX)")
    a("        return -1;")
    a("")
    a("    g_aKsym[g_cKsym].szName = szName;")
    a("    g_aKsym[g_cKsym].pAddr  = pAddr;")
    a("    g_cKsym++;")
    a("    return 0;")
    a("}")
    a("")
    a("void *")
    a("linux_ksym_lookup(const char *szName)")
    a("{")
    a("    u32 uSlot;")
    a("")
    a("    if (szName == 0 || !g_fInited)")
    a("        return 0;")
    a("    if (!ksym_find_slot(szName, &uSlot))")
    a("        return 0;")
    a("    return g_aKsym[uSlot].pAddr;")
    a("}")
    a("")
    a("u32")
    a("linux_ksym_count(void)")
    a("{")
    a("    return g_cKsym;")
    a("}")
    a("")
    a("void")
    a("linux_ksym_soft_inventory(void)")
    a("{")
    a("    if (g_fInventoryDone)")
    a("        return;")
    a("    g_fInventoryDone = 1;")
    a("")
    a("    /* Grep: linux_ksym: soft inventory n= */")
    a('    kprintf("linux_ksym: soft inventory n=%u max=%u inited=%u "')
    a('            "(soft≠product Linux ABI)\\n",')
    a("            g_cKsym, (u32)LINUX_KSYM_MAX, g_fInited ? 1u : 0u);")
    a("}")
    a("")

    n = 2 + len(ENTRIES) + len(THUNK_REGS) + len(JTHUNK_REGS)  # printk + _printk
    return "\n".join(lines), n


def main() -> None:
    text, n = emit()
    OUT.write_text(text)
    print(f"wrote {OUT} bytes={OUT.stat().st_size} inventory_n≈{n} entries={len(ENTRIES)}")


if __name__ == "__main__":
    main()
