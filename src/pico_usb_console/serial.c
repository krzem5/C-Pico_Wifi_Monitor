#include <pico_usb_console/serial.h>
#include <tusb.h>



unsigned char _pico_usb_console_read_buffer[(PACKET_BUFFER_MASK+1)<<1];
pico_usb_console_data_length_t _pico_usb_console_read_buffer_start=0;
pico_usb_console_data_length_t _pico_usb_console_read_buffer_end=0;



void pico_usb_console_init(void){
	tusb_init();
}



void pico_usb_console_update(void){
	tud_task();
}



void pico_usb_console_read(void){
	pico_usb_console_data_length_t space=(_pico_usb_console_read_buffer_start-_pico_usb_console_read_buffer_end-1)&PACKET_BUFFER_MASK;
	if (space&&tud_cdc_connected()){
		pico_usb_console_data_length_t count=tud_cdc_available();
		if (count>space){
			count=space;
		}
		count=tud_cdc_read(_pico_usb_console_read_buffer+_pico_usb_console_read_buffer_end,count);
		_pico_usb_console_read_buffer_end+=count;
		for (pico_usb_console_data_length_t i=PACKET_BUFFER_MASK+1;i<_pico_usb_console_read_buffer_end;i++){
			_pico_usb_console_read_buffer[i-PACKET_BUFFER_MASK-1]=_pico_usb_console_read_buffer[i];
		}
		_pico_usb_console_read_buffer_end&=PACKET_BUFFER_MASK;
	}
}



void pico_usb_console_write(const unsigned char* data,pico_usb_console_data_length_t length){
	while (tud_cdc_connected()&&length){
		pico_usb_console_data_length_t count=tud_cdc_write_available();
		if (count>length){
			count=length;
		}
		if (count){
			count=tud_cdc_write(data,count);
			data+=count;
			length-=count;
		}
		tud_task();
		tud_cdc_write_flush();
	}
}
