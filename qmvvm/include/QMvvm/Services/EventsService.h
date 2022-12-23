#pragma once

#include <memory>
#include <mutex>
#include <set>
#include <unordered_map>

#include "QMvvm/Interfaces/IEventsService.h"


typedef struct subscription_s {
    subscription_s()
        : m_eventId( typeid( subscription_s ) ) {
    }
    uint64_t m_id = 0;
    std::type_index m_eventId;
    event_handler_t m_handler;
} subscription_t;

class EventsService : public IEventsService, public std::enable_shared_from_this<EventsService> {
public:
    EventsService();
    ~EventsService() override;

protected:
    EventSubscription Subscribe( std::type_index eventId, event_handler_t handler ) override;
    void Invoke( std::type_index eventId, std::shared_ptr<void> sender, std::shared_ptr<BaseEventArgs> args ) override;
    std::shared_ptr<BaseEventArgs> GetLastEvent( std::type_index eventId ) override;
    void Unsubscribe( uint64_t subscriptionId ) override;

private:
    std::unordered_map<int, std::shared_ptr<subscription_t>> m_idToSubscriptionsMap;
    std::unordered_map<std::type_index, std::set<std::shared_ptr<subscription_t>>> m_eventIdToSubscriptionsMap;
    std::unordered_map<std::type_index, std::shared_ptr<BaseEventArgs>> m_lastEvents;
    uint64_t m_subscriptionsCounter;
    std::mutex m_mutex;
};
