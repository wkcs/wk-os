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
    COM_DATA_PKG,
};

struct com_cmd_package {
    uint8_t type;
#define DATA0    0
#define DATA1    1
#define DATA2    2
#define DATA_ERR 0xff
    uint8_t data_type;
    uint32_t index;
    uint32_t size;
    uint8_t buf[0];
} __attribute__ ((packed));

void com_read_data_all_wait(uint8_t *buf, size_t size);
int com_read_data(uint8_t *buf, size_t size);
int com_write_data(const uint8_t *buf, size_t size);
int com_send_ack(bool err);
int com_wait_ack(void);
int com_download_img(uint8_t *buf, uint32_t size, uint32_t index);
int com_get_version(void);

extern struct device *flash_dev;

#endif