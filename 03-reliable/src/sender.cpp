#include "sender.hpp"
#include <chrono>
#include <iostream>
using asio::ip::tcp;

Sender::Sender(asio::io_context& io_context,
               std::string ip_addr,
               std::string port)
    : socket(io_context), num_msgs(0), finished(false) {
  tcp::resolver resolver(io_context);
  tcp::resolver::results_type endpoints = resolver.resolve(ip_addr, port);
  asio::connect(socket, endpoints);

  _t = std::thread([&] {
    while (true) {
      while (!socket.available()) {
        if (finished) {
          goto exit_thread;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
      }

      asio::error_code error;
      std::array<char, CHUNK_SIZE + 2> buf;
      buf.fill(' ');
      size_t len = socket.read_some(asio::buffer(buf), error);
      data_lock.lock();
      if (num_msgs < data.size()) {
        data[num_msgs].msg_id = *reinterpret_cast<uint16_t*>(&buf[0]);
        data[num_msgs].data =
            *reinterpret_cast<std::array<char, CHUNK_SIZE>*>(&buf[2]);
        num_msgs++;
      }
      data_lock.unlock();
    }
  exit_thread:;
  });
}

Sender::~Sender() {
  finished = true;
  _t.join();
}

bool Sender::data_ready() {
  return num_msgs > 0;
}

Msg Sender::get_msg() {
  if (!data_ready()) {
    std::cout << "Warning: no Msg instance available! Aborting." << std::endl;
    exit(1);
  }

  data_lock.lock();
  num_msgs--;
  auto msg = data[num_msgs];
  data_lock.unlock();
  return msg;
}

void Sender::request_msg(uint16_t msg_id) {
  asio::error_code error;
  std::array<uint16_t, 1> msg_id_buf;
  msg_id_buf[0] = msg_id;
  asio::write(socket, asio::buffer(msg_id_buf), error);
}