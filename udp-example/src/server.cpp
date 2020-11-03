#include <inttypes.h>
#include <asio.hpp>
#include <ctime>
#include <iostream>
#include <string>

using asio::ip::udp;

int main() {
  asio::io_context io_context;
  udp::socket socket(io_context, udp::endpoint(udp::v4(), 3000));

  asio::error_code error;
  std::array<uint8_t, 1> empty;
  std::array<uint8_t, 1> counter = {0};
  while (true) {
    udp::endpoint remote_endpoint;

    socket.receive_from(asio::buffer(empty), remote_endpoint);
    socket.send_to(asio::buffer(counter), remote_endpoint, 0, error);
    std::cout << std::to_string(counter[0]) << std::endl;
    if (counter[0] == 255) {
      break;
    }
    counter[0] += 1;
  }

  return 0;
}
