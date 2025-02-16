/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright 2018,2020 NXP
 */

#ifndef __LX2_RDB_H
#define __LX2_RDB_H

#include "lx2160a_common.h"

#define CONFIG_GPIO_EXTRA_HEADER

#define SEMIHOSTING_MC_INIT_CMD				\
	"load hostfs - 0x80a00000 mc.itb;"	\
	"load hostfs - 0x80e00000 dpc.dtb;"	\
	"env exists secureboot && "		\
	"mmc read 0x80640000 0x3200 0x20 && "	\
	"mmc read 0x80680000 0x3400 0x20 && "	\
	"esbc_validate 0x80640000 && "		\
	"esbc_validate 0x80680000 ;"		\
	"fsl_mc start mc 0x80a00000 0x80e00000\0"

/* Initial environment variables */
#define CFG_EXTRA_ENV_SETTINGS		\
	EXTRA_ENV_SETTINGS			\
	"boot_scripts=orion_boot.scr\0"	\
	"boot_script_hdr=orion_bs.out\0"	\
	"BOARD=orion\0"			\
	"sd2_bootcmd=echo Trying load from emmc card..;"	\
		"mmc dev 1; mmcinfo; mmc read $load_addr "	\
		"$kernel_addr_sd $kernel_size_sd ;"		\
		"env exists secureboot && mmc read $kernelheader_addr_r "\
		"$kernelhdr_addr_sd $kernelhdr_size_sd "	\
		" && esbc_validate ${kernelheader_addr_r};"	\
		"bootm $load_addr#$BOARD\0" \
	"semihost_bootcmd=echo Trying load from semihosting..;"	\
		"load hostfs - $load_addr kernel.bin;" \
		"bootm $load_addr#$BOARD\0"
#include <asm/fsl_secure_boot.h>

#endif /* __LX2_RDB_H */
