#include <wk/err.h>
#include <version.h>
#include "com.h"

int com_get_version(void)
{
    uint8_t buf[3] = {VERSION, PATCHLEVEL, SUBLEVEL};
    int ret;

    ret = com_send_ack(3, PKG_END);
    if (ret != 0)
        goto err;
    pr_info("%s: send ack ok\r\n", __func__);
    ret = com_write_data(buf, 3);
    if (ret != 0)
        goto err;
    pr_info("%s: send data ok\r\n", __func__);
    ret = com_wait_ack(NULL);
    if (ret != 0)
        goto err;
    pr_info("%s: wait ack ok\r\n", __func__);

    return 0;
err:
    pr_err("get version err, ret = %d\r\n", ret);
    return ret;
}