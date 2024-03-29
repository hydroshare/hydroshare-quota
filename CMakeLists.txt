cmake_minimum_required(VERSION 3.5.0 FATAL_ERROR) #CPACK_DEBIAN_<COMPONENT>_PACKAGE_NAME

find_package(IRODS 4.2.11 EXACT REQUIRED)

set(CMAKE_C_COMPILER ${IRODS_EXTERNALS_FULLPATH_CLANG}/bin/clang)
set(CMAKE_CXX_COMPILER ${IRODS_EXTERNALS_FULLPATH_CLANG}/bin/clang++)

project(irods-microservice-hydroshare C CXX)

if (NOT CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/" CACHE STRING "Install root path. Change from \"/\" for run-in-place installations." FORCE)
  message(STATUS "Setting unspecified CMAKE_INSTALL_PREFIX to '${CMAKE_INSTALL_PREFIX}'. This is the correct setting for normal builds.")
endif()

if (NOT CPACK_PACKAGING_INSTALL_PREFIX)
  set(CPACK_PACKAGING_INSTALL_PREFIX "/" CACHE STRING "Package root path. \"/\" is correct for normal package builds.." FORCE)
  message(STATUS "Setting unspecified CPACK_PACKAGING_INSTALL_PREFIX to '${CPACK_PACKAGING_INSTALL_PREFIX}'. This is the correct setting for normal builds.")
endif()


if (NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build {Debug, Release}." FORCE)
  message(STATUS "Setting unspecified CMAKE_BUILD_TYPE to '${CMAKE_BUILD_TYPE}'")
endif()

if (NOT IRODS_EXTERNALS_PACKAGE_ROOT)
  set(IRODS_EXTERNALS_PACKAGE_ROOT "/opt/irods-externals" CACHE STRING "Choose the location of iRODS external packages." FORCE)
  message(STATUS "Setting unspecified IRODS_EXTERNALS_PACKAGE_ROOT to '${IRODS_EXTERNALS_PACKAGE_ROOT}'")
endif()

macro(IRODS_MACRO_SET_AND_CHECK_DEPENDENCY_FULL_PATH DEPENDENCY_NAME DEPENDENCY_SUBDIRECTORY)
  if (NOT IS_DIRECTORY ${IRODS_EXTERNALS_PACKAGE_ROOT}/${DEPENDENCY_SUBDIRECTORY})
    message(FATAL_ERROR "${DEPENDENCY_NAME} not found at ${IRODS_EXTERNALS_PACKAGE_ROOT}/${DEPENDENCY_SUBDIRECTORY}")
  endif()
  set(IRODS_EXTERNALS_FULLPATH_${DEPENDENCY_NAME} ${IRODS_EXTERNALS_PACKAGE_ROOT}/${DEPENDENCY_SUBDIRECTORY})
endmacro()

set(CMAKE_C_COMPILER ${IRODS_EXTERNALS_FULLPATH_CLANG}/bin/clang)
set(CMAKE_CXX_COMPILER ${IRODS_EXTERNALS_FULLPATH_CLANG}/bin/clang++)

find_package(OpenSSL REQUIRED)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++")
set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -stdlib=libc++")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -stdlib=libc++")
add_compile_options(-nostdinc++)
add_compile_options(-std=c++14)
link_libraries(
    "-Wl,-rpath=${IRODS_EXTERNALS_FULLPATH_CLANG_RUNTIME}/lib"
    "-Wl,-rpath=${IRODS_EXTERNALS_FULLPATH_FMT}/lib"
    ${IRODS_EXTERNALS_FULLPATH_FMT}/lib/libfmt.so
    c++abi)
include_directories(
    ${IRODS_EXTERNALS_FULLPATH_CLANG}/include/c++/v1
    ${IRODS_EXTERNALS_FULLPATH_FMT}/include)

set(
  IRODS_AUDIT_PLUGIN_LOCKING_SERVER_COMPILE_DEFINITIONS
  RODS_SERVER
  ENABLE_RE
  )

set(
  IRODS_AUDIT_PLUGIN_LOCKING_SERVER_LINK_LIBRARIES
  irods_server
  irods_clerver
  )

install(FILES hydroshare.re DESTINATION /etc/irods/)

