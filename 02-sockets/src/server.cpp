#include <inttypes.h>
#include <asio.hpp>
#include <ctime>
#include <iostream>
#include <string>

#include "bakery.hpp"

using asio::ip::tcp;

inline static int getNumOrders(int E, Bakery* bakery){
  int num = 0;
  switch (E){
  case 0:
    for (auto order : bakery->orders)
      if(order.employee == "Brad")
          num ++;
    break;
  
  case 1:
    for (auto order : bakery->orders)
      if(order.employee == "Claudia")
        num ++;
    break;
  
  case 2:
    for (auto order : bakery->orders)
      if(order.employee == "Simone")
        num ++;
    break;

  default:
    for (auto order : bakery->orders)
      num ++;
    break;
  }
  return num;
}

inline static void addOrder(std::array<uint8_t, 3>* buf, Bakery* bakery){
  int E = ((uint8_t((*buf)[0])>>4)&3);
  int Q1 = (uint8_t((*buf)[0])&15);
  int Q2 = (uint8_t((*buf)[1])>>4);
  int Q3 = (uint8_t((*buf)[1])&15);
  int Q4 = (uint8_t((*buf)[2])>>4);
  int Q5 = (uint8_t((*buf)[2])&15);
  Order order;
  std::string quantity, item_name;
  std::array<std::string, 5> itemName = {"Biscuit","Bun","Brownie","White Loaf","Wheat Loaf"};
  std::array<int, 5> itemQuantity = {Q1,Q2,Q3,Q4,Q5};
  switch (E){
    case 0:
      order.employee = "Brad";
      break;
    
    case 1:
      order.employee = "Claudia";
      break;
    
    case 2:
      order.employee = "Simone";
      break;
  }
  for(int i=0;i<5;i++)
    order.items.push_back(std::make_pair(itemName[i], std::to_string(itemQuantity[i])));
  bakery->orders.push_back(order);
}

int main() {
  asio::io_context io_context;
  tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 3000));

  // Use this bakery to handle queries from the client
  Bakery bakery = text_deserializer("../data/bakery.txt");

  uint16_t numOrder = 0;
  while (true) {
    // Wait for client
    std::cout << "Blocked for read" << std::endl;
    tcp::socket socket(io_context);
    acceptor.accept(socket);

    std::array<uint8_t, 3> buf;
    asio::error_code error;
    size_t len = socket.read_some(asio::buffer(buf), error);

    // Example of error handling
    // if (error != asio::error::eof)
    //   throw asio::system_error(error);
    
    auto x = uint8_t(buf[0]);
    //x>>6 gives the first two bits, which is M
    switch (x>>6){
    // When M = 0, it will get total number of orders. 3 is set to be this action.
    case 0:
      numOrder=getNumOrders(3,&bakery);
      break;
    // When M = 1, the follwing E is ((x>>4)&3)
    case 1:
      numOrder=getNumOrders(((x>>4)&3),&bakery);
      break;   
    // When M = 0b10 = 2, we need to add order. 
    case 2:
      addOrder(&buf,&bakery);
      break;
    // In any other case, we will do nothing
    default:
      continue;
    }
    
    buf.fill(0);
    if((x>>6)!=2)
      std::memcpy(&buf, &numOrder, sizeof(uint16_t));
    asio::write(socket, asio::buffer(buf), error);
  }

  return 0;
}