#pragma once

#include <cstdint>
#include <memory>


class IEventsService;

class EventSubscription {
public:
    EventSubscription();
    EventSubscription( uint64_t id, std::weak_ptr<IEventsService> eventsService );
    EventSubscription( EventSubscription&& other ) noexcept ;
    EventSubscription& operator=( EventSubscription&& other ) noexcept ;
    ~EventSubscription();

    void Unsubscribe();

    EventSubscription( const EventSubscription& other ) = delete;
    EventSubscription& operator=( const EventSubscription& other ) = delete;

private:
    void Move( EventSubscription&& other );
    uint64_t m_id;
    std::weak_ptr<IEventsService> m_eventsService;
};
