if(NOT  CONFIG_REGION_AS923)
set( MAC_EXCLUDE_BUILD_SOURCES
    ${MAC_EXCLUDE_BUILD_SOURCES}
    "mac/region/RegionAS923.c" )
endif()

if(NOT  CONFIG_REGION_AU915)
set( MAC_EXCLUDE_BUILD_SOURCES
     ${MAC_EXCLUDE_BUILD_SOURCES}
     "mac/region/RegionAU915.c" )
endif()

if(NOT  CONFIG_REGION_CN470)
set( MAC_EXCLUDE_BUILD_SOURCES
     ${MAC_EXCLUDE_BUILD_SOURCES}
     "mac/region/RegionCN470.c"
     "mac/region/RegionCN470A20.c"
     "mac/region/RegionCN470B20.c"
     "mac/region/RegionCN470A26.c"
     "mac/region/RegionCN470B26.c" )
endif()

if(NOT  CONFIG_REGION_CN779)
set( MAC_EXCLUDE_BUILD_SOURCES
    ${MAC_EXCLUDE_BUILD_SOURCES}
    "mac/region/RegionCN779.c" )
endif()

if(NOT  CONFIG_REGION_EU433)
set( MAC_EXCLUDE_BUILD_SOURCES
    ${MAC_EXCLUDE_BUILD_SOURCES}
    "mac/region/RegionEU433.c" )
endif()

if(NOT CONFIG_REGION_EU868)
set( MAC_EXCLUDE_BUILD_SOURCES
    ${MAC_EXCLUDE_BUILD_SOURCES}
    "mac/region/RegionEU868.c" )
endif()

if(NOT  CONFIG_REGION_IN865)
set( MAC_EXCLUDE_BUILD_SOURCES
    ${MAC_EXCLUDE_BUILD_SOURCES}
    "mac/region/RegionIN865.c" )
endif()

if(NOT  CONFIG_REGION_KR920)
set( MAC_EXCLUDE_BUILD_SOURCES
    ${MAC_EXCLUDE_BUILD_SOURCES}
    "mac/region/RegionKR920.c" )
endif()

if(NOT  CONFIG_REGION_RU864)
set( MAC_EXCLUDE_BUILD_SOURCES
    ${MAC_EXCLUDE_BUILD_SOURCES}
    "mac/region/RegionRU864.c" )
endif()

if( NOT CONFIG_REGION_US915)
set( MAC_EXCLUDE_BUILD_SOURCES
     ${MAC_EXCLUDE_BUILD_SOURCES}
    #  "mac/region/RegionBaseUS.c"
     "mac/region/RegionUS915.c" )
endif()

MESSAGE( STATUS "MAC_EXCLUDE_BUILD_SOURCES: ${MAC_EXCLUDE_BUILD_SOURCES}")

idf_component_register(
    SRC_DIRS
        "common"
        "common/LmHandler"
        "common/LmHandler/packages"
        "mac"
        "mac/region"
        "soft-se"
        "utilities"
        "adapter"
    EXCLUDE_SRCS 
        "${MAC_EXCLUDE_BUILD_SOURCES}"
    INCLUDE_DIRS
        "common"
        "common/LmHandler"
        "common/LmHandler/packages"
        "mac"
        "mac/region"
        "soft-se"
        "utilities"
        "adapter"
    REQUIRES
        lora)

target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
