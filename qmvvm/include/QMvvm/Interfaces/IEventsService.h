#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <typeindex>
#include <utility>

#include "QMvvm/Classes/EventSubscription.h"


class BaseEventArgs {
public:
    virtual ~BaseEventArgs() = default;
};
typedef std::function<void( std::shared_ptr<void>, std::shared_ptr<BaseEventArgs> )> event_handler_t;

class IEventsService {
    friend EventSubscription;

public:
    template<typename TEvent>
    EventSubscription Subscribe( event_handler_t handler ) {
        auto index = std::type_index( typeid( TEvent ) );
        return Subscribe( index, std::move( handler ) );
    }

    template<typename TEvent, typename TEventArgs>
    void Invoke( std::shared_ptr<void> sender, std::shared_ptr<TEventArgs> args ) {
        auto index = std::type_index( typeid( TEvent ) );
        this->Invoke( index, sender, args );
    }

    template<typename TEvent>
    std::shared_ptr<TEvent> GetLastEvent() {
        auto index = std::type_index( typeid( TEvent ) );
        return std::static_pointer_cast<TEvent>( this->GetLastEvent( index ) );
    }

public:
    virtual ~IEventsService() = default;

protected:
    virtual EventSubscription Subscribe( std::type_index eventId, event_handler_t handler ) = 0;
    virtual void Invoke( std::type_index eventId, std::shared_ptr<void> sender, std::shared_ptr<BaseEventArgs> args ) = 0;
    virtual std::shared_ptr<BaseEventArgs> GetLastEvent( std::type_index eventId ) = 0;
    virtual void Unsubscribe( uint64_t subscriptionId ) = 0;
};
