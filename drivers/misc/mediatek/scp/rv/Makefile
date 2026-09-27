obj-$(CONFIG_MTK_TINYSYS_SCP_SUPPORT) += scp.o
scp-$(CONFIG_MTK_TINYSYS_SCP_SUPPORT) += scp_helper.o
scp-$(CONFIG_MTK_TINYSYS_SCP_SUPPORT) += scp_excep.o
scp-$(CONFIG_MTK_TINYSYS_SCP_SUPPORT) += scp_wrapper_ipi.o
scp-$(CONFIG_MTK_TINYSYS_SCP_SUPPORT) += scp_irq.o
scp-$(CONFIG_MTK_TINYSYS_SCP_SUPPORT) += scp_logger.o
scp-$(CONFIG_MTK_TINYSYS_SCP_SUPPORT) += scp_l1c.o
scp-$(CONFIG_MTK_TINYSYS_SCP_SUPPORT) += scp_scpctl.o

scp-$(CONFIG_MTK_TINYSYS_SCP_SUPPORT) += scp_feature_table.o
scp-$(CONFIG_MTK_TINYSYS_SCP_SUPPORT) += scp_awake.o
scp-$(CONFIG_MTK_TINYSYS_SCP_SUPPORT) += scp_dvfs.o
scp-$(CONFIG_MTK_TINYSYS_SCP_SUPPORT) += scp_hwvoter_dbg.o

# AGATE(5.10): -fno-pic -mcmodel=large made clang 20 emit
# R_AARCH64_MOVW_UABS_G0..G3 for every global in this directory, which
# lld rejects ("cannot be used against symbol ...; recompile with
# -fPIC") because a normal kernel link resolves them near.  The 4.19
# reference tree builds these same sources without it.
ccflags-y += -D DEBUG_DO
ccflags-y += -I$(srctree)/drivers/misc/mediatek/scp/include
ccflags-y += -I$(srctree)/drivers/misc/mediatek/include
ccflags-y += -I$(srctree)/drivers/clk/mediatek/
ccflags-y += -I$(srctree)/sound/soc/mediatek/common
