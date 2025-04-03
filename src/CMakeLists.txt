add_library(duifene_sign SHARED
    ${CMAKE_SOURCE_DIR}/src/duifene_sign.cpp
)

add_library(duifene_sign_c SHARED
    ${CMAKE_SOURCE_DIR}/src/duifene_sign_c.cpp
)

target_include_directories(duifene_sign PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

target_include_directories(duifene_sign_c PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

find_package(cpr CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(LibXml2 REQUIRED)

target_link_libraries(duifene_sign PRIVATE
        cpr::cpr
        nlohmann_json::nlohmann_json
        LibXml2::LibXml2
)

target_link_libraries(duifene_sign_c PRIVATE
        duifene_sign
)
