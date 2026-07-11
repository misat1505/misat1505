#pragma once

#include <vector>

#include "file_server.hpp"
#include "router.hpp"

class HttpConnection {
public:
  HttpConnection(int socket_fd, Router &router, FileServer &file_server);
  void process();

private:
  int _socket_fd;
  Router &_router;
  FileServer &_file_server;

  std::pair<std::vector<char>, ssize_t> await_full_request();
};