set(
  PLUGIN1
  msiHSAddNewFile
  )

  add_library(
      ${PLUGIN1}
      MODULE
      ${CMAKE_SOURCE_DIR}/src/lib-msiHSAddNewFile.cpp
      )

  target_include_directories(
      ${PLUGIN1}
      PRIVATE
      /usr/include/irods
      ${IRODS_EXTERNALS_FULLPATH_BOOST}/include
      ${IRODS_EXTERNALS_FULLPATH_JSON}/include
      )

  target_link_libraries(
      ${PLUGIN1}
      PRIVATE
      ${OPENSSL_CRYPTO_LIBRARY}
      curl
      )

  target_compile_definitions(${PLUGIN1} PRIVATE ${IRODS_AUDIT_PLUGIN_LOCKING_SERVER_COMPILE_DEFINITIONS} ${IRODS_COMPILE_DEFINITIONS} BOOST_SYSTEM_NO_DEPRECATED)

  target_compile_options(${PLUGIN1} PRIVATE -Wno-write-strings)

  set_property(TARGET ${PLUGIN1} PROPERTY CXX_STANDARD ${IRODS_CXX_STANDARD})

install(
  TARGETS
  ${PLUGIN1}
  LIBRARY
    DESTINATION usr/lib/irods/plugins/microservices
  )

set(
  PLUGIN2
  msiHSRemoveFile
  )

  add_library(
      ${PLUGIN2}
      MODULE
      ${CMAKE_SOURCE_DIR}/src/lib-msiHSRemoveFile.cpp
      )

  target_include_directories(
      ${PLUGIN2}
      PRIVATE
      /usr/include/irods
      ${IRODS_EXTERNALS_FULLPATH_BOOST}/include
      ${IRODS_EXTERNALS_FULLPATH_JSON}/include
      )

  target_link_libraries(
      ${PLUGIN2}
      PRIVATE
      ${OPENSSL_CRYPTO_LIBRARY}
      curl
      )

  target_compile_definitions(${PLUGIN2} PRIVATE ${IRODS_AUDIT_PLUGIN_LOCKING_SERVER_COMPILE_DEFINITIONS} ${IRODS_COMPILE_DEFINITIONS} BOOST_SYSTEM_NO_DEPRECATED)

  target_compile_options(${PLUGIN2} PRIVATE -Wno-write-strings)

  set_property(TARGET ${PLUGIN2} PROPERTY CXX_STANDARD ${IRODS_CXX_STANDARD})

install(
  TARGETS
  ${PLUGIN2}
  LIBRARY
    DESTINATION usr/lib/irods/plugins/microservices
  )

set(
  PLUGIN3
  msiHSAddQuotaHolder
  )

  add_library(
      ${PLUGIN3}
      MODULE
      ${CMAKE_SOURCE_DIR}/src/lib-msiHSAddQuotaHolder.cpp
      )

  target_include_directories(
      ${PLUGIN3}
      PRIVATE
      /usr/include/irods
      ${IRODS_EXTERNALS_FULLPATH_BOOST}/include
      ${IRODS_EXTERNALS_FULLPATH_JSON}/include
      )

  target_link_libraries(
      ${PLUGIN3}
      PRIVATE
      ${OPENSSL_CRYPTO_LIBRARY}
      curl
      )

  target_compile_definitions(${PLUGIN3} PRIVATE ${IRODS_AUDIT_PLUGIN_LOCKING_SERVER_COMPILE_DEFINITIONS} ${IRODS_COMPILE_DEFINITIONS} BOOST_SYSTEM_NO_DEPRECATED)

  target_compile_options(${PLUGIN3} PRIVATE -Wno-write-strings)

  set_property(TARGET ${PLUGIN3} PROPERTY CXX_STANDARD ${IRODS_CXX_STANDARD})

install(
  TARGETS
  ${PLUGIN3}
  LIBRARY
    DESTINATION usr/lib/irods/plugins/microservices
  )

set(
  PLUGIN4
  msiHSRemoveQuotaHolder
  )

  add_library(
      ${PLUGIN4}
      MODULE
      ${CMAKE_SOURCE_DIR}/src/lib-msiHSRemoveQuotaHolder.cpp
      )

  target_include_directories(
      ${PLUGIN4}
      PRIVATE
      /usr/include/irods
      ${IRODS_EXTERNALS_FULLPATH_BOOST}/include
      ${IRODS_EXTERNALS_FULLPATH_JSON}/include
      )

  target_link_libraries(
      ${PLUGIN4}
      PRIVATE
      ${OPENSSL_CRYPTO_LIBRARY}
      curl
      )

  target_compile_definitions(${PLUGIN4} PRIVATE ${IRODS_AUDIT_PLUGIN_LOCKING_SERVER_COMPILE_DEFINITIONS} ${IRODS_COMPILE_DEFINITIONS} BOOST_SYSTEM_NO_DEPRECATED)

  target_compile_options(${PLUGIN4} PRIVATE -Wno-write-strings)

  set_property(TARGET ${PLUGIN4} PROPERTY CXX_STANDARD ${IRODS_CXX_STANDARD})

