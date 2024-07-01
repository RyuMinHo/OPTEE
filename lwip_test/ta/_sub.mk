global-incdirs-y += include

#srcs-y += api_lib.c
#srcs-y += api_msg.c
srcs-y += lwip_test_ta.c
#srcs-y += sys.c
#srcs-y += sys_arch.c
#srcs-y += tcpip.c
srcs-y += default_netif.c
#srcs-y += tapif.c

srcs-y += ./netif/sio.c

srcs-y += ./core/init.c
srcs-y += ./core/def.c
srcs-y += ./core/dns.c
srcs-y += ./core/inet_chksum.c
srcs-y += ./core/ip.c
srcs-y += ./core/mem.c
srcs-y += ./core/memp.c
srcs-y += ./core/netif.c
srcs-y += ./core/pbuf.c
srcs-y += ./core/raw.c
srcs-y += ./core/stats.c
srcs-y += ./core/sys.c
srcs-y += ./core/altcp.c
srcs-y += ./core/altcp_alloc.c
srcs-y += ./core/altcp_tcp.c
srcs-y += ./core/tcp.c
srcs-y += ./core/tcp_in.c
srcs-y += ./core/tcp_out.c
srcs-y += ./core/timeouts.c
srcs-y += ./core/udp.c

srcs-y += ./core/ipv4/acd.c
srcs-y += ./core/ipv4/autoip.c
srcs-y += ./core/ipv4/dhcp.c
srcs-y += ./core/ipv4/etharp.c
srcs-y += ./core/ipv4/icmp.c
srcs-y += ./core/ipv4/igmp.c
srcs-y += ./core/ipv4/ip4_frag.c
srcs-y += ./core/ipv4/ip4.c
srcs-y += ./core/ipv4/ip4_addr.c


srcs-y += ./core/ipv6/dhcp6.c
srcs-y += ./core/ipv6/ethip6.c
srcs-y += ./core/ipv6/icmp6.c
srcs-y += ./core/ipv6/inet6.c
srcs-y += ./core/ipv6/ip6.c
srcs-y += ./core/ipv6/ip6_addr.c
srcs-y += ./core/ipv6/ip6_frag.c
srcs-y += ./core/ipv6/mld6.c
srcs-y += ./core/ipv6/nd6.c

srcs-y += ./api/api_lib.c
srcs-y += ./api/api_msg.c
srcs-y += ./api/err.c
srcs-y += ./api/if_api.c
srcs-y += ./api/netbuf.c
srcs-y += ./api/netdb.c
srcs-y += ./api/netifapi.c
srcs-y += ./api/sockets.c
srcs-y += ./api/tcpip.c

srcs-y += ./netif/ethernet.c
srcs-y += ./netif/bridgeif.c
srcs-y += ./netif/bridgeif_fdb.c
#srcs-y += ./netif/slipif.c

#srcs-y += ./netif/lowpan6_common.c
#srcs-y += ./netif/lowpan6.c
#srcs-y += ./netif/lowpan6_ble.c
#srcs-y += ./netif/zepif.c

srcs-y += ./netif/ppp/auth.c
srcs-y += ./netif/ppp/ccp.c
srcs-y += ./netif/ppp/chap-md5.c
srcs-y += ./netif/ppp/chap_ms.c
srcs-y += ./netif/ppp/chap-new.c
srcs-y += ./netif/ppp/demand.c
srcs-y += ./netif/ppp/eap.c
srcs-y += ./netif/ppp/ecp.c
srcs-y += ./netif/ppp/eui64.c
srcs-y += ./netif/ppp/fsm.c
srcs-y += ./netif/ppp/ipcp.c
srcs-y += ./netif/ppp/ipv6cp.c
srcs-y += ./netif/ppp/lcp.c
srcs-y += ./netif/ppp/magic.c
srcs-y += ./netif/ppp/mppe.c
srcs-y += ./netif/ppp/multilink.c
srcs-y += ./netif/ppp/ppp.c
srcs-y += ./netif/ppp/pppapi.c
srcs-y += ./netif/ppp/pppcrypt.c
srcs-y += ./netif/ppp/pppoe.c
srcs-y += ./netif/ppp/pppol2tp.c
srcs-y += ./netif/ppp/pppos.c
srcs-y += ./netif/ppp/upap.c
srcs-y += ./netif/ppp/utils.c
srcs-y += ./netif/ppp/vj.c
srcs-y += ./netif/ppp/polarssl/arc4.c
srcs-y += ./netif/ppp/polarssl/des.c
srcs-y += ./netif/ppp/polarssl/md4.c
srcs-y += ./netif/ppp/polarssl/md5.c
srcs-y += ./netif/ppp/polarssl/sha1.c


#srcs-y += ./apps/snmp/snmp_asn1.c
#srcs-y += ./apps/snmp/snmp_core.c
#srcs-y += ./apps/snmp/snmp_mib2.c
#srcs-y += ./apps/snmp/snmp_mib2_icmp.c
#srcs-y += ./apps/snmp/snmp_mib2_interfaces.c
#srcs-y += ./apps/snmp/snmp_mib2_ip.c
#srcs-y += ./apps/snmp/snmp_mib2_snmp.c
#srcs-y += ./apps/snmp/snmp_mib2_system.c
#srcs-y += ./apps/snmp/snmp_mib2_tcp.c
#srcs-y += ./apps/snmp/snmp_mib2_udp.c
#srcs-y += ./apps/snmp/snmp_snmpv2_framework.c
#srcs-y += ./apps/snmp/snmp_snmpv2_usm.c
#srcs-y += ./apps/snmp/snmp_msg.c
#srcs-y += ./apps/snmp/snmpv3.c
#srcs-y += ./apps/snmp/snmp_netconn.c
#srcs-y += ./apps/snmp/snmp_pbuf_stream.c
#srcs-y += ./apps/snmp/snmp_raw.c
#srcs-y += ./apps/snmp/snmp_scalar.c
#srcs-y += ./apps/snmp/snmp_table.c
#srcs-y += ./apps/snmp/snmp_threadsync.c
#srcs-y += ./apps/snmp/snmp_traps.c

#srcs-y += ./apps/http/altcp_proxyconnect.c
#srcs-y += ./apps/http/fs.c
#srcs-y += ./apps/http/http_client.c
#srcs-y += ./apps/http/httpd.c

#srcs-y += ./apps/http/makefsdata/makefsdata.c

#srcs-y += ./apps/lwiperf/lwiperf.c

#srcs-y += ./apps/smtp/smtp.c

#srcs-y += ./apps/sntp/sntp.c

#srcs-y += ./apps/mdns/mdns.c
#srcs-y += ./apps/mdns/mdns_out.c
#srcs-y += ./apps/mdns/mdns_domain.c

#srcs-y += ./apps/tftp/tftp.c

#srcs-y += ./apps/mqtt/mqtt.c

#srcs-y += ./apps/altcp_tls/altcp_tls_mbedtls.c
#srcs-y += ./apps/altcp_tls/altcp_tls_mbedtls_mem.c
#srcs-y += ./apps/snmp/snmpv3_mbedtls.c

# To remove a certain compiler flag, add a line like this
#cflags-template_ta.c-y += -Wno-strict-prototypes
