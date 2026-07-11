#pragma once

#include <functional>
#include <map>
#include <optional>
#include <string>

#include "./core/file_server.hpp"
#include "./core/methods/http_methods_list.hpp"
#include "./core/request/request.hpp"
#include "./core/response/response.hpp"
#include "./core/router.hpp"

class Boomer {
  using Handler = std::function<void(const Request &, Response &)>;
  using MethodsMap = std::map<HttpMethods::Type, Handler>;
  using Route = std::pair<std::string, MethodsMap>;

public:
  Boomer();
  void run(int port);

  void set_max_chunk_size(size_t size);

  void serve(std::filesystem::path directory,
             std::vector<std::string> fallback_files = {});

  void get(const std::string &path, const Handler &handler);
  void post(const std::string &path, const Handler &handler);
  void put(const std::string &path, const Handler &handler);
  void del(const std::string &path, const Handler &handler);
  void patch(const std::string &path, const Handler &handler);
  void options(const std::string &path, const Handler &handler);
  void head(const std::string &path, const Handler &handler);

private:
  Router _router;
  std::vector<Route> _routes;
  FileServer _file_server;
};
