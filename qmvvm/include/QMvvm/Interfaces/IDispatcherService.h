#pragma once

#include <functional>


class IDispatcherService {
public:
    virtual ~IDispatcherService() = default;
    virtual void DispatchToMainThread( std::function<void()> task ) = 0;
};
