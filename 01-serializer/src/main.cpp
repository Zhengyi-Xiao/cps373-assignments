#include <chrono>
#include <iostream>
#include "../include/bakery.hpp"
using namespace std::chrono;

int main() {
  auto start = high_resolution_clock::now();
  auto bakery = text_deserializer("../data/large.txt");
  auto deser_time = high_resolution_clock::now() - start;
  std::cout << "Text deserializer took: "
            << duration_cast<milliseconds>(deser_time).count() << "ms"
            << std::endl
            << std::endl;
  //print_bakery(bakery);

  start = high_resolution_clock::now();
  text_serializer(bakery,"../data/textLarge.txt");
  deser_time = high_resolution_clock::now() - start;
  std::cout << "Text serializer took: "
            << duration_cast<milliseconds>(deser_time).count() << "ms"
            << std::endl
            << std::endl;
  
  start = high_resolution_clock::now();
  bakery = binary_deserializer("../data/binaryLarge.bin");
  deser_time = high_resolution_clock::now() - start;
  std::cout << "Binary deserializer took: "
            << duration_cast<milliseconds>(deser_time).count() << "ms"
            << std::endl
            << std::endl;
  // To check if the binary is correct, one can print the bakery in text here
  //print_bakery(bakery);
 
  start = high_resolution_clock::now();
  binary_serializer(bakery,"../data/binaryLarge.bin");
  deser_time = high_resolution_clock::now() - start;
  std::cout << "Binary serializer took: "
            << duration_cast<milliseconds>(deser_time).count() << "ms"
            << std::endl
            << std::endl;
  
  return 0;
}