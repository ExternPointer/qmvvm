cmake_minimum_required( VERSION 3.20 )
project( qmvvm_tests )

set( CMAKE_CXX_STANDARD 23 )
set( CMAKE_INCLUDE_CURRENT_DIR ON )

set( SOURCES Classes/ObservableCollectionTests.cpp
             Services/EventsServiceTests.cpp
             main.cpp )

set( HEADERS TestHelpers/RelayObserver.h
             TestHelpers/VectorHelper.h )


find_package( Threads REQUIRED )

include( FetchContent )
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
set( gtest_force_shared_crt ON CACHE BOOL "" FORCE )
FetchContent_MakeAvailable( googletest )

add_executable( ${PROJECT_NAME} ${SOURCES} ${HEADERS} ${GTestFiles} )
target_link_libraries( ${PROJECT_NAME} Threads::Threads gtest_main gmock_main qmvvm_lib )

include( GoogleTest )
gtest_add_tests( ${PROJECT_NAME} ${PROJECT_NAME} AUTO )