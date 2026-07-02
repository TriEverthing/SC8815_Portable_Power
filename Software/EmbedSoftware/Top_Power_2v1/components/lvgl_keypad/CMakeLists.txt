set(CURRENT_IDF_VERSION "${IDF_VERSION_MAJOR}.${IDF_VERSION_MINOR}.${IDF_VERSION_PATCH}")
#message(STATUS "Current ESP-IDF version: ${CURRENT_IDF_VERSION}")

if("${CURRENT_IDF_VERSION}" VERSION_LESS  "5.3") #ESP-IDF version < 5.3
    set(REQ driver)
    set(PRIVREQ )
else() #ESP-IDF version >= 5.3
    set(REQ esp_driver_gpio )
    set(PRIVREQ )
endif()

set(REQ ${REQ} esp_pm lvgl guiguider espressif__button espressif__knob)

file(GLOB_RECURSE KEYPAD_SRC ./*.c)
set(KEYPAD_INC . )
    
idf_component_register(SRCS ${KEYPAD_SRC}
                    INCLUDE_DIRS ${KEYPAD_INC}
                    REQUIRES ${REQ}
                    PRIV_REQUIRES ${PRIVREQ})
