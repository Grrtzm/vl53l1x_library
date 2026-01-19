idf_component_register(
    SRCS
        "src/vl53l1x_ulp_esp.c"
        "src/vl53l1x_ulp_platform_esp.c"
        "st_uld/VL53L1X_ULP_api.c"

    INCLUDE_DIRS
        "include"
        "st_uld"

    REQUIRES
        driver
        freertos
        log
)
