obj-$(CONFIG_MTK_TINYSYS_VCP_SUPPORT) += vcp_status.o

obj-$(CONFIG_MTK_TINYSYS_VCP_SUPPORT) += vcp.o
vcp-$(CONFIG_MTK_TINYSYS_VCP_SUPPORT) += vcp_helper.o
vcp-$(CONFIG_MTK_TINYSYS_VCP_SUPPORT) += vcp_excep.o
vcp-$(CONFIG_MTK_TINYSYS_VCP_SUPPORT) += vcp_irq.o
vcp-$(CONFIG_MTK_TINYSYS_VCP_SUPPORT) += vcp_logger.o
vcp-$(CONFIG_MTK_TINYSYS_VCP_SUPPORT) += vcp_l1c.o
vcp-$(CONFIG_MTK_TINYSYS_VCP_SUPPORT) += vcp_vcpctl.o
vcp-$(CONFIG_MTK_TINYSYS_VCP_SUPPORT) += vcp_feature_table.o
vcp-$(CONFIG_MTK_TINYSYS_VCP_SUPPORT) += vcp_awake.o

# AGATE(5.10): -fno-pic -mcmodel=large made clang 20 emit
# R_AARCH64_MOVW_UABS_G0..G3 for every global in this directory, which
# lld rejects ("cannot be used against symbol ...; recompile with
# -fPIC") because a normal kernel link resolves them near.  The 4.19
# reference tree builds these same sources without it.
ccflags-y += -D DEBUG_DO
ccflags-y += -I$(srctree)/drivers/misc/mediatek/vcp/include
ccflags-y += -I$(srctree)/drivers/misc/mediatek/include
ccflags-y += -I$(srctree)/drivers/misc/mediatek/vcp/rv
ccflags-y += -I$(srctree)/drivers/misc/mediatek/include/mt-plat
ccflags-y += -I$(srctree)/drivers/clk/mediatek