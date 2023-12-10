#include <hardware/gpio.h>
#include <hardware/watchdog.h>
#include <password.h>
#include <pico/bootrom.h>
#include <pico/cyw43_arch.h>
#include <pico_usb_console/protocol.h>
#include <pico_usb_console/serial.h>



void cyw43_cb_process_ethernet(void* cb_data,int itf,size_t len,const uint8_t* buf){
	pico_usb_console_protocol_send_log(0,"Packet");
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
		pico_usb_console_protocol_send_log(0,"%u",(cyw43_wifi_link_status(&cyw43_state,CYW43_ITF_STA)==CYW43_LINK_JOIN?"connected":"not connected"));
	}
	else if (length==1&&*data=='q'){
		reset_usb_boot(0,0);
	}
}



int main(void){
	cyw43_arch_init();
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN,1);
    cyw43_arch_enable_sta_mode();
	pico_usb_console_init();
	pico_usb_console_protocol_set_input_callback(_input_callback);
	watchdog_enable(500,0);
	cyw43_arch_wifi_connect_async(WIFI_SSID,WIFI_PASSWORD,CYW43_AUTH_WPA2_AES_PSK);
	while (1){
		pico_usb_console_update();
		pico_usb_console_protocol_update();
		cyw43_arch_poll();
		watchdog_update();
	}
	reset_usb_boot(0,0);
}
