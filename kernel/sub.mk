srcs-$(CFG_CORE_SANITIZE_KADDRESS) += asan.c
cflags-remove-asan.c-y += $(cflags_kasan)
srcs-$(CFG_TEE_CORE_DEBUG) += spin_lock_debug.c
srcs-y += assert.c
srcs-y += console.c
srcs-$(CFG_DT) += dt.c
srcs-$(CFG_DT) += dt_driver.c
srcs-y += pm.c
srcs-y += handle.c
srcs-y += interrupt.c
ifeq ($(CFG_WITH_USER_TA),y)
srcs-y += ldelf_loader.c
srcs-y += ldelf_syscalls.c
srcs-y += scall.c
endif
srcs-$(CFG_LOCKDEP) += lockdep.c
ifneq ($(CFG_CORE_FFA),y)
srcs-$(CFG_CORE_DYN_SHM) += msg_param.c
endif
srcs-y += panic.c
srcs-y += trace_ext.c
srcs-y += refcount.c
srcs-y += delay.c
srcs-y += tee_time.c
srcs-$(CFG_SECURE_TIME_SOURCE_REE) += tee_time_ree.c
srcs-y += otp_stubs.c
srcs-y += tee_misc.c
srcs-y += tee_ta_manager.c
srcs-y += ts_manager.c
srcs-$(CFG_CORE_SANITIZE_UNDEFINED) += ubsan.c
srcs-y += scattered_array.c
srcs-y += huk_subkey.c
srcs-$(CFG_SHOW_CONF_ON_BOOT) += show_conf.c
srcs-y += user_mode_ctx.c
srcs-$(CFG_CORE_TPM_EVENT_LOG) += tpm.c
srcs-y += initcall.c
srcs-$(CFG_WITH_USER_TA) += user_access.c
srcs-y += mutex.c
srcs-$(CFG_LOCKDEP) += mutex_lockdep.c
srcs-y += wait_queue.c
srcs-y += notif.c
srcs-$(_CFG_CORE_ASYNC_NOTIF_DEFAULT_IMPL) += notif_default.c
srcs-y += thread.c

ifeq ($(CFG_WITH_USER_TA),y)
srcs-y += user_ta.c
srcs-$(CFG_REE_FS_TA) += ree_fs_ta.c
srcs-$(CFG_EARLY_TA) += early_ta.c
srcs-$(CFG_SECSTOR_TA) += secstor_ta.c
endif

srcs-$(CFG_EMBEDDED_TS) += embedded_ts.c
srcs-y += pseudo_ta.c

srcs-$(CFG_TRANSFER_LIST) += transfer_list.c

srcs-$(CFG_SEMIHOSTING) += semihosting.c

ifeq ($(CFG_SYSCALL_FTRACE),y)
# We would not like to profile spin_lock_debug.c file as it provides
# common APIs that are needed for ftrace framework to trace syscalls.
# So profiling this file could create an incorrect cyclic behaviour.
cflags-remove-spin_lock_debug.c-$(CFG_TEE_CORE_DEBUG) += -pg
endif
srcs-y += nv_counter.c
srcs-$(CFG_CALLOUT) += callout.c

srcs-$(CFG_LWIP_STACK) += minho_lwip.c

ifeq ($(CFG_LWIP_STACK),y)
srcs-y += core/dns.c
srcs-y += netif/ethernet.c
#srcs-y += server_side.c
#srcs-y += client_side.c
srcs-y += sys_arch.c
srcs-y += core/init.c
srcs-y += core/mem.c
srcs-y += core/memp.c
srcs-y += core/netif.c
srcs-y += core/udp.c
srcs-y += core/tcp.c
srcs-y += core/ipv4/igmp.c
srcs-y += core/stats.c
srcs-y += netif/ppp/ppp.c
srcs-y += core/timeouts.c
srcs-y += core/raw.c
srcs-y += core/ipv4/acd.c
srcs-y += core/ipv4/ip4.c
srcs-y += core/pbuf.c
srcs-y += core/def.c
srcs-y += core/ipv4/ip4_addr.c
srcs-y += core/ipv4/etharp.c
srcs-y += core/ipv4/dhcp.c
srcs-y += core/ipv4/autoip.c
srcs-y += core/ip.c
srcs-y += core/inet_chksum.c
srcs-y += core/ipv4/icmp.c
srcs-y += core/tcp_out.c
srcs-y += core/tcp_in.c
srcs-y += netif/ppp/pppos.c
srcs-y += netif/ppp/pppoe.c
srcs-y += netif/ppp/magic.c
srcs-y += netif/ppp/lcp.c
srcs-y += netif/ppp/auth.c
srcs-y += netif/ppp/ccp.c
srcs-y += netif/ppp/chap-md5.c
srcs-y += netif/ppp/chap_ms.c
srcs-y += netif/ppp/chap-new.c
srcs-y += netif/ppp/demand.c
srcs-y += netif/ppp/eap.c
srcs-y += netif/ppp/ecp.c
srcs-y += netif/ppp/eui64.c
srcs-y += netif/ppp/fsm.c
srcs-y += netif/ppp/ipcp.c
srcs-y += netif/ppp/ipv6cp.c
srcs-y += netif/ppp/mppe.c
srcs-y += netif/ppp/multilink.c
srcs-y += netif/ppp/pppapi.c
srcs-y += netif/ppp/pppcrypt.c
srcs-y += netif/ppp/pppol2tp.c
srcs-y += netif/ppp/upap.c
srcs-y += netif/ppp/utils.c
srcs-y += netif/ppp/vj.c
srcs-y += core/ipv4/ip4_frag.c
srcs-y += netif/ppp/polarssl/md5.c
srcs-y += tcpecho_raw.c
endif

