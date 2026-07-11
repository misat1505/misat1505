# Boomer - Build your express app in C++

Boomer is a lightweight, Express-inspired HTTP server framework for C++. It wraps raw POSIX sockets in a familiar, declarative routing API, so building an HTTP backend in C++ feels as simple as writing one in Node.js - no boilerplate socket handling, no manual header parsing.

```cpp
Boomer app;

app.get("/text", [](const Request &req, Response &res) {
  res.set_body("hello GET");
});

app.run(8080);
```

---

## Features

- **Express-style routing** - `app.get`, `app.post`, `app.put`, `app.patch`, `app.del`, `app.options`, and `app.head`, each taking a path and a handler.
- **Path parameters** - define routes like `/users/:id` and read them back with `req.get_param("id")`.
- **Query parameters** - read individual params with `req.get_query_param("key")`, or grab the full map via `req.get_query_params()`.
- **Typed request bodies** - the body is a variant that can hold plain text, parsed `nlohmann::json`, or multipart `FormData` (including binary file uploads), so handlers can `std::get<>` the type they expect.
- **Multipart form-data parsing** - including binary fields, so file uploads can be read and written straight to disk.
- **Static file serving** - `app.serve(directory, fallback_files)` serves a directory as-is and falls back to files like `index.html` for SPA-style routing.
- **Range requests / streaming support** - the static file server supports byte-range responses, which is what makes serving and seeking through video files work.
- **Header utilities** - typed helpers for common HTTP headers (e.g. `Content-Type`) instead of raw string juggling.
- **Multithreaded by default** - every accepted connection is handled on its own detached thread.

---

## Quick start

### Prerequisites

- A C++17 (or later) compiler
- [`libuuid`](https://linux.die.net/man/3/libuuid) (used in the example below for generating IDs)
- [`nlohmann/json`](https://github.com/nlohmann/json) for JSON bodies

### Example

```cpp
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
```

Compile against the Boomer headers/sources, run the binary, and the server starts listening on the configured port - request routing, body parsing, and static file serving are all handled for you.

---

## API overview

| Method | Description |
|---|---|
| `app.get(path, handler)` | Register a `GET` route |
| `app.post(path, handler)` | Register a `POST` route |
| `app.put(path, handler)` | Register a `PUT` route |
| `app.patch(path, handler)` | Register a `PATCH` route |
| `app.del(path, handler)` | Register a `DELETE` route |
| `app.options(path, handler)` | Register an `OPTIONS` route |
| `app.head(path, handler)` | Register a `HEAD` route |
| `app.serve(dir, fallbacks)` | Serve a directory of static files, with fallback files for unmatched paths |
| `app.run(port)` | Start listening and accepting connections |

Inside a handler, the `Request` object exposes:

- `get_param(name)` - route/path parameters (e.g. `:id`)
- `get_query_param(name)` / `get_query_params()` - query string parameters
- `get_body()` - a variant over plain text, `nlohmann::json`, and `FormData`

And the `Response` object exposes:

- `set_body(...)` - accepts strings or `nlohmann::json`
- `set_header(name, value)` - with typed helpers for common headers like `Content-Type`

---

## How it works

Under the hood, Boomer is built directly on top of POSIX sockets:

1. A TCP socket is created, bound, and set to listen on the given port.
2. On each accepted connection, a new detached `std::thread` takes over, so slow clients never block the accept loop.
3. Each connection is handed off to an `HttpConnection`, which parses the raw request into a structured `Request`, resolves it against the registered routes (or the static file server), invokes the matching handler, and writes the resulting `Response` back to the socket.

This keeps the core small and readable while still supporting real-world needs like concurrent connections, file uploads, and range-based media streaming.

---

## Project status

Boomer was built as a learning project to explore what it takes to implement an HTTP server - routing, parsing, headers, multipart bodies, and streaming - from raw sockets up, without relying on an existing HTTP library.

## License

This project is licensed under the MIT License - see the `LICENSE` file for details.