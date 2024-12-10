// Copyright 2024 dim0n4eg

#include <mingw.thread.h>
#include <windows.h>

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <random>
#include <set>

const size_t BLOCK_SIZE = 8192;

int benchmark1(int argc, char* argv[]) {
  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " <filename> <size_in_MB>"
              << std::endl;
    return 1;
  }

  const char* filename = argv[1];
  size_t size_in_MB = std::atoi(argv[2]);
  size_t TOTAL_SIZE = size_in_MB * 1024 * 1024;

  std::vector<char> buffer(BLOCK_SIZE, 'T');

  HANDLE hFile =
      CreateFile(filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
                 FILE_FLAG_WRITE_THROUGH | FILE_FLAG_NO_BUFFERING, NULL);

  if (hFile == INVALID_HANDLE_VALUE) {
    std::cerr << "Error opening file for writing: " << GetLastError()
              << std::endl;
    return 1;
  }

  auto start_time = std::chrono::high_resolution_clock::now();

  size_t total_written = 0;
  DWORD bytes_written;

  while (total_written < TOTAL_SIZE) {
    if (!WriteFile(hFile, buffer.data(), BLOCK_SIZE, &bytes_written, NULL)) {
      std::cerr << "Error writing to file: " << GetLastError() << std::endl;
      CloseHandle(hFile);
      return 1;
    }
    total_written += bytes_written;
  }

  auto end_time = std::chrono::high_resolution_clock::now();
  CloseHandle(hFile);

  std::chrono::duration<double> duration = end_time - start_time;

  double throughput = (TOTAL_SIZE / (1024.0 * 1024.0)) / duration.count();
  std::cout << "Write throughput: " << throughput << " MB/s" << std::endl;

  return 0;
}













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

int benchmark2(int argc, char* argv[]) {
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














void runBenchmark1(int id, double& elapsed) {
    char* argv[] = { "benchmark1", (char *)("output" + std::to_string(id) + ".txt").c_str(), "1024"};
    auto start = std::chrono::high_resolution_clock::now();
    benchmark1(3, argv);
    auto end = std::chrono::high_resolution_clock::now();
    elapsed = std::chrono::duration<double>(end - start).count();
}

void runBenchmark2(int id, int times, double& elapsed) {
    char* argv[] = { "benchmark2", "256", "2"};
    auto start = std::chrono::high_resolution_clock::now();
    benchmark2(3, argv);
    auto end = std::chrono::high_resolution_clock::now();
    elapsed = std::chrono::duration<double>(end - start).count();
}

int main(int argc, char* argv[]) {
    const int n = 2 * 8; // Total number of threads
    const int size_in_MB = 100; // Example size for benchmarks
    const int times = 5; // Example times for benchmark2

    std::vector<std::thread> threads;
    std::vector<double> times_benchmark1(n / 2);
    std::vector<double> times_benchmark2(n / 2);

    for (int i = 0; i < n; ++i) {
        if (i % 2 == 0) {
            // Even index: Run benchmark1
            times_benchmark1[i / 2] = 0.0; // Initialize the time variable
            threads.emplace_back(runBenchmark1, i / 2 + 1, std::ref(times_benchmark1[i / 2]));
        } else {
            // Odd index: Run benchmark2
            times_benchmark2[i / 2] = 0.0; // Initialize the time variable
            threads.emplace_back(runBenchmark2, i / 2 + 1, times, std::ref(times_benchmark2[i / 2]));
        }
    }

    // Wait for all threads to finish
    for (auto& thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }

    // Output results
    for (int i = 0; i < n / 2; ++i) {
        std::cout << "(benchmark1 " << (i + 1) << ", " 
                  << std::fixed
                  << times_benchmark1[i] << ")" << std::endl;
        std::cout << "(benchmark2 " << (i + 1) << ", " 
                  << std::fixed
                  << times_benchmark2[i] << ")" << std::endl;
    }

    return 0;
}