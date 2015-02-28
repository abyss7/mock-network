#include <mock_network/connection.h>
#include <mock_network/mock.h>
#include <mock_network/test_library.h>

#include <third_party/gtest/exported/include/gtest/gtest.h>

#include <string>

#include <sys/socket.h>

namespace mock_network {

TEST(MockNetworkTest, Socket) {
  String error;
  ASSERT_TRUE(Mock::Enable({".*libtest_library\\.so"}, &error)) << error;

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

  ASSERT_TRUE(Mock::Disable(&error)) << error;
}

TEST(MockNetworkTest, Connect) {
  String error;
  ASSERT_TRUE(Mock::Enable({".*libtest_library\\.so"}, &error)) << error;

  // Unknown socket.
  EXPECT_EQ(-1, test::connect(1, nullptr, 0));
  EXPECT_EQ(ENOTSOCK, errno);

  // Too large address.
  int socket = test::socket(AF_UNIX, SOCK_STREAM, 0);
  char large_address[sizeof(struct sockaddr_storage) + 1];
  EXPECT_NE(-1, socket);
  EXPECT_EQ(-1, test::connect(socket, reinterpret_cast<const struct sockaddr*>(
                                          large_address),
                              sizeof(large_address)));
  EXPECT_EQ(EINVAL, errno);
  EXPECT_NE(-1, test::close(socket));

  // Connect with the same socket twice.
  socket = test::socket(AF_UNIX, SOCK_STREAM, 0);
  struct sockaddr address;
  EXPECT_NE(-1, socket);
  EXPECT_NE(-1, test::connect(socket, &address, sizeof(address)));
  EXPECT_EQ(-1, test::connect(socket, &address, sizeof(address)));
  EXPECT_EQ(EISCONN, errno);
  EXPECT_NE(-1, test::close(socket));

  // Check connection custom result.
  socket = test::socket(AF_UNIX, SOCK_STREAM, 0);
  EXPECT_NE(-1, socket);
  Connection::SetResult(EAGAIN);
  EXPECT_EQ(-1, test::connect(socket, &address, sizeof(address)));
  EXPECT_EQ(EAGAIN, errno);
  EXPECT_NE(-1, test::close(socket));
  Connection::SetResult(0);

  // TODO: cover more test-cases.

  ASSERT_TRUE(Mock::Disable(&error)) << error;
}

TEST(MockNetworkTest, Bind) {
  String error;
  ASSERT_TRUE(Mock::Enable({".*libtest_library\\.so"}, &error)) << error;

  // Unknown socket.
  EXPECT_EQ(-1, test::bind(1, nullptr, 0));
  EXPECT_EQ(ENOTSOCK, errno);

  // Too large address.
  int socket = test::socket(AF_UNIX, SOCK_STREAM, 0);
  char large_address[sizeof(struct sockaddr_storage) + 1];
  EXPECT_NE(-1, socket);
  EXPECT_EQ(-1, test::bind(socket, reinterpret_cast<const struct sockaddr*>(
                                       large_address),
                           sizeof(large_address)));
  EXPECT_EQ(EINVAL, errno);
  EXPECT_NE(-1, test::close(socket));

  // TODO: cover more test-cases.

  ASSERT_TRUE(Mock::Disable(&error)) << error;
}

}  // namespace mock_network
