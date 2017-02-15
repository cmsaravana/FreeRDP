cmake_minimum_required(VERSION 2.8)

set(CMAKE_SYSTEM_NAME "Generic" CACHE STRING "Target system.")

#set (NACL_SDK_ROOT "/home/mkumara/gonacl/nacl_sdk/pepper_49")
if(NOT EXISTS "$ENV{FREERDP_NACL_SDK_ROOT}") 
  message(FATAL_ERROR "Could not find FREERDP_NACL_SDK_ROOT environment variable") 
endif(NOT EXISTS "$ENV{FREERDP_NACL_SDK_ROOT}")
#set (NACL_SDK_ROOT "/home/mkumara/gonacl/nacl_sdk/pepper_49/payload")
if(NOT EXISTS "$ENV{FREERDP_NACL_OPENSSL_ROOT}") 
  message(FATAL_ERROR "Could not find FREERDP_NACL_OPENSSL_ROOT environment variable") 
endif(NOT EXISTS "$ENV{FREERDP_NACL_OPENSSL_ROOT}")

# specify cross-compilers
set (NACL_OPENSSL_ROOT "$ENV{FREERDP_NACL_OPENSSL_ROOT}")
set (NACL_SDK_ROOT "$ENV{FREERDP_NACL_SDK_ROOT}")
set (PNACL_TC_PATH "${NACL_SDK_ROOT}/toolchain/linux_pnacl")
set (NACL_BIN "${PNACL_TC_PATH}/bin")

set(CMAKE_C_COMPILER "${NACL_BIN}/pnacl-clang" CACHE PATH "gcc" FORCE)
set(CMAKE_CXX_COMPILER "${NACL_BIN}/pnacl-clang++" CACHE PATH "g++" FORCE)
set(CMAKE_AR "${NACL_BIN}/pnacl-ar" CACHE PATH "archive" FORCE)
set(CMAKE_LINKER "${NACL_BIN}/pnacl-clang++" CACHE PATH "linker" FORCE)
set(CMAKE_RANLIB "${NACL_BIN}/pnacl-ranlib" CACHE PATH "ranlib" FORCE)
SET(CMAKE_REQUIRED_INCLUDES "${NACL_SDK_ROOT}/include/pnacl")

set( CMAKE_CROSSCOMPILING TRUE )
set( CMAKE_C_COMPILER_ID  "pnacl_id" CACHE PATH "CLang" FORCE)

set (CHROMEOS True)

# global includes and link directories

include_directories ("${NACL_SDK_ROOT}/include/pnacl")
include_directories ("${NACL_OPENSSL_ROOT}/include")

add_definitions(-DCHROMEOS_BUILD)
add_definitions(-DWITH_OPENSSL)
