cmake_minimum_required( VERSION 3.20 )
project( qmvvm )

add_subdirectory( qmvvm )
add_subdirectory( third_party/coroutines )

find_package( Qt6 COMPONENTS Quick Gui REQUIRED )
add_library( ${PROJECT_NAME} INTERFACE )
target_link_libraries( ${PROJECT_NAME} INTERFACE qmvvm_lib coroutines Qt6::Quick Qt6::Gui )

enable_testing()
add_subdirectory( qmvvm_tests )