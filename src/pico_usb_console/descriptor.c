#include <stdint.h>
#include <tusb.h>



#define USB_DESCRIPTOR_VID 0xfff0
#define USB_DESCRIPTOR_PID 0x0001

#define USB_DESCRIPTOR_MAX_POWER 250

#define USB_DESCRIPTOR_INTERFACE_CDC 0
#define USB_DESCRIPTOR_INTERFACE_MAX 1

#define USB_DESCRIPTOR_CDC_EP_CMD 0x81
#define USB_DESCRIPTOR_CDC_EP_OUT 0x02
#define USB_DESCRIPTOR_CDC_EP_IN 0x82
#define USB_DESCRIPTOR_CDC_CMD_MAX_SIZE 8
#define USB_DESCRIPTOR_CDC_IN_OUT_MAX_SIZE 64

#define USB_DESCRIPTOR_STRING_EMPTY 0x01
#define USB_DESCRIPTOR_STRING_PRODUCT 0x02
#define USB_DESCRIPTOR_STRING_MANUFACTURER 0x03



static const tusb_desc_device_t usb_descriptor_device={
	.bLength=sizeof(tusb_desc_device_t),
	.bDescriptorType=TUSB_DESC_DEVICE,
	.bcdUSB=0x0200,
	.bDeviceClass=TUSB_CLASS_MISC,
	.bDeviceSubClass=MISC_SUBCLASS_COMMON,
	.bDeviceProtocol=MISC_PROTOCOL_IAD,
	.bMaxPacketSize0=CFG_TUD_ENDPOINT0_SIZE,
	.idVendor=USB_DESCRIPTOR_VID,
	.idProduct=USB_DESCRIPTOR_PID,
	.bcdDevice=0x0100,
	.iManufacturer=USB_DESCRIPTOR_STRING_MANUFACTURER,
	.iProduct=USB_DESCRIPTOR_STRING_PRODUCT,
	.bNumConfigurations=1,
};



static const uint8_t usb_descriptor_config[TUD_CONFIG_DESC_LEN+TUD_CDC_DESC_LEN]={
	TUD_CONFIG_DESCRIPTOR(1,USB_DESCRIPTOR_INTERFACE_MAX,USB_DESCRIPTOR_STRING_EMPTY,TUD_CONFIG_DESC_LEN+TUD_CDC_DESC_LEN,0,USB_DESCRIPTOR_MAX_POWER),
	TUD_CDC_DESCRIPTOR(USB_DESCRIPTOR_INTERFACE_CDC,USB_DESCRIPTOR_STRING_EMPTY,USB_DESCRIPTOR_CDC_EP_CMD,USB_DESCRIPTOR_CDC_CMD_MAX_SIZE,USB_DESCRIPTOR_CDC_EP_OUT,USB_DESCRIPTOR_CDC_EP_IN,USB_DESCRIPTOR_CDC_IN_OUT_MAX_SIZE)
};



static uint16_t _usb_descriptor_string_buffer[256];



const uint8_t* tud_descriptor_device_cb(void){
	return (const uint8_t*)&usb_descriptor_device;
}



const uint8_t* tud_descriptor_configuration_cb(uint8_t index){
	return usb_descriptor_config;
}



const uint16_t* tud_descriptor_string_cb(uint8_t index,uint16_t langid){
	if (!index){
		static const uint16_t out[2]={(TUSB_DESC_STRING<<8)|4,0x0409};
		return out;
	}
	const char* string="";
	switch (index){
		case USB_DESCRIPTOR_STRING_EMPTY:
			string="N/A";
			break;
		case USB_DESCRIPTOR_STRING_PRODUCT:
			string="Pi Pico USB Console";
			break;
		case USB_DESCRIPTOR_STRING_MANUFACTURER:
			string="krzem5";
			break;
	}
	uint8_t length=1;
	while (*string){
		_usb_descriptor_string_buffer[length]=*string;
		length++;
		string++;
	}
	_usb_descriptor_string_buffer[0]=(TUSB_DESC_STRING<<8)|(length<<1);
	return _usb_descriptor_string_buffer;
}
