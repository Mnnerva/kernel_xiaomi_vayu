/*
 * Copyright (C) 2010 - 2018 Novatek, Inc.
 *
 * $Revision: 32206 $
 * $Date: 2018-08-10 19:23:04 +0800 (週五, 10 八月 2018) $
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 */
#ifndef 	_LINUX_NVT_TOUCH_H
#define		_LINUX_NVT_TOUCH_H

#include <linux/delay.h>
#include <linux/input.h>
#include <linux/of.h>
#include <linux/spi/spi.h>
#include <linux/uaccess.h>
#include <linux/platform_device.h>
#include <linux/sysfs.h>
#include <linux/workqueue.h>
#include <linux/pm_qos.h>


#ifdef CONFIG_HAS_EARLYSUSPEND
#include <linux/earlysuspend.h>
#endif

#include "nt36xxx_mem_map.h"

#ifdef CONFIG_MTK_SPI
#include "mtk_spi.h"
#endif

#ifdef CONFIG_SPI_MT65XX
#include <linux/platform_data/spi-mt65xx.h>
#endif

#define FW_HISTORY_SIZE	128
/*Lock down info size*/
#define NVT_LOCKDOWN_SIZE			8

#define PINCTRL_STATE_ACTIVE		"pmx_ts_active"
#define PINCTRL_STATE_SUSPEND		"pmx_ts_suspend"
#define PINCTRL_STATE_RELEASE		"pmx_ts_release"

#define TOUCH_DISABLE_LPM 1
#define TOUCH_IRQ_BOOST 2

/* ---GPIO number--- */
#define NVTTOUCH_RST_PIN 980
#define NVTTOUCH_INT_PIN 943


/* ---INT trigger mode--- */
/* #define IRQ_TYPE_EDGE_RISING 1
#define IRQ_TYPE_EDGE_FALLING 2 */
#define INT_TRIGGER_TYPE (IRQ_TYPE_EDGE_RISING)


/* ---SPI driver info.--- */
#define NVT_SPI_NAME "NVT-ts-spi"
#define NVT_LOG(fmt, args...)	pr_info("[%s] %s %d: " fmt, NVT_SPI_NAME, __func__, __LINE__, ##args)
#define NVT_ERR(fmt, args...)	pr_err("[%s] %s %d: " fmt, NVT_SPI_NAME, __func__, __LINE__, ##args)

/* ---Input device info.--- */
#define NVT_TS_NAME "NVTCapacitiveTouchScreen"


/* ---Touch info.--- */
#define TOUCH_DEFAULT_MAX_WIDTH 1080
#define TOUCH_DEFAULT_MAX_HEIGHT 2400
#define TOUCH_MAX_FINGER_NUM 10
#define TOUCH_KEY_NUM 0
#define TOUCH_FORCE_NUM 1000

/* Enable only when module have tp reset pin and connected to host */
#define NVT_TOUCH_SUPPORT_HW_RST 1

/* ---Customerized func.--- */
#define NVT_TOUCH_PROC 1
#define NVT_TOUCH_EXT_PROC 1
#define MT_PROTOCOL_B 1
#define WAKEUP_GESTURE 1
#define FUNCPAGE_PALM 4
#define PACKET_PALM_ON 3
#define PACKET_PALM_OFF 4

#define BOOT_UPDATE_FIRMWARE 1
#define DEFAULT_BOOT_UPDATE_FIRMWARE_FIRST "j20s_novatek_ts_fw01_V13.0.3.0.bin"
#define DEFAULT_BOOT_UPDATE_FIRMWARE_SECOND "j20s_novatek_ts_fw02_V13.0.3.0.bin"
#define DEFAULT_DEBUG_FW_NAME "novatek_debug_fw.bin"
#define DEFAULT_DEBUG_MP_NAME "novatek_debug_mp.bin"

#define NVT_TOUCH_WDT_RECOVERY 1
#define NVT_TOUCH_ESD_DISP_RECOVERY 1

struct nvt_config_info {
	u8 tp_vendor;
	u8 tp_color;
	u8 display_maker;
	u8 glass_vendor;
	const char *nvt_fw_name;
	const char *nvt_limit_name;
};

enum nvt_ic_state {
	NVT_IC_SUSPEND_IN,
	NVT_IC_SUSPEND_OUT,
	NVT_IC_RESUME_IN,
	NVT_IC_RESUME_OUT,
	NVT_IC_INIT,
};

struct nvt_ts_data {
	struct spi_device *client;
	struct platform_device *pdev;
	struct input_dev *input_dev;
	struct delayed_work nvt_fwu_work;
	struct delayed_work nvt_lockdown_work;
	struct work_struct switch_mode_work;
	uint16_t addr;
	int8_t phys[32];

#ifdef CONFIG_DRM
	struct notifier_block drm_notif;
#else
	struct notifier_block fb_notif;
#endif
#if defined(CONFIG_HAS_EARLYSUSPEND)
	struct early_suspend early_suspend;
#endif

