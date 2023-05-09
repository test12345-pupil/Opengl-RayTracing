add_executable(Hand
        gl_env.h
        main.cpp)

find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    target_link_libraries(Hand PUBLIC OpenMP::OpenMP_CXX)
endif()

target_link_libraries(Hand PRIVATE glew_s glm stb glfw)
target_include_directories(Hand PRIVATE
        ../third_party/glew/include
        ${CMAKE_CURRENT_BINARY_DIR} )

target_compile_features(Hand PRIVATE cxx_std_11)

configure_file(config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)