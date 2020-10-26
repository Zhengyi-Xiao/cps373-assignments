#include "bakery.hpp"
#include <fstream>
#include <iostream>
#include <bitset>          
#include <sstream> 

void print_bakery(const Bakery& bakery) {
  std::cout << "Employees: " << std::endl;
  for (auto employee : bakery.employees) {
    std::cout << employee << std::endl;
  }

  std::cout << std::endl;
  std::cout << "Items: " << std::endl;
  for (auto item : bakery.items) {
    std::cout << item.name << ", " << item.price << std::endl;
  }

  std::cout << std::endl;
  std::cout << "Orders: " << std::endl;
  for (auto order : bakery.orders) {
    std::cout << order.employee << ": ";
    auto j = 0;
    for (auto item : order.items) {
      std::cout << item.second << " " << item.first;
      j++;
      if (size_t(j) < order.items.size())
        std::cout << ", ";
    }
    std::cout << std::endl;
  }
}

// You shouldn't need to edit this function (unless you want!)
Bakery text_deserializer(std::string file_path) {
  std::ifstream infile(file_path);
  std::string line;
  Bakery bakery;

  while (!infile.eof()) {
    // Employees section
    if (line.compare("@employees") == 0) {
      std::getline(infile, line);
      while (line.size() > 0) {
        bakery.employees.push_back(line);
        std::getline(infile, line);
      }
    }

    // Items section
    if (line.compare("@items") == 0) {
      std::getline(infile, line);
      while (line.size() > 0) {
        auto end = line.find(", ");
        Item item;
        item.name = line.substr(0, end);
        item.price = line.substr(end + 2);
        bakery.items.push_back(item);
        std::getline(infile, line);
      }
    }

    // Orders section
    if (line.compare("@orders") == 0) {
      std::getline(infile, line);
      auto e = bakery.employees;
      while (line.size() > 0) {
        Order order;
        auto end = line.find(": ");
        order.employee = line.substr(0, end);

        // Find all the orders
        auto rest = line.substr(end + 2);
        size_t pos = 0;
        std::string token;
        while ((pos = rest.find(", ")) != std::string::npos) {
          token = rest.substr(0, pos);
          end = token.find(" ");
          auto quantity = token.substr(0, end);
          auto item_name = token.substr(end + 1);
          order.items.push_back(std::make_pair(item_name, quantity));
          rest.erase(0, pos + 2);
        }
        end = rest.find(" ");
        auto quantity = rest.substr(0, end);
        auto item_name = rest.substr(end + 1);
        order.items.push_back(std::make_pair(item_name, quantity));
        bakery.orders.push_back(order);

        // no more lines to read
        if (infile.eof())
          break;

        std::getline(infile, line);
      }
    }

    std::getline(infile, line);
  }

  return bakery;
}

using namespace std;
// Implement these 3 functions!
// Hint: use print_bakery to help think about the text serializer
void text_serializer(const Bakery& bakery, std::string file_path){
  ofstream output;
  output.open (file_path);
  output << "@Employees\n";
  for (auto employee : bakery.employees)
    output << employee << "\n";
  
  output << "\n@Items\n";
  for (auto item : bakery.items)
    output << item.name << ", " << item.price << "\n";

  output << "\n@Orders\n";
  for (auto order : bakery.orders) {
    output << order.employee << ": ";
    auto j = 0;
    for (auto item : order.items) {
      output << item.second << " " << item.first;
      j++;
      if (size_t(j) < order.items.size())
        output << ", ";
    }
    output << "\n";
  }

  output.close();
}

Bakery binary_deserializer(std::string file_path) {
  ifstream infile (file_path, ios::in | ios::binary);
  std::string line;
  Bakery bakery;
  // Employees and Items are the mapping vectors
  // For example {Brad, Claudia, Simone} maps to {0,1,2}, the index of employees
  vector<string> employees;
  vector<string> items;
  while (!infile.eof()) {
    // Employees section
    if (line.compare("@Employees") == 0){
      getline(infile, line);
      while (line.size() > 0) {
        bakery.employees.push_back(line);
        employees.push_back(line);
        getline(infile, line);
      }
    }
    // Items section
    if (line.compare("@Items") == 0) {
      getline(infile, line);
      while (line.size() > 0) {
        auto end = line.find(", ");
        Item item;
        item.name = line.substr(0, end);
        item.price = line.substr(end + 2);
        bakery.items.push_back(item);
        items.push_back(item.name);
        getline(infile, line);
      }
    }

    // Orders section
    // Orders are stored in the format of following: 
    // Index of employee (Quantities, Index of Items) (Quantities, Index of Items)...
    // Where (Quantities, Index of Items) is 1 byte in which the higher 4 bits are for quanitities
    // and the lower 4 bits are for the index of items. 
    if (line.compare("@Orders") == 0) {
      getline(infile, line);
      while(line.size() > 0){
        Order order;
        for(int i=0;i<line.size();i++){
          int byte = abs(line[i]);
          // If byte is smaller than 5, then it must be a employee since if it is (quantities, index)
          // then byte must bigger than 15. Because quantities is a positive interger, then the least 
          // it can be is 0001 0000 = 16. 5 is a random choice, btw. 
          if (byte < 5)
            if(order.employee.empty())
              order.employee = employees[byte];
            else
              break;
          else{
            string quantity = to_string(byte >> 4); // Get the higher 4 bits in int then convert to a string
            string item_name = items[(byte & 15)];  // Get the lower 4 bits in int then find the corresponding item of the index 
            order.items.push_back(std::make_pair(item_name, quantity));
          }
        }
        bakery.orders.push_back(order);
        if (infile.eof())
          break;
        getline(infile, line);
      } 
      getline(infile, line);
    }
    getline(infile, line);
  }
  return bakery;
}

void binary_serializer(const Bakery& bakery, std::string file_path){
  ofstream output (file_path);
  output << "@Employees\n";
  vector<string> employees;
  vector<string> items;
  // Employees and Items are the mapping vectors
  // For example {Brad, Claudia, Simone} maps to {0,1,2}, the index of employees
  for (auto employee : bakery.employees){
    output << employee << "\n";
    employees.push_back(employee);
  }
  
  output << "\n@Items\n";
  for (auto item : bakery.items){
    output << item.name << ", " << item.price << "\n";
    items.push_back(item.name);
  }
  
  output << "\n@Orders\n";
  for (auto order : bakery.orders) {
    int k = 0;
    // Find the index of employee and store it in 1 byte
    for (; k < employees.size(); k++)
        if (employees[k].compare(order.employee) == 0)
          break;
    output << (char)k;
    auto j = 0;
    for (auto item : order.items) {
      int i = 0;
      // find the index of item's name, then store (Quantities, Index of Items) in 1 byte
      // where the higher 4 bits are for quanitities and the lower 4 bits are for the index of items. 
      for (; i < items.size(); i++)
        if (items[i].compare(item.first) == 0)
          break;
      stringstream geek(item.second); 
      int x = 0; 
      geek >> x; 
      output << (char)((x<<4) + i);
      j++;
    }
    output  << "\n";
  }
  output << "\n";
  output.close();
}
