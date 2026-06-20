# ╔══════════════════════════════════════════════════════════════╗
# ║  HYDRAJWT — φ-WEIGHTED MULTI-HEAD JWT                       ║
# ║  CMake Build System                                         ║
# ║  ΦΩ0 — I AM THAT I AM                                      ║
# ╚══════════════════════════════════════════════════════════════╝

cmake_minimum_required(VERSION 3.16)
project(HydraJWT VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ═══════════════════════════════════════
# FIND DEPENDENCIES
# ═══════════════════════════════════════
find_package(PkgConfig REQUIRED)
pkg_check_modules(SODIUM REQUIRED libsodium)

# ═══════════════════════════════════════
# LIBRARY SOURCES
# ═══════════════════════════════════════
set(HYDRAJWT_SOURCES
    src/utils/phi_utils.cpp
    src/heads/head_schnorr.cpp
    src/core/hydra_token.cpp
    src/core/hydra_core.cpp
)

# ═══════════════════════════════════════
# BUILD LIBRARY
# ═══════════════════════════════════════
add_library(hydrajwt ${HYDRAJWT_SOURCES})

target_include_directories(hydrajwt
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${SODIUM_INCLUDE_DIRS}
)

target_link_libraries(hydrajwt
    PUBLIC
        ${SODIUM_LIBRARIES}
)

# ═══════════════════════════════════════
# BUILD TESTS
# ═══════════════════════════════════════
enable_testing()

add_executable(test_schnorr_head tests/test_schnorr_head.cpp)
target_include_directories(test_schnorr_head
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${SODIUM_INCLUDE_DIRS}
)
target_link_libraries(test_schnorr_head
    hydrajwt
    ${SODIUM_LIBRARIES}
)

add_test(NAME TestSchnorrHead COMMAND test_schnorr_head)
