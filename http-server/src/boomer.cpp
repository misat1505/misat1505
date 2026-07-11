#include "../include/boomer.hpp"

#include <algorithm>
#include <arpa/inet.h>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <unistd.h>

#include "../include/core/exceptions/boomer_initialization_exception.hpp"
#include "../include/core/http_connection.hpp"
#include "../include/core/request/request.hpp"
#include "../include/core/request/request_parser.hpp"

Boomer::Boomer() {}

void Boomer::run(int port) {
  int server_fd, new_socket;
  struct sockaddr_in address;
  int opt = 1;
  unsigned int backlog_length = 20;
  socklen_t addrlen = sizeof(address);

  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    throw BoomerInitializationException("socket failed");
  }

  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
    close(server_fd);
    throw BoomerInitializationException("setsockopt failed");
  }

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(port);

  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    close(server_fd);
    throw BoomerInitializationException("bind failed");
  }

  if (listen(server_fd, backlog_length) < 0) {
    close(server_fd);
    throw BoomerInitializationException("listen failed");
  }

  std::cout << "HTTP Server running on port " << port << "...\n";

  bool is_running = true;

  while (is_running) {
    if ((new_socket =
             accept(server_fd, (struct sockaddr *)&address, &addrlen)) < 0) {
      perror("accept failed");
      continue;
    }

    std::thread([this, new_socket]() {
      HttpConnection(new_socket, _router, _file_server).process();
    }).detach();
  }
}

void Boomer::serve(std::filesystem::path directory,
                   std::vector<std::string> fallback_files) {
  _file_server.set_static_dir(directory);
  _file_server.set_fallback_files(std::move(fallback_files));
}

void Boomer::get(const std::string &path, const Handler &handler) {
  _router.add_route(path, HttpMethods::Type::GET, handler);
}

void Boomer::post(const std::string &path, const Handler &handler) {
  _router.add_route(path, HttpMethods::Type::POST, handler);
}

void Boomer::put(const std::string &path, const Handler &handler) {
  _router.add_route(path, HttpMethods::Type::PUT, handler);
}

void Boomer::del(const std::string &path, const Handler &handler) {
  _router.add_route(path, HttpMethods::Type::DELETE, handler);
}

void Boomer::patch(const std::string &path, const Handler &handler) {
  _router.add_route(path, HttpMethods::Type::PATCH, handler);
}

void Boomer::options(const std::string &path, const Handler &handler) {
  _router.add_route(path, HttpMethods::Type::OPTIONS, handler);
}

void Boomer::head(const std::string &path, const Handler &handler) {
  _router.add_route(path, HttpMethods::Type::HEAD, handler);
}
