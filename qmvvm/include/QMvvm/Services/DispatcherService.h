#pragma once

#include <functional>

#include "QMvvm/Interfaces/IApplicationService.h"
#include "QMvvm/Interfaces/IDispatcherService.h"


class DispatcherService : public IDispatcherService {
public:
    explicit DispatcherService( std::shared_ptr<IApplicationService> application );
    ~DispatcherService() override;

    void DispatchToMainThread( std::function<void()> task ) override;

private:
    std::shared_ptr<IApplicationService> m_applicationService;
};
