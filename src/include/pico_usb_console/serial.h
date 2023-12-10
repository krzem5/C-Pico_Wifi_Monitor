#ifndef _PICO_USB_CONSOLE_SERIAL_H_
#define _PICO_USB_CONSOLE_SERIAL_H_ 1



#define PACKET_BUFFER_MASK 255



typedef unsigned short int pico_usb_console_data_length_t;



extern unsigned char _pico_usb_console_read_buffer[(PACKET_BUFFER_MASK+1)<<1];
extern pico_usb_console_data_length_t _pico_usb_console_read_buffer_start;
extern pico_usb_console_data_length_t _pico_usb_console_read_buffer_end;



static inline pico_usb_console_data_length_t pico_usb_console_data_length(void){
	return (_pico_usb_console_read_buffer_end-_pico_usb_console_read_buffer_start+PACKET_BUFFER_MASK+1)&PACKET_BUFFER_MASK;
}



static inline unsigned char pico_usb_console_get_data(pico_usb_console_data_length_t offset){
	return _pico_usb_console_read_buffer[(offset+_pico_usb_console_read_buffer_start)&PACKET_BUFFER_MASK];
}



static inline void pico_usb_console_consume_data(pico_usb_console_data_length_t count){
	_pico_usb_console_read_buffer_start=(_pico_usb_console_read_buffer_start+count)&PACKET_BUFFER_MASK;
}



void pico_usb_console_init(void);



void pico_usb_console_update(void);



void pico_usb_console_read(void);



void pico_usb_console_write(const unsigned char* data,pico_usb_console_data_length_t length);



#endif
