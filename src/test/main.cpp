

#include <gtest/gtest.h>

#include "metrics_test.h"
#include "network_test.h"
#include "packet_handler_test.h"
#include "sequence_test.h"
#include "session_test.h"
#include "sync_test.h"
#include "world_test.h"
#include "zone_test.h"

int
main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}