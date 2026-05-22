#include <gtest/gtest.h>
#include "../src/runtime/WorldModel.hpp"

using namespace endeus;

TEST(WorldModelTest, SetAndGetLayer) {
    WorldModel model;
    LayerData data{ "bg", 0, true, {0,0}, 1.0f, {} };
    model.setLayer("bg", data);
    const auto* retrieved = model.getLayer("bg");
    ASSERT_NE(retrieved, nullptr);
    EXPECT_EQ(retrieved->textureKey, "bg");
    EXPECT_EQ(retrieved->order, 0);
    EXPECT_TRUE(retrieved->visible);
    // 脏标记应在 setLayer 时被设为 true
    EXPECT_TRUE(retrieved->dirty);
}

TEST(WorldModelTest, DirtyFlagResetOnSameData) {
    WorldModel model;
    LayerData data{ "bg", 0, true, {0,0}, 1.0f, {} };
    model.setLayer("bg", data);
    const auto* first = model.getLayer("bg");
    EXPECT_TRUE(first->dirty);

    first->dirty = false;   // 这里手动重置 dirty 为 false

    LayerData data1{ "bg", 1, true, {0,0}, 1.0f, {} };       // 修改 order
    model.setLayer("bg", data1);
    first = model.getLayer("bg");
    EXPECT_TRUE(first->dirty);

    first->dirty = false;   // 这里手动重置 dirty 为 false

    LayerData data2{ "bg", 1, true, {0,0}, 1.0f, {} };       // 设置相同数据
    model.setLayer("bg", data2);
    first = model.getLayer("bg");
    EXPECT_FALSE(first->dirty);
}

TEST(WorldModelTest, HideLayer) {
    WorldModel model;
    LayerData data{ "bg", 0, true, {0,0}, 1.0f, {} };
    model.setLayer("bg", data);
    model.hideLayer("bg");
    const auto* layer = model.getLayer("bg");
    EXPECT_FALSE(layer->visible);
    EXPECT_TRUE(layer->dirty);  // 因为 visible 改变
}

TEST(WorldModelTest, SpeakerAndContent) {
    WorldModel model;
    model.setSpeaker("Alice");
    EXPECT_EQ(model.getSpeaker(), "Alice");

    model.setContent("Ciallo", false);
    EXPECT_EQ(model.getContent(), "Ciallo");
    model.setContent(" world", true);
    EXPECT_EQ(model.getContent(), "Ciallo world");
}

TEST(WorldModelTest, ChoiceManagement) {
    WorldModel model;
    std::vector<ChoiceOption> opts = { {"Yes", "label_yes"}, {"No", "label_no"} };
    model.setChoice(opts);
    EXPECT_TRUE(model.hasChoice());
    EXPECT_EQ(model.getChoiceOptions().size(), 2);

    model.clearChoice();
    EXPECT_FALSE(model.hasChoice());
    EXPECT_TRUE(model.getChoiceOptions().empty());
}

TEST(WorldModelTest, GetNonExistentLayerReturnsNull) {
    WorldModel model;
    EXPECT_EQ(model.getLayer("not_exist"), nullptr);
}