deps_config := \
	/home/alex/esp32/esp-idf/components/app_trace/Kconfig \
	/home/alex/esp32/esp-idf/components/aws_iot/Kconfig \
	/home/alex/esp32/esp-idf/components/bt/Kconfig \
	/home/alex/esp32/esp-idf/components/driver/Kconfig \
	/home/alex/esp32/esp-idf/components/esp32/Kconfig \
	/home/alex/esp32/esp-idf/components/esp_adc_cal/Kconfig \
	/home/alex/esp32/esp-idf/components/esp_http_client/Kconfig \
	/home/alex/esp32/esp-idf/components/ethernet/Kconfig \
	/home/alex/esp32/esp-idf/components/fatfs/Kconfig \
	/home/alex/esp32/esp-idf/components/freertos/Kconfig \
	/home/alex/esp32/esp-idf/components/heap/Kconfig \
	/home/alex/esp32/esp-idf/components/http_server/Kconfig \
	/home/alex/esp32/esp-idf/components/libsodium/Kconfig \
	/home/alex/esp32/esp-idf/components/log/Kconfig \
	/home/alex/esp32/esp-idf/components/lwip/Kconfig \
	/home/alex/esp32/esp-idf/components/mbedtls/Kconfig \
	/home/alex/esp32/esp-idf/components/mdns/Kconfig \
	/home/alex/esp32/esp-idf/components/openssl/Kconfig \
	/home/alex/esp32/esp-idf/components/pthread/Kconfig \
	/home/alex/esp32/esp-idf/components/spi_flash/Kconfig \
	/home/alex/esp32/esp-idf/components/spiffs/Kconfig \
	/home/alex/esp32/esp-idf/components/tcpip_adapter/Kconfig \
	/home/alex/esp32/esp-idf/components/vfs/Kconfig \
	/home/alex/esp32/esp-idf/components/wear_levelling/Kconfig \
	/home/alex/esp32/esp-idf/examples/my_proyects/Blynk_test/components/arduino/Kconfig.projbuild \
	/home/alex/esp32/esp-idf/components/bootloader/Kconfig.projbuild \
	/home/alex/esp32/esp-idf/components/esptool_py/Kconfig.projbuild \
	/home/alex/esp32/esp-idf/components/partition_table/Kconfig.projbuild \
	/home/alex/esp32/esp-idf/Kconfig

include/config/auto.conf: \
	$(deps_config)

ifneq "$(IDF_CMAKE)" "n"
include/config/auto.conf: FORCE
endif

$(deps_config): ;
