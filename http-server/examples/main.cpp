#include "../include/boomer.hpp"

#include <fstream>
#include <iostream>
#include <uuid/uuid.h>

constexpr int PORT = 8080;

std::string generateUUID() {
  uuid_t uuid;
  uuid_generate_random(uuid);
  char str[37];
  uuid_unparse(uuid, str);
  return std::string(str);
}

int main() {
  Boomer app;

  app.serve(std::filesystem::current_path() / "public", {"index.html"});

  app.post("/form-data", [](const Request &req, Response &res) {
    auto body = std::get<FormData>(req.get_body());
    auto name = std::get<std::string>(body.get("name").value());

    auto file = std::get<FormData::Binary>(body.get("file").value());

    std::ofstream out(file.filename, std::ios::binary);
    out.write(file.data.data(), file.data.size());
    out.close();

    res.set_body(name);
  });

  app.get("/queries", [](const Request &req, Response &res) {
    nlohmann::json result;

    for (const auto &[key, value] : req.get_query_params()) {
      result[key] = value;
    }

    res.set_body(result);
  });

  app.get("/users/:id", [](const Request &req, Response &res) {
    std::string id = req.get_param("id").value();
    std::string q = req.get_query_param("q").value();

    res.set_body("user: " + id + "\nq=" + q);
  });

  app.get("/text",
          [](const Request &req, Response &res) { res.set_body("hello GET"); });

  app.post("/text", [](const Request &req, Response &res) {
    res.set_body("hello POST");
  });

  app.get("/html", [](const Request &req, Response &res) {
    res.set_body(std::string(
        "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\" "
        "/><meta name=\"viewport\" content=\"width=device-width, "
        "initial-scale=1.0\" /><title>Document</title></head><body><h2>Hello "
        "world!</h2></body></html>"));
    res.set_header(HttpHeaders::Names::ContentType,
                   HttpHeaders::Values::ContentType::to_string(
                       HttpHeaders::Values::ContentType::TextHtml));
  });

  app.get("/self", [](const Request &req, Response &res) {
    nlohmann::json body = std::get<nlohmann::json>(req.get_body());

    res.set_body(body);
  });

  app.get("/json", [](const Request &req, Response &res) {
    nlohmann::json data;
    data["id"] = generateUUID();
    data["displayName"] = "John";
    data["age"] = 20;
    data["isPremium"] = true;

    res.set_body(data);
  });

  app.run(PORT);

  return 0;
}
