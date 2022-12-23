#pragma once

#include <functional>
#include <memory>


class BaseObserverEvent;
class IObserver;
class IObservable;

class IObserver {
public:
    virtual ~IObserver() = default;
    virtual void OnEvent( IObservable* source, std::shared_ptr<BaseObserverEvent> eventArgs ) = 0;
};

class IObservable {
public:
    virtual ~IObservable() = default;
    virtual void AttachObserver( IObserver* observer ) = 0;
    virtual void DetachObserver( IObserver* observer ) = 0;
};

class BaseObserverEvent {
public:
    virtual ~BaseObserverEvent() = default;
};
