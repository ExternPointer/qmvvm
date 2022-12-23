#pragma once

#include <functional>
#include <memory>
#include <utility>

#include <QMvvm/Interfaces/IObservable.h>

namespace TestHelpers {

class RelayObserver : public IObserver {
public:
    explicit RelayObserver( std::function<void( IObservable* source, std::shared_ptr<BaseObserverEvent> eventArgs )> onEvent ) {
        this->m_onEvent = std::move(onEvent);
    }

    inline void OnEvent( IObservable* source, std::shared_ptr<BaseObserverEvent> eventArgs ) override {
        this->m_onEvent( source, eventArgs );
    }

private:
    std::function<void( IObservable* source, std::shared_ptr<BaseObserverEvent> eventArgs )> m_onEvent;
};

}
