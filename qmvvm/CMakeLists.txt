cmake_minimum_required( VERSION 3.20 )
project( qmvvm_lib )

set( CMAKE_CXX_STANDARD 23 )
set( CMAKE_INCLUDE_CURRENT_DIR ON )
set( CMAKE_AUTOMOC ON )
set( CMAKE_AUTORCC ON )


set( SOURCES
        src/Classes/ListModel.cpp
        src/Classes/ProxyModel.cpp
        src/Classes/EventSubscription.cpp
        src/QmlHelpers/ListModelHelper.cpp
        src/QmlHelpers/AsyncHelper.cpp
        src/Services/ApplicationService.cpp
        src/Services/DispatcherService.cpp
        src/Services/EventsService.cpp
        src/Services/ResourceLockerService.cpp )

set( HEADERS
        include/QMvvm/Classes/CommandLineArgs.h
        include/QMvvm/Interfaces/IApplicationService.h
        include/QMvvm/Interfaces/IEventsService.h
        include/QMvvm/Interfaces/IResourceLockerService.h
        include/QMvvm/Interfaces/IDispatcherService.h
        include/QMvvm/Interfaces/IObservable.h
        include/QMvvm/Interfaces/IViewModel.h
        include/QMvvm/Helpers/TimeHelper.h
        include/QMvvm/Helpers/PropertyHelper.h
        include/QMvvm/Classes/ProxyModel.h
        include/QMvvm/Classes/ListModel.h
        include/QMvvm/Classes/MainContext.h
        include/QMvvm/Classes/EventSubscription.h
        include/QMvvm/Classes/ObservableCollection.h
        include/QMvvm/Services/ApplicationService.h
        include/QMvvm/Services/DispatcherService.h
        include/QMvvm/Services/EventsService.h
        include/QMvvm/Services/ResourceLockerService.h
        include/QMvvm/Services/Configurator.h
        src/QmlHelpers/ListModelHelper.h
        src/QmlHelpers/AsyncHelper.h
        include/QMvvm/DependencyInjection/Injector.h )

find_package( Qt6 COMPONENTS Quick Gui REQUIRED )
add_library( ${PROJECT_NAME} STATIC ${SOURCES} ${HEADERS} qmvvm_qml.qrc )
target_include_directories( ${PROJECT_NAME} PUBLIC include )
target_link_libraries( ${PROJECT_NAME} PUBLIC coroutines Qt6::Quick Qt6::Gui )

