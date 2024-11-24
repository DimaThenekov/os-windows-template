// Copyright 2024 dim0n4eg

#include <iostream>
#include <random>
#include <set>
#include <vector>

std::vector<int> generateRandomData(size_t size) {
  std::vector<int> data;
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, 99);

  for (size_t i = 0; i < size; ++i) {
    data.push_back(dis(gen));
  }
  return data;
}

size_t deduplicate(const std::vector<int>& data) {
  std::set<int> unique_data(data.begin(), data.end());
  return unique_data.size();
}

int main(int argc, char* argv[]) {
  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " <size_in_MB> <times>" << std::endl;
    return 1;
  }

  size_t array_size = std::stoul(argv[1]) / sizeof(int) * 1024 * 1024;
  int times = std::stoi(argv[2]);

  size_t unique_count = 0;
  for (int i = 0; i < times; ++i) {
    std::vector<int> data = generateRandomData(array_size);
    unique_count += deduplicate(data);
  }

  std::cout << ": " << unique_count << std::endl;  // anti-optimization

  return 0;
}
