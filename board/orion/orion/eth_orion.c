// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright 2018-2021 NXP
 *
 */

//#include <common.h>
#include <asm/io.h>
#include <command.h>
#include <fm_eth.h>
#include <fsl_mdio.h>
#include <fsl-mc/fsl_mc.h>
#include <fsl-mc/ldpaa_wriop.h>
#include <netdev.h>
#include <asm/arch/clock.h>
#include <fdt_support.h>

DECLARE_GLOBAL_DATA_PTR;

int board_eth_init(struct bd_info *bis)
{
	return pci_eth_init(bis);
}

#if defined(CONFIG_RESET_PHY_R)
void reset_phy(void)
{
#if defined(CONFIG_FSL_MC_ENET)
	mc_env_boot();
#endif
}
#endif /* CONFIG_RESET_PHY_R */

#ifndef CONFIG_CMD_TLV_EEPROM
int mac_read_from_eeprom(void)
{
	return 0;
}
#endif

int do_mac(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])
{
	puts("Not implemented.\n");
	return CMD_RET_FAILURE;
}

static void dpmac_set_phymode(void *fdt, unsigned int id, const char *mode) {
	char path[34] = {};
	int node;

	snprintf(path, sizeof(path), "/fsl-mc@80c000000/dpmacs/dpmac@%x", id);
	node = fdt_path_offset(fdt, path);
	fdt_delprop(fdt, node, "phy-mode");
	do_fixup_by_path_string(fdt, path, "phy-connection-type", mode);
	do_fixup_by_path_string(fdt, path, "status", "okay");
}

/*
 * Fixup dpmac phy-modes by serdes protocol to fix ethernet driver probe
 */
