#include <gtest/gtest.h>
#include "../src/event/Event.hpp"
#include "../src/event/EventBus.hpp"
#include "../src/utils/Types.hpp"

using namespace endeus;

TEST(EventBusTest, SubscribeAndPublish) {
    EventBus bus;
    bool actionCompleted = false;
    bool choiceSelected = false;

    bus.subscribe<Event::ActionCompleted>([&](const Event&) {
        actionCompleted = true;
    });
    bus.subscribe<Event::ChoiceSelected>([&](const Event&) {
        choiceSelected = true;
    });

    bus.publish(Event::ActionCompleted{});
    EXPECT_TRUE(actionCompleted);
    EXPECT_FALSE(choiceSelected);

    bus.publish(Event::ChoiceSelected{ "target" });
    EXPECT_TRUE(choiceSelected);
}

TEST(EventBusTest, MultipleSubscribers) {
    EventBus bus;
    int count = 0;
    auto cb = [&](const Event&) { ++count; };
    bus.subscribe<Event::ActionCompleted>(cb);
    bus.subscribe<Event::ActionCompleted>(cb);

    bus.publish(Event::ActionCompleted{});
    EXPECT_EQ(count, 2);
}

TEST(EventBusTest, GetIfWorks) {
    Event e(Event::MouseClicked{ {100.f, 200.f} });
    ASSERT_TRUE(e.is<Event::MouseClicked>());
    auto* ptr = e.getIf<Event::MouseClicked>();
    ASSERT_NE(ptr, nullptr);
    EXPECT_FLOAT_EQ(ptr->position.x, 100);
    EXPECT_FLOAT_EQ(ptr->position.y, 200);
}