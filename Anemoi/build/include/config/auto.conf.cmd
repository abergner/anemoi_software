deps_config := \
	/c/Users/cufar/eclipse-workspace/esp-bm/esp-idf/components/app_trace/Kconfig \
	/c/Users/cufar/eclipse-workspace/esp-bm/esp-idf/components/aws_iot/Kconfig \
	/c/Users/cufar/eclipse-workspace/esp-bm/esp-idf/components/bt/Kconfig \
	/c/Users/cufar/eclipse-workspace/esp-bm/esp-idf/components/driver/Kconfig \
	/c/Users/cufar/eclipse-workspace/esp-bm/esp-idf/components/esp32/Kconfig \
	/c/Users/cufar/eclipse-workspace/esp-bm/esp-idf/components/esp_adc_cal/Kconfig \
	/c/Users/cufar/eclipse-workspace/esp-bm/esp-idf/components/esp_event/Kconfig \
	/c/Users/cufar/eclipse-workspace/esp-bm/esp-idf/components/esp_http_client/Kconfig \
	/c/Users/cufar/eclipse-workspace/esp-bm/esp-idf/components/esp_http_server/Kconfig \
	/c/Users/cufar/eclipse-workspace/esp-bm/esp-idf/components/ethernet/Kconfig \
	/c/Users/cufar/eclipse-workspace/esp-bm/esp-idf/components/fatfs/Kconfig \
	/c/Users/cufar/eclipse-workspace/esp-bm/esp-idf/components/freemodbus/Kconfig \
	/c/Users/cufar/eclipse-workspace/esp-bm/esp-idf/components/freertos/Kconfig \
	/c/Users/cufar/eclipse-workspace/esp-bm/esp-idf/components/heap/Kconfig \
	/c/Users/cufar/eclipse-workspace/esp-bm/esp-idf/components/libsodium/Kconfig \
	/c/Users/cufar/eclipse-workspace/esp-bm/esp-idf/components/log/Kconfig \
	/c/Users/cufar/eclipse-workspace/esp-bm/esp-idf/components/lwip/Kconfig \
	/c/Users/cufar/eclipse-workspace/esp-bm/esp-idf/components/mbedtls/Kconfig \
	/c/Users/cufar/eclipse-workspace/esp-bm/esp-idf/components/mdns/Kconfig \
	/c/Users/cufar/eclipse-workspace/esp-bm/esp-idf/components/mqtt/Kconfig \
	/c/Users/cufar/eclipse-workspace/esp-bm/esp-idf/components/nvs_flash/Kconfig \
	/c/Users/cufar/eclipse-workspace/esp-bm/esp-idf/components/openssl/Kconfig \
	/c/Users/cufar/eclipse-workspace/esp-bm/esp-idf/components/pthread/Kconfig \
	/c/Users/cufar/eclipse-workspace/esp-bm/esp-idf/components/spi_flash/Kconfig \
	/c/Users/cufar/eclipse-workspace/esp-bm/esp-idf/components/spiffs/Kconfig \
	/c/Users/cufar/eclipse-workspace/esp-bm/esp-idf/components/tcpip_adapter/Kconfig \
	/c/Users/cufar/eclipse-workspace/esp-bm/esp-idf/components/unity/Kconfig \
	/c/Users/cufar/eclipse-workspace/esp-bm/esp-idf/components/vfs/Kconfig \
	/c/Users/cufar/eclipse-workspace/esp-bm/esp-idf/components/wear_levelling/Kconfig \
	/c/Users/cufar/eclipse-workspace/esp-bm/esp-idf/components/app_update/Kconfig.projbuild \
	/c/Users/cufar/eclipse-workspace/esp-bm/esp-idf/components/bootloader/Kconfig.projbuild \
	/c/Users/cufar/eclipse-workspace/esp-bm/esp-idf/components/esptool_py/Kconfig.projbuild \
	/c/Users/cufar/eclipse-workspace/esp-bm/esp-idf/components/partition_table/Kconfig.projbuild \
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