void board_fix_fdt_eth(void *fdt) {
	struct ccsr_gur *gur = (void *)(CFG_SYS_FSL_GUTS_ADDR);
	u32 is_lx2162 = get_svr() & 0x800;
	u32 srds_s1, srds_s2;

	srds_s1 = in_le32(&gur->rcwsr[28]) &
			  FSL_CHASSIS3_RCWSR28_SRDS1_PRTCL_MASK;
	srds_s1 >>= FSL_CHASSIS3_RCWSR28_SRDS1_PRTCL_SHIFT;

	srds_s2 = in_le32(&gur->rcwsr[28]) &
			  FSL_CHASSIS3_RCWSR28_SRDS2_PRTCL_MASK;
	srds_s2 >>= FSL_CHASSIS3_RCWSR28_SRDS2_PRTCL_SHIFT;

	/* allocate space in case properties must be added */
	fdt_increase_size(fdt, 256);

	switch (srds_s1) {
	case 0:
	case 1:
		break;
	case 2:
		/* 3, 4, 5, 6 = sgmii */
		dpmac_set_phymode(fdt, 3, "sgmii");
		dpmac_set_phymode(fdt, 4, "sgmii");
		dpmac_set_phymode(fdt, 5, "sgmii");
		dpmac_set_phymode(fdt, 6, "sgmii");
		break;
	case 3:
		/* 3, 4, 5, 6 = xgmii */
		dpmac_set_phymode(fdt, 3, "xgmii");
		dpmac_set_phymode(fdt, 4, "xgmii");
		dpmac_set_phymode(fdt, 5, "xgmii");
		dpmac_set_phymode(fdt, 6, "xgmii");
		break;
	case 4:
		/* 3, 4, 5, 6, 7, 8, 9, 10 = sgmii */
		dpmac_set_phymode(fdt, 3, "sgmii");
		dpmac_set_phymode(fdt, 4, "sgmii");
		dpmac_set_phymode(fdt, 5, "sgmii");
		dpmac_set_phymode(fdt, 6, "sgmii");
		if (is_lx2162)
			break;
		dpmac_set_phymode(fdt, 7, "sgmii");
		dpmac_set_phymode(fdt, 8, "sgmii");
		dpmac_set_phymode(fdt, 9, "sgmii");
		dpmac_set_phymode(fdt, 10, "sgmii");
		break;
	case 5:
		/* 7, 8, 9, 10 = xgmii */
		if (is_lx2162)
			break;
		dpmac_set_phymode(fdt, 7, "xgmii");
		dpmac_set_phymode(fdt, 8, "xgmii");
		dpmac_set_phymode(fdt, 9, "xgmii");
		dpmac_set_phymode(fdt, 10, "xgmii");
		break;
	case 6:
		/* 3, 4 = xgmii; 5, 6, 7, 8, 9, 10 = sgmii */
		dpmac_set_phymode(fdt, 3, "xgmii");
		dpmac_set_phymode(fdt, 4, "xgmii");
		dpmac_set_phymode(fdt, 5, "sgmii");
		dpmac_set_phymode(fdt, 6, "sgmii");
		if (is_lx2162)
			break;
		dpmac_set_phymode(fdt, 7, "sgmii");
		dpmac_set_phymode(fdt, 8, "sgmii");
		dpmac_set_phymode(fdt, 9, "sgmii");
		dpmac_set_phymode(fdt, 10, "sgmii");
		break;
	case 7:
		/* 3, 4, 5, 6 = xgmii; 7, 8, 9, 10 = sgmii */
		dpmac_set_phymode(fdt, 3, "xgmii");
		dpmac_set_phymode(fdt, 4, "xgmii");
		dpmac_set_phymode(fdt, 5, "xgmii");
		dpmac_set_phymode(fdt, 6, "xgmii");
		if (is_lx2162)
			break;
		dpmac_set_phymode(fdt, 7, "sgmii");
		dpmac_set_phymode(fdt, 8, "sgmii");
		dpmac_set_phymode(fdt, 9, "sgmii");
		dpmac_set_phymode(fdt, 10, "sgmii");
		break;
	case 8:
		/* 3, 4, 5, 6, 7, 8, 9, 10 = xgmii */
		dpmac_set_phymode(fdt, 3, "xgmii");
		dpmac_set_phymode(fdt, 4, "xgmii");
		dpmac_set_phymode(fdt, 5, "xgmii");
		dpmac_set_phymode(fdt, 6, "xgmii");
		if (is_lx2162)
			break;
		dpmac_set_phymode(fdt, 7, "xgmii");
		dpmac_set_phymode(fdt, 8, "xgmii");
		dpmac_set_phymode(fdt, 9, "xgmii");
		dpmac_set_phymode(fdt, 10, "xgmii");
		break;
	case 9:
		/* 4, 5, 6, 8, 9, 10 = sgmii */
		dpmac_set_phymode(fdt, 4, "sgmii");
		dpmac_set_phymode(fdt, 5, "sgmii");
		dpmac_set_phymode(fdt, 6, "sgmii");
		if (is_lx2162)
			break;
		dpmac_set_phymode(fdt, 8, "sgmii");
		dpmac_set_phymode(fdt, 9, "sgmii");
		dpmac_set_phymode(fdt, 10, "sgmii");
		break;
	case 10:
		/* 4, 5, 6, 8, 9, 10 = xgmii */
		dpmac_set_phymode(fdt, 4, "xgmii");
		dpmac_set_phymode(fdt, 5, "xgmii");
		dpmac_set_phymode(fdt, 6, "xgmii");
		if (is_lx2162)
			break;
		dpmac_set_phymode(fdt, 8, "xgmii");
		dpmac_set_phymode(fdt, 9, "xgmii");
		dpmac_set_phymode(fdt, 10, "xgmii");
		break;
	case 11:
		/* 5, 6, 9, 10 = sgmii */
		dpmac_set_phymode(fdt, 5, "sgmii");
		dpmac_set_phymode(fdt, 6, "sgmii");
		if (is_lx2162)
			break;
		dpmac_set_phymode(fdt, 9, "sgmii");
		dpmac_set_phymode(fdt, 10, "sgmii");
		break;
	case 12:
		/* 9, 10 = sgmii */
		if (is_lx2162)
			break;
		dpmac_set_phymode(fdt, 9, "sgmii");
		dpmac_set_phymode(fdt, 10, "sgmii");
		break;
	case 13:
		/* 1, 2 = caui4 */
		dpmac_set_phymode(fdt, 1, "caui4");
		if (is_lx2162)
			break;
		dpmac_set_phymode(fdt, 2, "caui4");
		break;
	case 14:
		/* 1 = caui4 */
		dpmac_set_phymode(fdt, 1, "caui4");
		break;
	case 15:
		/* 1, 2 = caui2 */
		dpmac_set_phymode(fdt, 1, "caui2");
		dpmac_set_phymode(fdt, 2, "caui2");
		break;
	case 16:
		/* 1 = caui2; 5, 6 = 25g-aui */
		dpmac_set_phymode(fdt, 1, "caui2");
		dpmac_set_phymode(fdt, 5, "25g-aui");
		dpmac_set_phymode(fdt, 6, "25g-aui");
		break;
	case 17:
		/* 3, 4, 5, 6 = 25g-aui */
		dpmac_set_phymode(fdt, 3, "25g-aui");
		dpmac_set_phymode(fdt, 4, "25g-aui");
		dpmac_set_phymode(fdt, 5, "25g-aui");
		dpmac_set_phymode(fdt, 6, "25g-aui");
		break;
	case 18:
		/* 3, 4, 7, 8, 9, 10 = xgmii; 5, 6 = 25g-aui */
		dpmac_set_phymode(fdt, 3, "xgmii");
		dpmac_set_phymode(fdt, 4, "xgmii");
		dpmac_set_phymode(fdt, 5, "25g-aui");
		dpmac_set_phymode(fdt, 6, "25g-aui");
		if (is_lx2162)
			break;
		dpmac_set_phymode(fdt, 7, "xgmii");
		dpmac_set_phymode(fdt, 8, "xgmii");
		dpmac_set_phymode(fdt, 9, "xgmii");
		dpmac_set_phymode(fdt, 10, "xgmii");
		break;
	case 19:
		/* 2 = xlaui4; 3, 4 = xgmii; 5, 6 = 25g-aui */
		dpmac_set_phymode(fdt, 3, "xgmii");
		dpmac_set_phymode(fdt, 4, "xgmii");
		dpmac_set_phymode(fdt, 5, "25g-aui");
		dpmac_set_phymode(fdt, 6, "25g-aui");
		if (is_lx2162)
			break;
		dpmac_set_phymode(fdt, 2, "xlaui4");
		break;
	case 20:
		/* 1, 2 = xlaui4 */
		dpmac_set_phymode(fdt, 1, "xlaui4");
		if (is_lx2162)
			break;
		dpmac_set_phymode(fdt, 2, "xlaui4");
		break;
	case 21:
		/* 3, 4, 5, 6, 9, 10 = 25g-aui */
		dpmac_set_phymode(fdt, 3, "25g-aui");
		dpmac_set_phymode(fdt, 4, "25g-aui");
		dpmac_set_phymode(fdt, 5, "25g-aui");
		dpmac_set_phymode(fdt, 6, "25g-aui");
		if (is_lx2162)
			break;
		dpmac_set_phymode(fdt, 9, "25g-aui");
		dpmac_set_phymode(fdt, 10, "25g-aui");
		break;
	case 22:
		/* 3, 4, 5, 6, 9, 10 = xgmii */
		dpmac_set_phymode(fdt, 3, "xgmii");
		dpmac_set_phymode(fdt, 4, "xgmii");
		dpmac_set_phymode(fdt, 5, "xgmii");
		dpmac_set_phymode(fdt, 6, "xgmii");
		if (is_lx2162)
			break;
		dpmac_set_phymode(fdt, 9, "xgmii");
		dpmac_set_phymode(fdt, 10, "xgmii");
		break;
	}

	switch (srds_s2) {
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
		break;
	case 6:
		/* 13, 14 = xgmii; 15, 16 = sgmii */
		dpmac_set_phymode(fdt, 13, "xgmii");
		dpmac_set_phymode(fdt, 14, "xgmii");
		dpmac_set_phymode(fdt, 15, "sgmii");
		dpmac_set_phymode(fdt, 16, "sgmii");
		break;
	case 7:
		/* 12, 16, 17, 18 = sgmii; 13, 14 = xgmii */
		dpmac_set_phymode(fdt, 12, "sgmii");
		dpmac_set_phymode(fdt, 13, "xgmii");
		dpmac_set_phymode(fdt, 14, "xgmii");
		dpmac_set_phymode(fdt, 16, "sgmii");
		dpmac_set_phymode(fdt, 17, "sgmii");
		dpmac_set_phymode(fdt, 18, "sgmii");
		break;
	case 8:
		/* 13, 14 = xgmii */
		dpmac_set_phymode(fdt, 13, "xgmii");
		dpmac_set_phymode(fdt, 14, "xgmii");
		break;
	case 9:
		/* 11, 12, 13, 14, 15, 16, 17, 18 = sgmii */
		dpmac_set_phymode(fdt, 11, "sgmii");
		dpmac_set_phymode(fdt, 12, "sgmii");
		dpmac_set_phymode(fdt, 13, "sgmii");
		dpmac_set_phymode(fdt, 14, "sgmii");
		dpmac_set_phymode(fdt, 15, "sgmii");
		dpmac_set_phymode(fdt, 16, "sgmii");
		dpmac_set_phymode(fdt, 17, "sgmii");
		dpmac_set_phymode(fdt, 18, "sgmii");
		break;
	case 10:
		/* 11, 12, 17, 18 = sgmii */
		dpmac_set_phymode(fdt, 11, "sgmii");
		dpmac_set_phymode(fdt, 12, "sgmii");
		dpmac_set_phymode(fdt, 17, "sgmii");
		dpmac_set_phymode(fdt, 18, "sgmii");
		break;
	case 11:
		/* 12, 13, 14, 16, 17, 18 = sgmii */
		dpmac_set_phymode(fdt, 12, "sgmii");
		dpmac_set_phymode(fdt, 13, "sgmii");
		dpmac_set_phymode(fdt, 14, "sgmii");
		dpmac_set_phymode(fdt, 16, "sgmii");
		dpmac_set_phymode(fdt, 17, "sgmii");
		dpmac_set_phymode(fdt, 18, "sgmii");
		break;
	case 12:
		/* 11, 12, 17, 18 = sgmii */
		dpmac_set_phymode(fdt, 11, "sgmii");
		dpmac_set_phymode(fdt, 12, "sgmii");
		dpmac_set_phymode(fdt, 17, "sgmii");
		dpmac_set_phymode(fdt, 18, "sgmii");
		break;
	case 13:
		/* 13, 14 = sgmii */
		dpmac_set_phymode(fdt, 13, "sgmii");
		dpmac_set_phymode(fdt, 14, "sgmii");
		break;
	case 14:
		/* 13, 14, 17, 18 = sgmii */
		dpmac_set_phymode(fdt, 13, "sgmii");
		dpmac_set_phymode(fdt, 14, "sgmii");
		dpmac_set_phymode(fdt, 17, "sgmii");
		dpmac_set_phymode(fdt, 18, "sgmii");
		break;
	}
}