	uint8_t fw_ver;
	uint8_t x_num;
	uint8_t y_num;
	uint16_t abs_x_max;
	uint16_t abs_y_max;
	uint8_t max_touch_num;
	uint8_t max_button_num;
	uint32_t int_trigger_type;
	int32_t irq_gpio;
	uint32_t irq_flags;
	int32_t reset_gpio;
	uint32_t reset_flags;
	struct mutex lock;
	const struct nvt_ts_mem_map *mmap;
	uint8_t carrier_system;
	uint8_t hw_crc;
	uint16_t nvt_pid;
	uint8_t rbuf[1025];
	uint8_t *xbuf;
	struct mutex xbuf_lock;
	bool irq_enabled;
#ifdef CONFIG_MTK_SPI
	struct mt_chip_conf spi_ctrl;
#endif
#ifdef CONFIG_SPI_MT65XX
    struct mtk_chip_config spi_ctrl;
#endif
	struct pinctrl *ts_pinctrl;
	struct pinctrl_state *pinctrl_state_active;
	struct pinctrl_state *pinctrl_state_suspend;
	int db_wakeup;
	bool lkdown_readed;
	u8 lockdown_info[NVT_LOCKDOWN_SIZE];
	uint32_t config_array_size;
	struct nvt_config_info *config_array;
	int panel_index;
	const u8 *fw_name;
	uint32_t spi_max_freq;
	struct attribute_group *attrs;
	/*bit map indicate which slot(0~9) has been used*/
	unsigned long slot_map[BITS_TO_LONGS(10)];
	bool fw_debug;
#ifdef CONFIG_TOUCHSCREEN_NVT_DEBUG_FS
	struct dentry *debugfs;
#endif
	struct workqueue_struct *event_wq;
	struct work_struct suspend_work;
	struct work_struct resume_work;
	int result_type;
	int ic_state;
	int gesture_command_delayed;
	bool dev_pm_suspend;
	struct completion dev_pm_suspend_completion;
	bool palm_sensor_switch;
	uint8_t debug_flag;
	struct pm_qos_request pm_qos_req;
};

#if NVT_TOUCH_PROC
struct nvt_flash_data{
	rwlock_t lock;
};
#endif

typedef enum {
	RESET_STATE_INIT = 0xA0,/* IC reset */
	RESET_STATE_REK,		/* ReK baseline */
	RESET_STATE_REK_FINISH,	/* baseline is ready */
	RESET_STATE_NORMAL_RUN,	/* normal run */
	RESET_STATE_MAX  = 0xAF
} RST_COMPLETE_STATE;

typedef enum {
	EVENT_MAP_HOST_CMD						= 0x50,
	EVENT_MAP_HANDSHAKING_or_SUB_CMD_BYTE	= 0x51,
	EVENT_MAP_RESET_COMPLETE				= 0x60,
	EVENT_MAP_FWINFO						= 0x78,
	EVENT_MAP_PROJECTID						= 0x9A,
} SPI_EVENT_MAP;

//---SPI READ/WRITE---
#define SPI_WRITE_MASK(a)	(a | 0x80)
#define SPI_READ_MASK(a)	(a & 0x7F)

#define DUMMY_BYTES (1)
#define NVT_TRANSFER_LEN	(63*1024)

typedef enum {
	NVTWRITE = 0,
	NVTREAD  = 1
} NVT_SPI_RW;

//---extern structures---
extern struct nvt_ts_data *ts;

//---extern functions---

int32_t CTP_SPI_READ(struct spi_device *client, uint8_t *buf, uint16_t len);
int32_t CTP_SPI_WRITE(struct spi_device *client, uint8_t *buf, uint16_t len);
void nvt_bootloader_reset(void);
void nvt_eng_reset(void);
void nvt_sw_reset(void);
void nvt_sw_reset_idle(void);
void nvt_boot_ready(void);
void nvt_bld_crc_enable(void);
void nvt_fw_crc_enable(void);
int32_t nvt_update_firmware(const char *firmware_name);
int32_t nvt_check_fw_reset_state(RST_COMPLETE_STATE check_reset_state);
int32_t nvt_get_fw_info(void);
int32_t nvt_clear_fw_status(void);
int32_t nvt_check_fw_status(void);
int32_t nvt_set_page(uint32_t addr);
int32_t nvt_write_addr(uint32_t addr, uint8_t data);
void nvt_set_dbgfw_status(bool enable);
bool nvt_get_dbgfw_status(void);
void nvt_match_fw(void);
int32_t nvt_set_pocket_palm_switch(uint8_t pocket_palm_switch);
#endif /* _LINUX_NVT_TOUCH_H */
