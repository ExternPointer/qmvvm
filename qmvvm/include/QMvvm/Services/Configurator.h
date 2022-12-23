#pragma once

#include <Coroutines/Async.h>

#include "QMvvm/Classes/CommandLineArgs.h"
#include "QMvvm/Classes/MainContext.h"
#include "QMvvm/DependencyInjection/Injector.h"
#include "QMvvm/Services/ApplicationService.h"
#include "QMvvm/Services/DispatcherService.h"
#include "QMvvm/Services/EventsService.h"
#include "QMvvm/Services/ResourceLockerService.h"
#include "../../../src/QmlHelpers/AsyncHelper.h"
#include "../../../src/QmlHelpers/ListModelHelper.h"

std::shared_ptr<ThreadPool> g_threadPool;
inline auto ConfigureCoreServices( int argc, char* argv[] ) {
    g_threadPool = std::make_shared<ThreadPool>();
    CommandLineArgs::Setup( argc, argv );
    return [ = ] {
        return boost::di::make_injector(
            boost::di::bind<CommandLineArgs>().in( di::singleton ),
            boost::di::bind<ThreadPool>().to( g_threadPool ),
            boost::di::bind<AsyncHelper>().in( di::singleton ),
            boost::di::bind<ListModelHelper>().in( di::singleton ),
            boost::di::bind<IApplicationService>().in( di::singleton ).to<ApplicationService>(),
            boost::di::bind<MainContext>().in( di::singleton ),
            boost::di::bind<IDispatcherService>().in( di::singleton ).to<DispatcherService>(),
            boost::di::bind<IEventsService>().in( di::singleton ).to<EventsService>(),
            boost::di::bind<IResourceLockerService>().in( di::singleton ).to<ResourceLockerService>() );
    };
}
