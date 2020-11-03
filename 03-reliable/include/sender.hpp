#ifndef _SENDER_H_
#define _SENDER_H_

#include <array>
#include <asio.hpp>
#include <mutex>
using asio::ip::tcp;

#define NUM_MSGS 854
#define CHUNK_SIZE 128
#define NUM_CHUNKS 10

struct Msg {
  uint16_t msg_id;
  std::array<char, CHUNK_SIZE> data;
};

class Sender {
 private:
  std::array<Msg, NUM_CHUNKS> data;
  std::mutex data_lock;
  std::thread _t;
  tcp::socket socket;
  size_t num_msgs;
  bool finished;

 public:
  Sender(asio::io_context&, std::string, std::string);
  ~Sender();
  bool data_ready();
  Msg get_msg();
  void request_msg(uint16_t msg_id);
};

#endif /* _SENDER_H_ */