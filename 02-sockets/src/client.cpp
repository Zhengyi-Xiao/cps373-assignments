#include <asio.hpp>
#include <iostream>

using asio::ip::tcp;

int main(int argc, char *argv[]) {
  //Add an order: 1001 0010 0101 0000 0001 0000
  std::array<uint8_t, 3> buf0;
  buf0.fill(0);
  buf0[0] |= 1 << 7;
  buf0[0] |= 1 << 4;
  buf0[0] |= 1 << 1;
  buf0[1] |= 1 << 6;
  buf0[1] |= 1 << 4;
  buf0[2] |= 1 << 4;
  //Get the number of orders for E=01: 0101 0000 0000 0000 0000 0000 
  std::array<uint8_t, 3> buf1;
  buf1.fill(0);
  buf1[0] |= 1 << 6;
  buf1[0] |= 1 << 4;
  //Get the total number of orders
  std::array<uint8_t, 3> buf2;
  buf2.fill(0);
  std::array<std::array<uint8_t, 3>,4> queries ={buf1, buf0, buf1, buf2};
  int i = 0;
  for(auto query : queries){
    asio::io_context io_context;
    tcp::resolver resolver(io_context);
    tcp::resolver::results_type endpoints = resolver.resolve("127.0.0.1", "3000");

    tcp::socket socket(io_context);
    asio::connect(socket, endpoints);

    asio::error_code error;
    std::array<uint8_t, 3> buf = query;
    asio::write(socket, asio::buffer(query), error);

    size_t len = socket.read_some(asio::buffer(query), error);
    uint16_t response = *reinterpret_cast<uint16_t*>(&query.data()[0]);
    if(i==0)
      std::cout << "The number of orders for E=01: " << response << std::endl;
    if(i==1)
      std::cout << "Add an order to E=01:          " << response << std::endl;
    if(i==2)
      std::cout << "The number of orders for E=01: " << response << std::endl;
    if(i==3)
      std::cout << "The total number of orders:    " << response << std::endl;
    i++;
  }


  return 0;
}