set(CURRENT_IDF_VERSION "${IDF_VERSION_MAJOR}.${IDF_VERSION_MINOR}.${IDF_VERSION_PATCH}")
#message(STATUS "Current ESP-IDF version: ${CURRENT_IDF_VERSION}")

if("${CURRENT_IDF_VERSION}" VERSION_LESS  "5.3") #ESP-IDF version < 5.3
    set(REQ driver)
    set(PRIVREQ )
else() #ESP-IDF version >= 5.3
    set(REQ esp_driver_spi esp_driver_i2c esp_driver_ledc esp_driver_gpio)
    set(PRIVREQ )
endif()

set(REQ ${REQ} esp_http_server nvs_flash esp_wifi comm_power lvgl guiguider)

file(GLOB_RECURSE WIFI_SRC ./*.c)

set(WIFI_INC . )

idf_component_register(SRCS ${WIFI_SRC}
                    INCLUDE_DIRS ${WIFI_INC}
                    REQUIRES ${REQ}  
                    PRIV_REQUIRES ${PRIVREQ}
                    EMBED_FILES "index.html" "PowerServer.html")
