#include "QMvvm/Classes/EventSubscription.h"

#include <utility>
#include "QMvvm/Interfaces/IEventsService.h"


EventSubscription::EventSubscription()
    : EventSubscription( 0, std::weak_ptr<IEventsService>() ) {
}

EventSubscription::EventSubscription( uint64_t id, std::weak_ptr<IEventsService> eventsService ) {
    this->m_id = id;
    this->m_eventsService = std::move(eventsService);
}

EventSubscription::EventSubscription( EventSubscription&& other ) noexcept {
    this->Move( std::move( other ) );
}

EventSubscription& EventSubscription::operator=( EventSubscription&& other )  noexcept {
    this->Move( std::move( other ) );
    return *this;
}

EventSubscription::~EventSubscription() {
    this->Unsubscribe();
}

void EventSubscription::Unsubscribe() {
    auto eventsService = this->m_eventsService.lock();
    if( eventsService ) {
        eventsService->Unsubscribe( this->m_id );
    }
}

void EventSubscription::Move( EventSubscription&& other ) {
    this->m_id = other.m_id;
    this->m_eventsService = other.m_eventsService;

    other.m_eventsService = std::weak_ptr<IEventsService>();
}
