set(CURRENT_IDF_VERSION "${IDF_VERSION_MAJOR}.${IDF_VERSION_MINOR}.${IDF_VERSION_PATCH}")
#message(STATUS "Current ESP-IDF version: ${CURRENT_IDF_VERSION}")

if("${CURRENT_IDF_VERSION}" VERSION_LESS  "5.3") #ESP-IDF version < 5.3
    set(REQ driver)
    set(PRIVREQ )
else() #ESP-IDF version >= 5.3
    set(REQ esp_driver_gpio esp_driver_spi)
    set(PRIVREQ )
endif()

set(REQ ${REQ} esp_timer comm_power lvgl_keypad wifi lvgl_esp32_drivers lvgl guiguider)
    
idf_component_register(SRCS "top_power_main.c"
                    INCLUDE_DIRS "."
                    REQUIRES ${REQ}
                    PRIV_REQUIRES ${PRIVREQ})