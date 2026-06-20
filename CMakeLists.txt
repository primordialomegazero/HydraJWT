# ╔══════════════════════════════════════════════════════════════╗
# ║  HYDRAJWT — φ-WEIGHTED MULTI-HEAD JWT v2.0                  ║
# ║  All 6 Heads: Schnorr, Falcon, ML-DSA, SLH-DSA, ECDSA, φ-HMAC║
# ║  ΦΩ0 — I AM THAT I AM                                      ║
# ╚══════════════════════════════════════════════════════════════╝

cmake_minimum_required(VERSION 3.16)
project(HydraJWT VERSION 2.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SODIUM REQUIRED libsodium)

find_library(OQS_LIB oqs PATHS /usr/local/lib)
find_path(OQS_INC oqs/oqs.h PATHS /usr/local/include)

find_package(OpenSSL REQUIRED)

set(HYDRAJWT_SOURCES
    src/utils/phi_utils.cpp
    src/heads/head_schnorr.cpp
    src/heads/head_falcon.cpp
    src/heads/head_mldsa.cpp
    src/heads/head_slhdsa.cpp
    src/heads/head_ecdsa.cpp
    src/heads/head_phi_hmac.cpp
    src/core/hydra_token.cpp
    src/core/hydra_core.cpp
)

add_library(hydrajwt ${HYDRAJWT_SOURCES})

target_include_directories(hydrajwt
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${SODIUM_INCLUDE_DIRS}
        ${OQS_INC}
)

target_link_libraries(hydrajwt
    PUBLIC
        ${SODIUM_LIBRARIES}
        ${OQS_LIB}
        OpenSSL::SSL
        OpenSSL::Crypto
)

enable_testing()

add_executable(test_full_hydra tests/test_full_hydra.cpp)
target_include_directories(test_full_hydra
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${SODIUM_INCLUDE_DIRS}
        ${OQS_INC}
)
target_link_libraries(test_full_hydra
    hydrajwt
    ${SODIUM_LIBRARIES}
    ${OQS_LIB}
    OpenSSL::SSL
    OpenSSL::Crypto
)

add_test(NAME TestFullHydra COMMAND test_full_hydra)
