/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (c) 2022, Linaro Ltd
 */
#ifndef __SOC_QCOM_PMIC_GLINK_H__
#define __SOC_QCOM_PMIC_GLINK_H__

//for s3 5g bringup
#include <linux/types.h>

struct device;
//
struct pmic_glink;
struct pmic_glink_client;

#define PMIC_GLINK_OWNER_BATTMGR	32778
#define PMIC_GLINK_OWNER_USBC		32779
#define PMIC_GLINK_OWNER_USBC_PAN	32780

#define PMIC_GLINK_REQ_RESP		1
#define PMIC_GLINK_NOTIFY		2

//for s3 5g bringup
enum pmic_glink_state {
	PMIC_GLINK_STATE_DOWN,
	PMIC_GLINK_STATE_UP,
};

/**
 * struct pmic_glink_client_data - pmic_glink client data
 * @name:	Client name
 * @id:		Unique id for client for communication
 * @priv:	private data for client
 * @msg_cb:	callback function for client to receive the messages that
 *		are intended to be delivered to it over PMIC Glink
 * @state_cb:	callback function to notify pmic glink state in the event of
 *		a subsystem restart (SSR) or a protection domain restart (PDR)
 */
struct pmic_glink_client_data {
	const char	*name;
	u32		id;
	void		*priv;
	int		(*msg_cb)(void *priv, void *data, size_t len);
	void		(*state_cb)(void *priv, enum pmic_glink_state state);
};
//

struct pmic_glink_hdr {
	__le32 owner;
	__le32 type;
	__le32 opcode;
};

//for s3 5g bringup
#if IS_ENABLED(CONFIG_QTI_PMIC_GLINK)
struct pmic_glink_client *pmic_glink_register_client(struct device *dev,
			const struct pmic_glink_client_data *client_data);
int pmic_glink_unregister_client(struct pmic_glink_client *client);
int pmic_glink_write(struct pmic_glink_client *client, void *data,
			size_t len);
#else
static inline struct pmic_glink_client *pmic_glink_register_client(
			struct device *dev,
			const struct pmic_glink_client_data *client_data)
{
	return ERR_PTR(-ENODEV);
}

static inline int pmic_glink_unregister_client(struct pmic_glink_client *client)
{
	return -ENODEV;
}

static inline int pmic_glink_write(struct pmic_glink_client *client, void *data,
				size_t len)
{
	return -ENODEV;
}
#endif
//

int pmic_glink_send(struct pmic_glink_client *client, void *data, size_t len);

struct pmic_glink_client *devm_pmic_glink_register_client(struct device *dev,
							  unsigned int id,
							  void (*cb)(const void *, size_t, void *),
							  void (*pdr)(void *, int),
							  void *priv);

#endif
