add_executable(duifene_sign_sample
    ${CMAKE_CURRENT_SOURCE_DIR}/sample.cpp
)

target_include_directories(duifene_sign_sample PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(duifene_sign_sample PRIVATE
    duifene_sign
)


add_executable(duifene_sign_c_sample
    ${CMAKE_CURRENT_SOURCE_DIR}/sample_c.cpp
)

target_include_directories(duifene_sign_c_sample PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(duifene_sign_c_sample PRIVATE
    duifene_sign_c
)