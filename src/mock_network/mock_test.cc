#include <mock_network/mock.h>
#include <mock_network/test_library.h>

#include <third_party/gtest/exported/include/gtest/gtest.h>

#include <string>

#include <sys/socket.h>

namespace mock_network {

TEST(MockNetworkTest, CreateSocket) {
  String error;
  ASSERT_TRUE(Enable(&error)) << error;

  int socket = test::socket(AF_UNIX, SOCK_STREAM, 0);
  EXPECT_NE(-1, socket);
  EXPECT_NE(-1, test::close(socket));

  socket = test::socket(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
  EXPECT_NE(-1, socket);
  EXPECT_NE(-1, test::close(socket));

  socket = test::socket(AF_INET, SOCK_STREAM, 0);
  EXPECT_NE(-1, socket);
  EXPECT_NE(-1, test::close(socket));

  socket = test::socket(AF_INET6, SOCK_STREAM, 0);
  EXPECT_NE(-1, socket);
  EXPECT_NE(-1, test::close(socket));

  EXPECT_EQ(-1, test::socket(AF_PACKET, SOCK_STREAM, 0));
  EXPECT_EQ(EINVAL, errno);

  EXPECT_EQ(-1, test::socket(AF_UNIX, SOCK_DGRAM, 0));
  EXPECT_EQ(EINVAL, errno);

  EXPECT_EQ(-1, test::socket(AF_UNIX, SOCK_STREAM, 1));

  ASSERT_TRUE(Disable(&error)) << error;
}

}  // namespace mock_network
