#pragma once

#include <gtest/gtest.h>
#include "network/metrics.h"

TEST(metrics, measurement_change) {
	auto expected = network::metrics::measurement{
		1, 100, 100, 0
	};

	network::metrics::connection();
	network::metrics::connection();
	network::metrics::disconnection();
	network::metrics::read(100);
	network::metrics::written(100);
	auto actual = network::metrics::measure();

	EXPECT_EQ(expected.connections, actual.connections);
	EXPECT_EQ(expected.read, actual.read);
	EXPECT_EQ(expected.written, actual.written);
	EXPECT_LE(expected.time_passed, actual.time_passed);
}