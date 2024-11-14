cmake_minimum_required(VERSION 3.0.0)
project(RMASmoke VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()
add_library(libtss2-sys STATIC IMPORTED)
set_target_properties(libtss2-sys PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/libtss2-sys.a )

add_library(libtss2-esys STATIC IMPORTED)
set_target_properties(libtss2-esys PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/libtss2-esys.a)

add_library(libtss2-mu STATIC IMPORTED)
set_target_properties(libtss2-mu PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/libtss2-mu.a)

add_library(libprotobufstatic STATIC IMPORTED)
set_target_properties(libprotobufstatic PROPERTIES IMPORTED_LOCATION /usr/lib/libprotobuf.a)


add_library(libsslstatic STATIC IMPORTED)
set_target_properties(libsslstatic PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/libssl.a)

add_library(libcryptostatic STATIC IMPORTED)
set_target_properties(libcryptostatic PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/libcrypto.a)
add_executable(rmasmoke src/main.cpp gen/src/tpm_manager.pb.cc )

target_include_directories(rmasmoke SYSTEM AFTER PUBLIC gen/)
add_compile_options( -Wl,--copy-dt-needed-entries -static -static-libstdc++ )
target_link_libraries(rmasmoke libtss2-sys libtss2-mu libsslstatic libcryptostatic libtss2-esys   libprotobufstatic -static-libgcc -static-libstdc++ )
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
