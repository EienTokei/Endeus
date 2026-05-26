#include <gtest/gtest.h>
#include "../src/runtime/WorldModel.hpp"

using namespace endeus;

TEST(WorldModelTest, AddAndGetLayer) {
    WorldModel model;
    LayerData data{ "bg", 0, true, {0,0}, 1.0f, {} };
    bool added = model.addLayer("bg", data);
    EXPECT_TRUE(added);   // 首次添加成功

    const auto* retrieved = model.getLayer("bg");
    ASSERT_NE(retrieved, nullptr);
    EXPECT_EQ(retrieved->textureKey, "bg");
    EXPECT_EQ(retrieved->order, 0);
    EXPECT_TRUE(retrieved->visible);
    // dirty 应该是 true，因为新添加的图层需要渲染器同步
    EXPECT_TRUE(retrieved->dirty);

    // 再次添加相同 id 应失败
    bool addedAgain = model.addLayer("bg", data);
    EXPECT_FALSE(addedAgain);
}

TEST(WorldModelTest, SetLayerDataUpdatesLayer) {
    WorldModel model;
    LayerData data{ "bg", 0, true, {0,0}, 1.0f, {} };
    model.addLayer("bg", data);

    LayerData newData{ "bg", 1, false, {10,20}, 0.5f, {0,0,100,100} };
    bool updated = model.setLayerData("bg", newData);
    EXPECT_TRUE(updated);

    const auto* layer = model.getLayer("bg");
    ASSERT_NE(layer, nullptr);
    EXPECT_EQ(layer->order, 1);
    EXPECT_FALSE(layer->visible);
    EXPECT_EQ(layer->position.x, 10);
    EXPECT_EQ(layer->position.y, 20);
    EXPECT_FLOAT_EQ(layer->alpha, 0.5f);
    EXPECT_EQ(layer->texRect.width, 100);
    EXPECT_TRUE(layer->dirty);   // 数据变化导致 dirty
}

TEST(WorldModelTest, SetLayerDataWithSameDataDoesNotChangeDirty) {
    WorldModel model;
    LayerData data{ "bg", 0, true, {0,0}, 1.0f, 0, {} };
    model.addLayer("bg", data);

    // 获取图层，记录 dirty 状态（刚刚添加完应该是 true）
    const auto* layer = model.getLayer("bg");
    bool firstDirty = layer->dirty;

    const_cast<LayerData*>(layer)->dirty = false;       // 手动修改为 false

    // 设置完全相同的数据
    bool updated = model.setLayerData("bg", data);
    EXPECT_TRUE(updated);           // 即使数据相同，setLayerData 仍会执行
    EXPECT_FALSE(layer->dirty);     // 但不会标记脏
}

TEST(WorldModelTest, SetLayerPosition) {
    WorldModel model;
    LayerData data{ "bg", 0, true, {0,0}, 1.0f, {} };
    model.addLayer("bg", data);

    bool success = model.setLayerPosition("bg", { 100, 200 });
    EXPECT_TRUE(success);
    const auto* layer = model.getLayer("bg");
    EXPECT_EQ(layer->position.x, 100);
    EXPECT_EQ(layer->position.y, 200);
    EXPECT_TRUE(layer->dirty);

    // 更新不存在的图层应失败
    bool fail = model.setLayerPosition("none", { 0,0 });
    EXPECT_FALSE(fail);
}

TEST(WorldModelTest, SetLayerAlpha) {
    WorldModel model;
    LayerData data{ "bg", 0, true, {0,0}, 1.0f, {} };
    model.addLayer("bg", data);

    model.setLayerAlpha("bg", 0.5f);
    const auto* layer = model.getLayer("bg");
    EXPECT_FLOAT_EQ(layer->alpha, 0.5f);
    EXPECT_TRUE(layer->dirty);
}

TEST(WorldModelTest, SetLayerVisible) {
    WorldModel model;
    LayerData data{ "bg", 0, true, {0,0}, 1.0f, {} };
    model.addLayer("bg", data);

    model.setLayerVisible("bg", false);
    const auto* layer = model.getLayer("bg");
    EXPECT_FALSE(layer->visible);
    EXPECT_TRUE(layer->dirty);
}

TEST(WorldModelTest, HideLayerViaSetLayerVisible) {
    WorldModel model;
    LayerData data{ "bg", 0, true, {0,0}, 1.0f, {} };
    model.addLayer("bg", data);

    model.setLayerVisible("bg", false);
    const auto* layer = model.getLayer("bg");
    EXPECT_FALSE(layer->visible);
    EXPECT_TRUE(layer->dirty);
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

TEST(WorldModelTest, GetNonExistentLayerReturnsNull) {
    WorldModel model;
    EXPECT_EQ(model.getLayer("not_exist"), nullptr);
}

// 注：关于 dirty 标志的测试比较敏感，因为 dirty 是供 Runtime 使用的内部标志。
// 如果将来需要测试脏标记行为，最好通过集成测试（创建 Runtime 并检查渲染同步）完成。