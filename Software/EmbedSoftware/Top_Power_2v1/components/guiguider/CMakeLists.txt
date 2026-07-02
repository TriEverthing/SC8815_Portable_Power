file(GLOB_RECURSE GUIGUIDER_SRC generated/*.c custom/*.c )

set(GUIGUIDER_INC .  ./generated ./custom)

idf_component_register(SRCS ${GUIGUIDER_SRC}
                    INCLUDE_DIRS ${GUIGUIDER_INC}
                    REQUIRES lvgl)