install(
  TARGETS
  ${PLUGIN4}
  LIBRARY
    DESTINATION usr/lib/irods/plugins/microservices
  )

set(
  PLUGIN5
  msiHSResetQuotaDir
  )

  add_library(
      ${PLUGIN5}
      MODULE
      ${CMAKE_SOURCE_DIR}/src/lib-msiHSResetQuotaDir.cpp
      )

  target_include_directories(
      ${PLUGIN5}
      PRIVATE
      /usr/include/irods
      ${IRODS_EXTERNALS_FULLPATH_BOOST}/include
      ${IRODS_EXTERNALS_FULLPATH_JSON}/include
      )

  target_link_libraries(
      ${PLUGIN5}
      PRIVATE
      ${OPENSSL_CRYPTO_LIBRARY}
      curl
      )

  target_compile_definitions(${PLUGIN5} PRIVATE ${IRODS_AUDIT_PLUGIN_LOCKING_SERVER_COMPILE_DEFINITIONS} ${IRODS_COMPILE_DEFINITIONS} BOOST_SYSTEM_NO_DEPRECATED)

  target_compile_options(${PLUGIN5} PRIVATE -Wno-write-strings)

  set_property(TARGET ${PLUGIN5} PROPERTY CXX_STANDARD ${IRODS_CXX_STANDARD})

install(
  TARGETS
  ${PLUGIN5}
  LIBRARY
    DESTINATION usr/lib/irods/plugins/microservices
  )

set(CPACK_PACKAGE_FILE_NAME "hydroshare-quota-microservices-${IRODS_LINUX_DISTRIBUTION_NAME}${IRODS_LINUX_DISTRIBUTION_VERSION_MAJOR}-${CMAKE_SYSTEM_PROCESSOR}")
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
set(CPACK_COMPONENT_INCLUDE_TOPLEVEL_DIRECTORY OFF)
set(CPACK_COMPONENTS_GROUPING IGNORE)
set(CPACK_PACKAGE_VERSION "1.3.0")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "3")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_CONTACT "CUAHSI Inc. <commgr@cuahsi.org>")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "The HydroShare microservices library")
set(CPACK_PACKAGE_VENDOR  "CUAHSI Inc. <commgr@cuahsi.org>")

set(CPACK_DEB_COMPONENT_INSTALL OFF)
set(CPACK_DEBIAN_PACKAGE_SECTION "contrib/science")
set(CPACK_DEBIAN_COMPRESSION_TYPE "gzip")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "extra")
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://www.cuahsi.org")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS OFF)
set(CPACK_DEBIAN_PACKAGE_CONTROL_STRICT_PERMISSION ON)

set(CPACK_RPM_COMPONENT_INSTALL OFF)
set(CPACK_RPM_PACKAGE_RELEASE "0")
set(CPACK_RPM_PACKAGE_LICENSE "BSD-3-Clause")
set(CPACK_RPM_PACKAGE_VENDOR "CUAHSI Inc.")
set(CPACK_RPM_PACKAGE_URL "https://www.cuahsi.org")
set(CPACK_RPM_PACKAGE_AUTOREQ 0)
set(CPACK_RPM_PACKAGE_AUTOPROV 0)

set(CPACK_ARCHIVE_COMPONENT_INSTALL OFF)

set(CPACK_DEBIAN_PACKAGE_NAME "hydroshare-quota-microservices")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "${IRODS_PACKAGE_DEPENDENCIES_STRING}, irods-runtime (= ${IRODS_VERSION}), libc6")

set(CPACK_RPM_PACKAGE_NAME "hydroshare-quota-microservices")
if (IRODS_LINUX_DISTRIBUTION_NAME STREQUAL "centos" OR IRODS_LINUX_DISTRIBUTION_NAME STREQUAL "centos linux")
  set(CPACK_RPM_PACKAGE_REQUIRES "${IRODS_PACKAGE_DEPENDENCIES_STRING}, irods-runtime = ${IRODS_VERSION}, openssl")
elseif (IRODS_LINUX_DISTRIBUTION_NAME STREQUAL "opensuse")
  set(CPACK_RPM_PACKAGE_REQUIRES "${IRODS_PACKAGE_DEPENDENCIES_STRING}, irods-runtime = ${IRODS_VERSION}, libopenssl1_0_0")
endif()


if (NOT CPACK_GENERATOR)
    set(CPACK_GENERATOR ${IRODS_CPACK_GENERATOR} CACHE STRING "CPack generator to use, e.g. {DEB, RPM, TGZ}." FORCE)
    message(STATUS "Setting unspecified CPACK_GENERATOR to ${CPACK_GENERATOR}. This is the correct setting for normal builds.")
endif()

include(CPack)
