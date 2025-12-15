/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _UFS_FLASTOR_HID_H_
#define _UFS_FLASTOR_HID_H_

#include <asm/unaligned.h>
#include <linux/interrupt.h>
#include <linux/sysfs.h>
#include <linux/blktrace_api.h>
#include <linux/blkdev.h>
#include <linux/bitfield.h>
#include <linux/delay.h>
#include <scsi/scsi_cmnd.h>
#include <ufs/ufs.h>
#include <ufs/ufshcd.h>

#include "../../../block/blk.h"

#define UFS_UPIU_MAX_GENERAL_LUN_FLASTOR		32

#define UFS_VENDOR_FLASTOR		0xF86

#define UFSHID_VER_FLASTOR		0x0100

#define UFSHID_SELECTOR		0x00

#define UFS_FEATURE_SUPPORT_HID_BIT_F_310	(1 << 10)
#define UFS_FEATURE_SUPPORT_HID_BIT_F_220	(1 << 12)

#define HID_TRIGGER_WORKER_DELAY_MS_DEFAULT	2000
#define HID_TRIGGER_WORKER_DELAY_MS_MIN		100
#define HID_TRIGGER_WORKER_DELAY_MS_MAX		10000

#define HID_FRAG_LEVEL_MASK		0xF

#define WAIT_HID_RESUME_TIMEOUT			(2 * HZ)

#define QUERY_FLAG_IDN_HID_ENABLE			0x13

/* Description */
#define UFSF_QUERY_DESC_DEVICE_MAX_SIZE		0x65

/* Device descriptor parameters offsets in bytes*/
#define DEVICE_DESC_PARAM_EX_FEAT_SUP			0x4F

/* Attribute idn for Flastor */
#define QUERY_ATTR_IDN_HID_OPERATION_FLASTOR		0x14 /* For UFS2.2 */
#define QUERY_ATTR_IDN_HID_FRAG_LEVEL_FLASTOR		0x34
#define QUERY_ATTR_IDN_HID_STATE_FLASTOR		0x35
#define QUERY_ATTR_IDN_HID_FEAT_SUP_FLASTOR		0x4F

#define INFO_MSG(msg, args...)		pr_info("%s:%d info: " msg "\n", \
					       __func__, __LINE__, ##args)
#define ERR_MSG(msg, args...)		pr_err("%s:%d err: " msg "\n", \
					       __func__, __LINE__, ##args)
#define WARN_MSG(msg, args...)		pr_warn("%s:%d warn: " msg "\n", \
					       __func__, __LINE__, ##args)
#undef HID_DEBUG
#define HID_DEBUG(hid, msg, args...)					\
	do { if (hid->hid_debug)					\
		pr_err("%40s:%3d [%01d%02d%02d] " msg "\n",		\
		       __func__, __LINE__,				\
		       hid->hid_trigger,				\
		       atomic_read(&hid->hid_info->hba->dev->power.usage_count),\
		       hid->hid_info->hba->clk_gating.active_reqs, ##args);	\
	} while (0)

enum UFSHID_DEV_STATE_FLASTOR {
	HID_IDLE_FLASTOR		= 0x0,
	HID_EXECUTING_FLASTOR		= 0x1,
	HID_INTERRUPTED_FLASTOR 	= 0x2,
	HID_DONE_FLASTOR		= 0x3,
	HID_NUM_DEV_STATES_FLASTOR	= 0x4,
};

struct ufshid_dev;

struct ufshid_flastor_info {
	struct ufs_hba *hba;
	struct scsi_device *sdev_ufs_lu[UFS_UPIU_MAX_GENERAL_LUN];
	bool check_init;
	struct work_struct device_check_work;

	struct work_struct reset_wait_work;
	struct work_struct resume_work;
	atomic_t hid_state;
	struct ufshid_flastor_dev *hid_dev;
};

struct ufshid_flastor_dev {
	struct ufshid_flastor_info *hid_info;

	unsigned int hid_trigger;   /* default value is false */
	struct delayed_work hid_trigger_work;
	unsigned int hid_trigger_delay;

	u32 ahit;			/* to restore ahit value */
	bool is_auto_enabled;

	/* for sysfs */
	struct kobject kobj;
	struct mutex sysfs_lock;
	struct ufshid_flastor_sysfs_entry *sysfs_entries;

	bool hid_enable;
	/* for debug */
	bool hid_debug;
#if defined(CONFIG_UFSHID_FLASTOR_POC)
	bool block_suspend;
#endif
	struct completion resume_compl;
};

struct ufshid_flastor_sysfs_entry {
	struct attribute attr;
	ssize_t (*show)(struct ufshid_flastor_dev *hid, char *buf);
	ssize_t (*store)(struct ufshid_flastor_dev *hid, const char *buf, size_t count);
};

void ufsf_fhid_remove(struct ufshid_flastor_info *hid_info);
void ufsf_fhid_reset_host(struct ufshid_flastor_info *hid_info);
void ufsf_fhid_set_init_state(struct ufs_hba *hba);
void ufsf_fhid_suspend(struct ufshid_flastor_info *hid_info, bool is_system_pm);
#endif /* End of Header */
