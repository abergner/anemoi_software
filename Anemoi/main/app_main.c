

#include "rom/cache.h"
#include "nvs_flash.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "esp_log.h"
#include "lwip/sockets.h"
//#include "lwip/dns.h"
//include "lwip/netdb.h"
//#include "lwip/igmp.h"
//#include "esp_wifi.h"
#include "soc/rtc_cntl_reg.h"
#include "esp_event.h"

#include "esp_event_loop.h"
#include "esp_intr_alloc.h"

#include "esp_err.h"


#include "include/anemoi_app.h"

//Main application
void app_main()
{    
	app();
    
}
