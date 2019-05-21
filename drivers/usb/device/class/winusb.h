/*
 * Copyright (C) 2019 胡启航<Hu Qihang>
 *
 * Author: wkcs
 * 
 * Email: hqh2030@gmail.com, huqihan@live.com
 */

#ifndef _USBDEVICE_CLASS_WINUSB_H_
#define _USBDEVICE_CLASS_WINUSB_H_

#define WINUSB_GUID "6860DC3C-C05F-4807-8807-1CA861CC1D66"

struct winusb_descriptor
{
#ifdef USB_DEVICE_COMPOSITE
    struct uiad_descriptor iad_desc;
#endif
    struct uinterface_descriptor intf_desc;
    struct uendpoint_descriptor ep_out_desc;
    struct uendpoint_descriptor ep_in_desc;
};
typedef struct winusb_descriptor* winusb_desc_t;

#endif
