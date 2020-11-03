#include <asio.hpp>
#include <iostream>

using asio::ip::udp;

int main(int argc, char* argv[]) {
  asio::io_context io_context;
  udp::resolver resolver(io_context);
  udp::endpoint receiver_endpoint =
      *resolver.resolve(udp::v4(), argv[1], "3000").begin();

  udp::socket socket(io_context);
  socket.open(udp::v4());
  std::array<uint8_t, 1> empty;
  std::array<uint8_t, 1> counter = {0};

  for (int i = 0; i < 256; ++i) {
    socket.send_to(asio::buffer(empty), receiver_endpoint);
  }

  for (int i = 0; i < 256; ++i) {
    udp::endpoint sender_endpoint;
    size_t len = socket.receive_from(asio::buffer(counter), sender_endpoint);
    std::cout << std::to_string(counter[0]) << "  " << i << std::endl;
  }

  return 0;
}
