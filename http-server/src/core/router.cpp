#include "../../include/core/router.hpp"

#include <cctype>
#include <regex>

void Router::add_route(const std::string &path, HttpMethods::Type method,
                       const Handler &handler) {
  for (auto &route : _routes) {
    if (route.first == path) {
      route.second[method] = handler;
      return;
    }
  }

  MethodsMap methods_map;
  methods_map[method] = handler;
  _routes.emplace_back(path, std::move(methods_map));
}

std::optional<Response> Router::resolve(const Request &request) const {
  std::string path = request.get_start_line().get_request_target();

  if (auto pos = path.find('?'); pos != std::string::npos)
    path = path.substr(0, pos);

  while (!path.empty() && std::isspace(path.back()))
    path.pop_back();
  while (!path.empty() && std::isspace(path.front()))
    path.erase(0, 1);

  auto method = request.get_start_line().get_method();

  for (const auto &route : _routes) {
    std::vector<std::string> param_names;
    std::string regex_str = compile_route(route.first, &param_names);
    std::regex r(regex_str);

    std::smatch match;
    if (std::regex_match(path, match, r)) {
      Request::Params params;
      for (size_t i = 0; i < param_names.size(); i++) {
        if (i + 1 < match.size())
          params[param_names[i]] = match[i + 1].str();
      }

      Request req_with_params(request.get_start_line(), request.get_headers(),
                              request.get_body(), params,
                              request.get_query_params());

      auto it = route.second.find(method);
      if (it == route.second.end())
        continue;

      Response response(ResponseStartLine(
          request.get_start_line().get_protocol(), HttpStatus::Code::OK));
      it->second(req_with_params, response);
      return response;
    }
  }

  return std::nullopt;
}

std::string Router::compile_route(const std::string &path,
                                  std::vector<std::string> *param_names) const {
  std::string regex_str = R"(^)";

  for (size_t i = 0; i < path.size();) {
    if (path[i] == ':') {
      size_t j = i + 1;
      while (j < path.size() && std::isalnum(path[j]))
        j++;
      std::string param_name = path.substr(i + 1, j - (i + 1));
      if (param_names)
        param_names->push_back(param_name);
      regex_str += "([^/]+)";
      i = j;
    } else {
      static const std::string specials = R"(.^$|()[]*+?{}\\)";
      if (specials.find(path[i]) != std::string::npos)
        regex_str += '\\';
      regex_str += path[i];
      i++;
    }
  }

  regex_str += "/?$";
  return regex_str;
}
