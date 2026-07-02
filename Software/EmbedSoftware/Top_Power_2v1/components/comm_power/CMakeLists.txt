set(CURRENT_IDF_VERSION "${IDF_VERSION_MAJOR}.${IDF_VERSION_MINOR}.${IDF_VERSION_PATCH}")
#message(STATUS "Current ESP-IDF version: ${CURRENT_IDF_VERSION}")

file(GLOB_RECURSE COMM_POWER_SRC src/*.c)

set(COMM_POWER_INC . inc )

if("${CURRENT_IDF_VERSION}" VERSION_LESS  "5.3") #ESP-IDF version < 5.3
    set(REQ driver )
    set(PRIVREQ )
else() #ESP-IDF version >= 5.3
    set(REQ esp_driver_gpio esp_driver_uart)
    set(PRIVREQ )
endif()

set(REQ ${REQ} lvgl guiguider)

idf_component_register(SRCS ${COMM_POWER_SRC}
                    INCLUDE_DIRS ${COMM_POWER_INC}
                    REQUIRES ${REQ}
                    PRIV_REQUIRES ${PRIVREQ})


