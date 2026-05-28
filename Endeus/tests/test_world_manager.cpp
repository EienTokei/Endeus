#include <gtest/gtest.h>
#include "../src/world/WorldManager.hpp"

using namespace endeus;

class WorldManagerTest : public ::testing::Test {
protected:
    WorldManager wm;
    LayerData bg{ "bg_tex", 0, true, {0,0}, 1.0f, {} };
    LayerData charLayer{ "char_tex", 1, true, {100,200}, 0.8f, {} };
};

TEST_F(WorldManagerTest, AddLayerSuccess) {
    EXPECT_TRUE(wm.addLayer("bg", bg));
    const auto* layer = wm.getLayer("bg");
    ASSERT_NE(layer, nullptr);
    EXPECT_EQ(layer->textureKey, "bg_tex");
    EXPECT_TRUE(layer->dirty);
}

TEST_F(WorldManagerTest, AddLayerDuplicateFails) {
    EXPECT_TRUE(wm.addLayer("bg", bg));
    EXPECT_FALSE(wm.addLayer("bg", bg));
}

TEST_F(WorldManagerTest, SetLayerPosition) {
    wm.addLayer("char", charLayer);
    const auto* layer = wm.getLayer("char");
    layer->dirty = false;   // 模拟同步完成，脏标记清除

    EXPECT_TRUE(wm.setLayerPosition("char", { 200, 300 }));
    EXPECT_EQ(layer->position.x, 200);
    EXPECT_EQ(layer->position.y, 300);
    EXPECT_TRUE(layer->dirty);   // 修改后必须为 true
}

TEST_F(WorldManagerTest, SetLayerAlpha) {
    wm.addLayer("char", charLayer);
    const auto* layer = wm.getLayer("char");
    layer->dirty = false;

    EXPECT_TRUE(wm.setLayerAlpha("char", 0.5f));
    EXPECT_FLOAT_EQ(layer->alpha, 0.5f);
    EXPECT_TRUE(layer->dirty);
}

TEST_F(WorldManagerTest, SetLayerVisible) {
    wm.addLayer("char", charLayer);
    const auto* layer = wm.getLayer("char");
    layer->dirty = false;

    EXPECT_TRUE(wm.setLayerVisible("char", false));
    EXPECT_FALSE(layer->visible);
    EXPECT_TRUE(layer->dirty);
}

TEST_F(WorldManagerTest, SetLayerPositionNoChange) {
    wm.addLayer("char", charLayer);
    const auto* layer1 = wm.getLayer("char");
    layer1->dirty = false;   // mutable 允许修改
    // 设置相同的位置
    EXPECT_TRUE(wm.setLayerPosition("char", charLayer.position));
    // 虽然调用成功（图层存在），但 dirty 应该为 false（未变化）
    const auto* layer2 = wm.getLayer("char");
    EXPECT_FALSE(layer2->dirty);
}

TEST_F(WorldManagerTest, RemoveLayer) {
    wm.addLayer("bg", bg);
    wm.removeLayer("bg");
    EXPECT_EQ(wm.getLayer("bg"), nullptr);
    EXPECT_TRUE(wm.allLayers().empty());
}

TEST_F(WorldManagerTest, TakeOptions) {
    std::vector<Instruction::Choice::Option> opts = { {"Enter", "label_enter"}, {"Exit", "label_exit"} };
    wm.setOptions(opts);
    auto taken = wm.takeOptions();
    EXPECT_EQ(taken.size(), 2);
    EXPECT_EQ(taken[0].text, "Enter");
    EXPECT_TRUE(wm.getOptions().empty());   // 原容器已空
}

TEST_F(WorldManagerTest, SetSpeakerAndContent) {
    wm.setSpeaker("Alice");
    wm.setContent("Hello", false);
    EXPECT_EQ(wm.getSpeaker(), "Alice");
    EXPECT_EQ(wm.getContent(), "Hello");

    wm.setContent(" world", true);
    EXPECT_EQ(wm.getContent(), "Hello world");
}

TEST_F(WorldManagerTest, GetWorldReturnsConstRef) {
    wm.addLayer("bg", bg);
    const World& world1 = wm.getWorld();
    const World& world2 = wm.getWorld();
    EXPECT_EQ(&world1, &world2);  // 保证每次调用返回同一对象的引用
}