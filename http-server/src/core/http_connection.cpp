#include "../../include/core/http_connection.hpp"

#include <arpa/inet.h>
#include <iostream>
#include <unistd.h>

#include "../../include/core/request/request_parser.hpp"

HttpConnection::HttpConnection(int socket_fd, Router &router,
                               FileServer &file_server)
    : _socket_fd(socket_fd), _router(router), _file_server(file_server) {}

void HttpConnection::process() {
  struct timeval timeout {
    2, 0
  };
  setsockopt(_socket_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

  auto [buffer, total_read] = this->await_full_request();

  if (total_read <= 0) {
    close(_socket_fd);
    return;
  }

  RequestParser parser(buffer.data(), total_read);
  Request request = parser.parse();

  auto response = _router.resolve(request);
  std::string response_str;

  if (response.has_value()) {
    response_str = (*response).to_string();
  } else if (_file_server.get_static_dir().has_value()) {
    auto serve_response = _file_server.try_serve(request);
    if (serve_response.has_value())
      response_str = (*serve_response).to_string();
  }

  if (response_str.empty()) {
    response_str =
        "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\nConnection: "
        "close\r\nContent-Length: 13\r\n\r\n404 Not Found";
  }

  send(_socket_fd, response_str.data(), response_str.size(), 0);
  close(_socket_fd);
}

std::pair<std::vector<char>, ssize_t> HttpConnection::await_full_request() {
  constexpr size_t initial_buffer_size = 5 * 1024;           // 5 KB
  constexpr size_t max_request_size = 1024ull * 1024 * 1024; // 1 GB

  std::vector<char> buffer(initial_buffer_size);

  ssize_t total_read = 0;
  ssize_t bytes_read = 0;

  while ((bytes_read = read(_socket_fd, buffer.data() + total_read,
                            buffer.size() - total_read)) > 0) {
    total_read += bytes_read;

    std::string req(buffer.data(), total_read);
    auto headers_end = req.find("\r\n\r\n");
    if (headers_end != std::string::npos) {
      auto pos = req.find("Content-Length:");
      if (pos == std::string::npos) {
        return {buffer, total_read}; // no Content-Length -> no body
      }

      auto end = req.find("\r\n", pos);
      size_t content_length =
          std::stoul(req.substr(pos + 15, end - (pos + 15)));

      size_t expected_size = headers_end + 4 + content_length;
      if (total_read >= (ssize_t)expected_size) {
        return {buffer, total_read}; // got full request
      }

      // grow up buffer but don’t exceed max_request_size
      if (expected_size > buffer.size()) {
        size_t new_size = std::min(std::max(buffer.size() * 2, expected_size),
                                   max_request_size);
        buffer.resize(new_size);
      }
    }

    // Safety stop
    if ((size_t)total_read >= max_request_size) {
      return {buffer, total_read};
    }

    if ((size_t)total_read >= buffer.size()) {
      size_t new_size = std::min(buffer.size() * 2, max_request_size);
      buffer.resize(new_size);
    }
  }

  return {buffer, total_read};
}
