CMAKE_MINIMUM_REQUIRED(VERSION 3.0)
PROJECT("Table Maker for Modern C++")

INCLUDE(cmake/ConfigureTarget.cmake)

INCLUDE(cmake/FindYamlCpp.cmake)
IF (YAMLCPP_FOUND)
  INCLUDE_DIRECTORIES(${YAMLCPP_INCLUDE_DIR})
  LINK_LIBRARIES(${YAMLCPP_LIBRARY})
ENDIF ()

INCLUDE(cmake/FindJsonCpp.cmake)
IF (JSONCPP_FOUND)
  INCLUDE_DIRECTORIES(${JSONCPP_INCLUDE_DIR})
  LINK_LIBRARIES(${JSONCPP_LIBRARY})
ENDIF ()

IF (OPENSSL_ROOT_PATH)
  INCLUDE_DIRECTORIES(${OPENSSL_ROOT_PATH}/include)
  LINK_DIRECTORIES(${OPENSSL_ROOT_PATH}/lib)
  LINK_LIBRARIES(crypto)
ELSE ()
  INCLUDE(${CMAKE_ROOT}/Modules/FindOpenSSL.cmake)
  IF (OPENSSL_FOUND)
    INCLUDE_DIRECTORIES(${OPENSSL_INCLUDE_DIR})
    LINK_LIBRARIES(${OPENSSL_CRYPTO_LIBRARY})
  ENDIF ()
ENDIF ()

INCLUDE(FindProtobuf)
FIND_PACKAGE(Protobuf REQUIRED)
FIND_PACKAGE(Boost COMPONENTS system filesystem REQUIRED)

INCLUDE_DIRECTORIES(${PROTOBUF_INCLUDE_DIR})
LINK_LIBRARIES(
  ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY} ${PROTOBUF_LIBRARY}
)

LINK_DIRECTORIES(/usr/local/lib/)
INCLUDE_DIRECTORIES(/usr/local/include/)

INCLUDE(${CMAKE_ROOT}/Modules/CheckIncludeFile.cmake)
CHECK_INCLUDE_FILE("roaring/roaring.h" HAS_ROARING)
IF (HAS_ROARING)
  LINK_LIBRARIES(roaring)
  ADD_DEFINITIONS(-DHAS_ROARING)
ENDIF ()

ENABLE_TESTING()
FOREACH (category tabulate cxxopt progressbar threadpool misc caches parsers)
  FILE(GLOB files samples/${category}/*.cc)
  ADD_CUSTOM_TARGET(${category})
  FOREACH (file ${files})
    GET_FILENAME_COMPONENT(sample ${file} NAME_WE)
    ADD_EXECUTABLE(${category}-${sample} ${file})
    TARGET_LINK_LIBRARIES(${category}-${sample} gtest gtest_main)
    TARGET_INCLUDE_DIRECTORIES(
      ${category}-${sample} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
    )
    CONFIGURETARGET(${category}-${sample})

    IF (NOT EXISTS "${file}.skip")
      ADD_TEST(NAME ${category}-${sample} COMMAND ${category}-${sample}
               WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
      )
      ADD_DEPENDENCIES(${category} ${category}-${sample})
    ENDIF ()
  ENDFOREACH ()
ENDFOREACH ()

ADD_EXECUTABLE(ecsense tools/ecsense.cc)
TARGET_INCLUDE_DIRECTORIES(ecsense PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
TARGET_COMPILE_OPTIONS(ecsense PRIVATE -std=c++20 -Wno-c++20-extensions)
