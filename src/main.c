#include <hardware/gpio.h>
#include <hardware/watchdog.h>
#include <password.h>
#include <pico/bootrom.h>
#include <pico/cyw43_arch.h>
#include <pico_usb_console/protocol.h>
#include <pico_usb_console/serial.h>



static uint8_t mac_address[6];



uint16_t pbuf_copy_partial(void* buf,void* dataptr,uint16_t len,uint16_t offset){
	return 0;
}



void cyw43_cb_process_ethernet(void* cb_data,int itf,size_t len,const uint8_t* buf){
	pico_usb_console_protocol_send_log(0,"Packet");
}



void cyw43_cb_tcpip_set_link_up(cyw43_t* self,int itf){
	pico_usb_console_protocol_send_log(0,"Link UP");
}



void cyw43_cb_tcpip_set_link_down(cyw43_t* self,int itf){
	pico_usb_console_protocol_send_log(0,"Link DOWN");
}



static int _scan_callback(void* ctx,const cyw43_ev_scan_result_t *result){
	if (result){
		pico_usb_console_protocol_send_log(0,"%s: rssi=-%u, chan=%u, mac=%x:%x:%x:%x:%x:%x, sec=%u",result->ssid,-result->rssi,result->channel,result->bssid[0],result->bssid[1],result->bssid[2],result->bssid[3],result->bssid[4],result->bssid[5],result->auth_mode);
	}
	return 0;
}



static void _input_callback(unsigned char length,const char* data){
	if (length==1&&*data=='s'){
		if (!cyw43_wifi_scan_active(&cyw43_state)){
			cyw43_wifi_scan_options_t scan_options={0};
			cyw43_wifi_scan(&cyw43_state,&scan_options,NULL,_scan_callback);
		}
	}
	else if (length==1&&*data=='c'){
		pico_usb_console_protocol_send_log(0,"%s (%x:%x:%x:%x:%x:%x)",(cyw43_wifi_link_status(&cyw43_state,CYW43_ITF_STA)==CYW43_LINK_JOIN?"connected":"not connected"),mac_address[0],mac_address[1],mac_address[2],mac_address[3],mac_address[4],mac_address[5]);
		uint8_t buffer[18]={
			0xff,0xff,0xff,0xff,0xff,0xff,
			mac_address[0],mac_address[1],mac_address[2],mac_address[3],mac_address[4],mac_address[5],
			'?','?',
			'D','a','t','a'
		};
		cyw43_send_ethernet(&cyw43_state,CYW43_ITF_STA,sizeof(buffer),buffer,0);
	}
	else if (length==1&&*data=='d'){
		cyw43_arch_wifi_connect_async(WIFI_SSID,WIFI_PASSWORD,CYW43_AUTH_WPA2_AES_PSK);
	}
	else if (length==1&&*data=='q'){
		reset_usb_boot(0,0);
	}
}



int main(void){
	cyw43_arch_init_with_country(CYW43_COUNTRY_LUXEMBOURG);
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN,1);
    cyw43_arch_enable_sta_mode();
	cyw43_wifi_pm(&cyw43_state,CYW43_PERFORMANCE_PM);
    cyw43_hal_get_mac(0,mac_address);
	pico_usb_console_init();
	pico_usb_console_protocol_set_input_callback(_input_callback);
	// watchdog_enable(500,0);
	while (1){
		pico_usb_console_update();
		pico_usb_console_protocol_update();
		cyw43_poll();
		// watchdog_update();
	}
	reset_usb_boot(0,0);
}
