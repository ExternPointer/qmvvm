#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>
#include <memory>
#include <utility>

#include <QMvvm/Services/EventsService.h>

using namespace testing;

class TestEventArgs : public BaseEventArgs {};
class EventSender {};

class EventsServiceFixture : public Test {
protected:
    void SetUp() override {
        this->m_eventsService = std::make_shared<EventsService>();
        this->m_eventHandler = [ this ]( std::shared_ptr<void> sender, std::shared_ptr<BaseEventArgs> args ) {
            this->m_lastSender = std::move(sender);
            this->m_lastArgs = std::move(args);
        };
    }

    void TearDown() override {
    }

    std::shared_ptr<IEventsService> m_eventsService;
    std::function<void( std::shared_ptr<void>, std::shared_ptr<BaseEventArgs> )> m_eventHandler;
    std::shared_ptr<void> m_lastSender;
    std::shared_ptr<BaseEventArgs> m_lastArgs;
};
typedef EventsServiceFixture EventsServiceTests;


TEST_F( EventsServiceTests, SubscribersReceiveEvents ) {
    // Arrange
    auto subscription = this->m_eventsService->Subscribe<int>( this->m_eventHandler );
    auto sender = std::make_shared<EventSender>();
    auto args = std::make_shared<TestEventArgs>();

    // Act
    this->m_eventsService->Invoke<int>( sender, args );

    // Assert
    EXPECT_EQ( this->m_lastSender, sender );
    EXPECT_EQ( this->m_lastArgs, args );
}

TEST_F( EventsServiceTests, DoesNotReceiveEventAfterUnsubscribingEvents ) {
    // Arrange
    auto subscription = this->m_eventsService->Subscribe<int>( this->m_eventHandler );
    auto sender = std::make_shared<EventSender>();
    auto args = std::make_shared<TestEventArgs>();

    // Act
    subscription.Unsubscribe();
    this->m_eventsService->Invoke<int>( sender, args );

    // Assert
    EXPECT_EQ( this->m_lastSender, nullptr );
    EXPECT_EQ( this->m_lastArgs, nullptr );
}

// TODO: Rewrite this test with gmock
TEST_F( EventsServiceTests, AutoUnsubscribing ) {
    // Arrange
    auto sender = std::make_shared<EventSender>();
    auto args = std::make_shared<TestEventArgs>();
    { auto subscription = this->m_eventsService->Subscribe<int>( this->m_eventHandler ); }

    // Act
    this->m_eventsService->Invoke<int>( sender, args );

    // Assert
    EXPECT_EQ( this->m_lastSender, nullptr );
    EXPECT_EQ( this->m_lastArgs, nullptr );
}

TEST_F( EventsServiceTests, DoesNotReceiveEventsIfNotSubscribed ) {
    // Arrange
    auto subscription = this->m_eventsService->Subscribe<int>( this->m_eventHandler );
    auto sender = std::make_shared<EventSender>();
    auto args = std::make_shared<TestEventArgs>();

    // Act
    this->m_eventsService->Invoke<float>( sender, args );

    // Assert
    EXPECT_EQ( this->m_lastSender, nullptr );
    EXPECT_EQ( this->m_lastArgs, nullptr );
}
