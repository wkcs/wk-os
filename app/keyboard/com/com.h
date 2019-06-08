#ifndef __COM_COM_H__
#define __COM_COM_H__

#include <wk/kernel.h>

enum com_cmd_type {
    COM_ACK_PKG = 0,
    COM_HEART_PKG,
    COM_SET_CONFIG_PKG,
    COM_GET_CONFIG_PKG,
    COM_GET_VERSION_PKG,
    COM_DOWNLOAD_PKG,
};

struct com_cmd_package {
    uint8_t type;
#define DATA0    0
#define DATA1    1
#define DATA2    2
#define DATA_ERR 0xff
    uint8_t data_type;
    uint8_t index;
#define PKG_END     0xff
#define PKG_NOT_END 0X00
    uint8_t end;
    uint32_t val;
} __attribute__ ((packed));

void com_read_data_all_wait(uint8_t *buf, size_t size);
int com_read_data(uint8_t *buf, size_t size);
int com_write_data(const uint8_t *buf, size_t size);
int com_send_ack(uint32_t val, uint8_t end);
int com_wait_ack(uint32_t *size);
int com_download_img(struct com_cmd_package *cmd_pkg);
int com_get_version(void);

extern struct device *flash_dev;

#endif