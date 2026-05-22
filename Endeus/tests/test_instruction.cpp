#include <gtest/gtest.h>
#include "../src/script/Instruction.hpp"

using namespace endeus;

TEST(InstructionTest, ShowLayerTypeAndFields) {
    Instruction::ShowLayer show{ "bg", "bg_janus", {100, 200}, 1, {0,0,400,300} };
    Instruction instr(show);

    EXPECT_TRUE(instr.is<Instruction::ShowLayer>());
    EXPECT_FALSE(instr.is<Instruction::HideLayer>());

    const auto* ptr = instr.getIf<Instruction::ShowLayer>();
    ASSERT_NE(ptr, nullptr);
    EXPECT_EQ(ptr->layerId, "bg");
    EXPECT_EQ(ptr->textureKey, "bg_janus");
    EXPECT_FLOAT_EQ(ptr->position.x, 100);
    EXPECT_FLOAT_EQ(ptr->position.y, 200);
    EXPECT_EQ(ptr->order, 1);
    EXPECT_EQ(ptr->texRect.width, 400);
}

TEST(InstructionTest, SetSpeakerAndContent) {
    Instruction::SetSpeaker spk{ "Alice" };
    Instruction instr1(spk);
    EXPECT_TRUE(instr1.is<Instruction::SetSpeaker>());
    EXPECT_EQ(instr1.getIf<Instruction::SetSpeaker>()->name, "Alice");

    Instruction::SetContent cont{ "Hello", false };
    Instruction instr2(cont);
    EXPECT_TRUE(instr2.is<Instruction::SetContent>());
    EXPECT_EQ(instr2.getIf<Instruction::SetContent>()->content, "Hello");
    EXPECT_FALSE(instr2.getIf<Instruction::SetContent>()->append);
}

TEST(InstructionTest, ChoiceOptions) {
    Instruction::Choice::Option opt1{ "Write", "label_writer" };
    Instruction::Choice::Option opt2{ "Read", "label_reader" };
    Instruction::Choice choice{ {opt1, opt2} };
    Instruction instr(choice);

    const auto* ptr = instr.getIf<Instruction::Choice>();
    ASSERT_NE(ptr, nullptr);
    ASSERT_EQ(ptr->options.size(), 2);
    EXPECT_EQ(ptr->options[0].text, "Write");
    EXPECT_EQ(ptr->options[1].targetLabel, "label_reader");
}