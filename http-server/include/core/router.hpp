#pragma once

#include <functional>
#include <unordered_map>
#include <vector>

#include "methods/http_methods_list.hpp"
#include "request/request.hpp"
#include "response/response.hpp"

class Router {
  using Handler = std::function<void(const Request &, Response &)>;

public:
  void add_route(const std::string &path, HttpMethods::Type method,
                 const Handler &handler);
  std::optional<Response> resolve(const Request &request) const;

private:
  using MethodsMap = std::unordered_map<HttpMethods::Type, Handler>;
  std::vector<std::pair<std::string, MethodsMap>> _routes;

  std::string
  compile_route(const std::string &path,
                std::vector<std::string> *param_names = nullptr) const;
};
