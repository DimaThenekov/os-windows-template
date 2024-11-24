// Copyright 2024 dim0n4eg

#include <windows.h>

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <vector>

const size_t BLOCK_SIZE = 8192;

int main(int argc, char* argv[]) {
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
