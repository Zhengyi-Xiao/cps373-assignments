#include <asio.hpp>
#include <chrono>
#include <iostream>
#include <fstream>
#include <sstream> 
#include "../include/sender.hpp"

using asio::ip::tcp;

void test(){
  std::ifstream output2;
  output2.open ("../data/output.txt");
  std::ifstream output1;
  output1.open ("../data/gilgamesh.txt");
  std::string line;
  std::string line1;
  while (!output1.eof()) {
    std::getline(output1, line1);
    std::getline(output2, line);
    if(line1.compare(line) != 0){
      std::cout << line << "\n" << line1 << std::endl;
      return;
    }
  }
  std::cout << "past test" << std::endl;
  output1.close();
  output2.close();
  return;
}

int main() {
  asio::io_context io_context;
  Sender sender(io_context, "127.0.0.1", "3000");

  // An explanation of the API
  // data_ready(): returns true if a message is available, otherwise false
  // get_msg(): gets an available message
  // request_msg(id): requests a message at id [0, NUM_MSGS)

  // You will need to make significant modifications to the logic below
  // As a starting point, run the server as follows:
  // ./server --no-delay --no-packet-drops which makes the below code work

  // As an example, you could start by requesting the first 10 messages
  std::ofstream output;
  output.open ("../data/output.txt");
  auto startRequest = std::chrono::high_resolution_clock::now();
  int32_t send_base = 0;
  std::string buf[10];
  // The window size is 10
  bool window[10]={false};
  for (int i = 0; i < 10; i++)
    sender.request_msg(i);

  auto start = std::chrono::high_resolution_clock::now();
  // An implementation of Selective Repeat
  while (true) {
    if (sender.data_ready()) {
      // Whenever a message is received, store it in the buffer
      auto msg = sender.get_msg();
      // However, we need to discard the message that is sent before
      if(msg.msg_id < send_base)
        continue;
      buf[msg.msg_id%10] = std::string(msg.data.data(), CHUNK_SIZE);
      window[msg.msg_id%10] = true;
      // When the send_base is received, start sending the next sequence number
      // The next sequence number is (send_base+1)+9 as the window size is 10
      // When send the next seq num request, start clocking
      while(window[send_base%10] == true){
        start = std::chrono::high_resolution_clock::now();
        output << buf[(send_base)%10];
        window[send_base%10] = false;
        send_base++;
        if(send_base+9 < 853)
          sender.request_msg(send_base+9);
        if(send_base == 853)
          goto END;
      }
    }
    // If it takes too long (mean + 2 * std), send request again.
    auto deser_time = std::chrono::high_resolution_clock::now() - start;
    if(std::chrono::duration_cast<std::chrono::milliseconds>(deser_time).count() > 550){
      sender.request_msg(send_base);
      // Restart clocking 
      start = std::chrono::high_resolution_clock::now();
    }
  }
  END:
  output.close();
  auto stopRequest = std::chrono::high_resolution_clock::now() - startRequest;
  std::cout << "Time: " << std::chrono::duration_cast<std::chrono::seconds>(stopRequest).count() << std::endl;
  // Test function
  test();
  return 0;
}
