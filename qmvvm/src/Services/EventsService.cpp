#include <QDebug>

#include "QMvvm/Services/EventsService.h"


EventsService::EventsService() {
    this->m_subscriptionsCounter = 0;
    qInfo() << "EventsService created";
}

EventsService::~EventsService() {
    qInfo() << "EventsService destroyed";
}

EventSubscription EventsService::Subscribe( std::type_index eventId, event_handler_t handler ) {
    qInfo() << "New subscription:" << eventId.name() << "id:" << this->m_subscriptionsCounter;
    std::lock_guard locker( this->m_mutex );
    auto subscription = std::make_shared<subscription_t>();
    subscription->m_id = this->m_subscriptionsCounter++;
    subscription->m_eventId = eventId;
    subscription->m_handler = handler;
    this->m_idToSubscriptionsMap[ subscription->m_id ] = subscription;
    if( this->m_eventIdToSubscriptionsMap.find( subscription->m_eventId ) != this->m_eventIdToSubscriptionsMap.end() ) {
        this->m_eventIdToSubscriptionsMap[ subscription->m_eventId ].insert( subscription );
    } else {
        std::set<std::shared_ptr<subscription_t>> set;
        set.insert( subscription );
        this->m_eventIdToSubscriptionsMap[ subscription->m_eventId ] = set;
    }
    return EventSubscription( subscription->m_id, this->weak_from_this() );
}

void EventsService::Invoke( std::type_index eventId, std::shared_ptr<void> sender, std::shared_ptr<BaseEventArgs> args ) {
    qInfo() << "Invoke:" << eventId.name();
    std::set<std::shared_ptr<subscription_t>> subscriptions;
    {
        std::lock_guard locker( this->m_mutex );
        this->m_lastEvents[ eventId ] = args;
        if( this->m_eventIdToSubscriptionsMap.find( eventId ) != this->m_eventIdToSubscriptionsMap.end() ) {
            subscriptions = this->m_eventIdToSubscriptionsMap[ eventId ];
        }
    }
    for( const auto& s: subscriptions ) {
        s->m_handler( sender, args );
    }
}

std::shared_ptr<BaseEventArgs> EventsService::GetLastEvent( std::type_index eventId ) {
    std::lock_guard locker( this->m_mutex );
    if( this->m_lastEvents.find( eventId ) != this->m_lastEvents.end() ) {
        return this->m_lastEvents[ eventId ];
    }
    return nullptr;
}

void EventsService::Unsubscribe( uint64_t subscriptionId ) {
    qInfo() << "Unsubscribe:" << subscriptionId;
    std::lock_guard locker( this->m_mutex );
    if( this->m_idToSubscriptionsMap.find( subscriptionId ) == this->m_idToSubscriptionsMap.end() ) {
        qWarning() << "Unknown subscription id";
        return;
    }
    auto subscription = this->m_idToSubscriptionsMap[ subscriptionId ];
    auto subscriptions = &this->m_eventIdToSubscriptionsMap[ subscription->m_eventId ];
    this->m_idToSubscriptionsMap.erase( subscriptionId );
    subscriptions->erase( subscription );
}
