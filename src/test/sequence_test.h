#pragma once

#include <gtest/gtest.h>
#include "common/sequence.h"

TEST(sequence, type_integer) {
	common::sequence<int> sequence{ 0, 1 };

	auto val = sequence.get();
	EXPECT_EQ(val, 0);

	val = sequence.get();
	EXPECT_EQ(val, 1);
}

TEST(sequence, type_float) {
	common::sequence<float> sequence{ 0.f, 0.1f };

	auto val = sequence.get();
	EXPECT_EQ(val, 0.f);

	val = sequence.get();
	EXPECT_EQ(val, 0.1f);
}