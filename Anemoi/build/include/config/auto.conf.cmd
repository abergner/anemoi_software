deps_config := \
	C:\Users\cufar\eclipse-workspace\esp-bm\esp-idf\components\app_trace\Kconfig \
	C:\Users\cufar\eclipse-workspace\esp-bm\esp-idf\components\aws_iot\Kconfig \
	C:\Users\cufar\eclipse-workspace\esp-bm\esp-idf\components\bt\Kconfig \
	C:\Users\cufar\eclipse-workspace\esp-bm\esp-idf\components\driver\Kconfig \
	C:\Users\cufar\eclipse-workspace\esp-bm\esp-idf\components\esp32\Kconfig \
	C:\Users\cufar\eclipse-workspace\esp-bm\esp-idf\components\esp_adc_cal\Kconfig \
	C:\Users\cufar\eclipse-workspace\esp-bm\esp-idf\components\esp_event\Kconfig \
	C:\Users\cufar\eclipse-workspace\esp-bm\esp-idf\components\esp_http_client\Kconfig \
	C:\Users\cufar\eclipse-workspace\esp-bm\esp-idf\components\esp_http_server\Kconfig \
	C:\Users\cufar\eclipse-workspace\esp-bm\esp-idf\components\ethernet\Kconfig \
	C:\Users\cufar\eclipse-workspace\esp-bm\esp-idf\components\fatfs\Kconfig \
	C:\Users\cufar\eclipse-workspace\esp-bm\esp-idf\components\freemodbus\Kconfig \
	C:\Users\cufar\eclipse-workspace\esp-bm\esp-idf\components\freertos\Kconfig \
	C:\Users\cufar\eclipse-workspace\esp-bm\esp-idf\components\heap\Kconfig \
	C:\Users\cufar\eclipse-workspace\esp-bm\esp-idf\components\libsodium\Kconfig \
	C:\Users\cufar\eclipse-workspace\esp-bm\esp-idf\components\log\Kconfig \
	C:\Users\cufar\eclipse-workspace\esp-bm\esp-idf\components\lwip\Kconfig \
	C:\Users\cufar\eclipse-workspace\esp-bm\esp-idf\components\mbedtls\Kconfig \
	C:\Users\cufar\eclipse-workspace\esp-bm\esp-idf\components\mdns\Kconfig \
	C:\Users\cufar\eclipse-workspace\esp-bm\esp-idf\components\mqtt\Kconfig \
	C:\Users\cufar\eclipse-workspace\esp-bm\esp-idf\components\nvs_flash\Kconfig \
	C:\Users\cufar\eclipse-workspace\esp-bm\esp-idf\components\openssl\Kconfig \
	C:\Users\cufar\eclipse-workspace\esp-bm\esp-idf\components\pthread\Kconfig \
	C:\Users\cufar\eclipse-workspace\esp-bm\esp-idf\components\spi_flash\Kconfig \
	C:\Users\cufar\eclipse-workspace\esp-bm\esp-idf\components\spiffs\Kconfig \
	C:\Users\cufar\eclipse-workspace\esp-bm\esp-idf\components\tcpip_adapter\Kconfig \
	C:\Users\cufar\eclipse-workspace\esp-bm\esp-idf\components\unity\Kconfig \
	C:\Users\cufar\eclipse-workspace\esp-bm\esp-idf\components\vfs\Kconfig \
	C:\Users\cufar\eclipse-workspace\esp-bm\esp-idf\components\wear_levelling\Kconfig \
	C:\Users\cufar\eclipse-workspace\esp-bm\esp-idf\components\app_update\Kconfig.projbuild \
	C:\Users\cufar\eclipse-workspace\esp-bm\esp-idf\components\bootloader\Kconfig.projbuild \
	C:\Users\cufar\eclipse-workspace\esp-bm\esp-idf\components\esptool_py\Kconfig.projbuild \
	C:\Users\cufar\eclipse-workspace\esp-bm\esp-idf\components\partition_table\Kconfig.projbuild \
	/c/Users/cufar/eclipse-workspace/esp-bm/esp-idf/Kconfig

include/config/auto.conf: \
	$(deps_config)

ifneq "$(IDF_TARGET)" "esp32"
include/config/auto.conf: FORCE
endif
ifneq "$(IDF_CMAKE)" "n"
include/config/auto.conf: FORCE
endif

$(deps_config): ;
