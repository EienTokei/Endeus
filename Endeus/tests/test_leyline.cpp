#include <gtest/gtest.h>
#include "../src/event/Event.hpp"
#include "../src/event/Leyline.hpp"
#include "../src/utils/Types.hpp"

using namespace endeus;

TEST(LeylineTest, SubscribeAndPublish) {
    Leyline leyline;
    bool actionCompleted = false;
    bool choiceSelected = false;

    leyline.subscribe<Event::ActionCompleted>([&](const Event&) {
        actionCompleted = true;
    });
    leyline.subscribe<Event::ChoiceSelected>([&](const Event&) {
        choiceSelected = true;
    });

    leyline.publish(Event::ActionCompleted{});
    EXPECT_TRUE(actionCompleted);
    EXPECT_FALSE(choiceSelected);

    leyline.publish(Event::ChoiceSelected{ "target" });
    EXPECT_TRUE(choiceSelected);
}

TEST(LeylineTest, MultipleSubscribers) {
    Leyline leyline;
    int count = 0;
    auto cb = [&](const Event&) { ++count; };
    leyline.subscribe<Event::ActionCompleted>(cb);
    leyline.subscribe<Event::ActionCompleted>(cb);

    leyline.publish(Event::ActionCompleted{});
    EXPECT_EQ(count, 2);
}

TEST(LeylineTest, GetIfWorks) {
    Event e(Event::MouseClicked{ {100.f, 200.f} });
    ASSERT_TRUE(e.is<Event::MouseClicked>());
    auto* ptr = e.getIf<Event::MouseClicked>();
    ASSERT_NE(ptr, nullptr);
    EXPECT_FLOAT_EQ(ptr->position.x, 100);
    EXPECT_FLOAT_EQ(ptr->position.y, 200);
}