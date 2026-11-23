#include <gtest/gtest.h>

#include "Audio/TeAudioUtility.h"

TEST(AudioUtility, ConvertsSupportedBitDepths)
{
    const te::INT8 input8[] = { 0x12, 0x34 };
    te::INT32 output32[2] = {};
    te::AudioUtility::ConvertBitDepth(reinterpret_cast<const te::UINT8*>(input8), 8,
        reinterpret_cast<te::UINT8*>(output32), 32, 2);
    EXPECT_EQ(output32[0], 0x12000000);
    EXPECT_EQ(output32[1], 0x34000000);

    const te::INT16 input16[] = { 0x1234, 0x5678 };
    te::UINT8 output8[2] = {};
    te::AudioUtility::ConvertBitDepth(reinterpret_cast<const te::UINT8*>(input16), 16, output8, 8, 2);
    EXPECT_EQ(output8[0], 0x12);
    EXPECT_EQ(output8[1], 0x56);

    const te::UINT8 input24[] = { 0x56, 0x34, 0x12, 0xcd, 0xab, 0x7f };
    te::INT16 output16[2] = {};
    te::AudioUtility::ConvertBitDepth(input24, 24, reinterpret_cast<te::UINT8*>(output16), 16, 2);
    EXPECT_EQ(output16[0], 0x1234);
    EXPECT_EQ(output16[1], 0x7fab);

    te::AudioUtility::ConvertBitDepth(reinterpret_cast<const te::UINT8*>(output32), 32, output8, 8, 2);
    EXPECT_EQ(output8[0], 0x12);
    EXPECT_EQ(output8[1], 0x34);
    te::AudioUtility::ConvertBitDepth(reinterpret_cast<const te::UINT8*>(output32), 32,
        reinterpret_cast<te::UINT8*>(output16), 16, 2);
    EXPECT_EQ(output16[0], 0x1200);
    EXPECT_EQ(output16[1], 0x3400);
}

TEST(AudioUtility, RejectsUnsupportedInputBitDepth)
{
    const te::UINT8 input[] = { 0x12 };
    te::UINT8 output[] = { 0x34 };
#ifdef NDEBUG
    te::AudioUtility::ConvertBitDepth(input, 7, output, 8, 1);
    EXPECT_EQ(output[0], 0x34);
#else
    EXPECT_DEATH(te::AudioUtility::ConvertBitDepth(input, 7, output, 8, 1), "");
#endif
}
