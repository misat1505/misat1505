#include "../../include/core/file_server.hpp"
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>

namespace fs = std::filesystem;

class FileServerTest : public ::testing::Test {
protected:
  fs::path temp_dir;

  void SetUp() override {
    temp_dir = fs::temp_directory_path() / "file_server_test";
    fs::create_directories(temp_dir);

    std::ofstream(temp_dir / "hello.txt") << "Hello world!";
    std::ofstream(temp_dir / "index.html") << "<html>hi</html>";
    std::ofstream(temp_dir / "big.txt") << std::string(100, 'x');
  }

  void TearDown() override { fs::remove_all(temp_dir); }

  Request makeRequest(const std::string &target,
                      HttpMethods::Type method = HttpMethods::Type::GET,
                      const std::optional<std::string> &range = std::nullopt) {
    RequestStartLine start_line(method, target, "HTTP/1.1");

    Headers headers;
    if (range.has_value()) {
      headers.set(HttpHeaders::Names::Range, *range);
    }

    BodyType body = std::monostate{};
    return Request(start_line, headers, body);
  }
};

TEST_F(FileServerTest, ServeExistingFile) {
  FileServer fsrv;
  fsrv.set_static_dir(temp_dir);

  auto req = makeRequest("/hello.txt");
  auto resp = fsrv.try_serve(req);

  ASSERT_TRUE(resp.has_value());
  EXPECT_EQ(resp->get_status_code(), HttpStatus::Code::OK);
  EXPECT_EQ(resp->get_body(), "Hello world!");
  EXPECT_EQ(resp->get_headers().get()[HttpHeaders::Names::ContentType],
            "text/plain");
}

TEST_F(FileServerTest, ServeWithFallbackFile) {
  FileServer fsrv;
  fsrv.set_static_dir(temp_dir);
  fsrv.set_fallback_files({"index.html"});

  auto req = makeRequest("/does-not-exist.txt");
  auto resp = fsrv.try_serve(req);

  ASSERT_TRUE(resp.has_value());
  EXPECT_EQ(resp->get_status_code(), HttpStatus::Code::OK);
  EXPECT_EQ(resp->get_body(), "<html>hi</html>");
  EXPECT_EQ(resp->get_headers().get()[HttpHeaders::Names::ContentType],
            "text/html");
}

TEST_F(FileServerTest, NonExistentFileReturnsNullopt) {
  FileServer fsrv;
  fsrv.set_static_dir(temp_dir);

  auto req = makeRequest("/does-not-exist.txt");
  auto resp = fsrv.try_serve(req);

  EXPECT_FALSE(resp.has_value());
}

TEST_F(FileServerTest, PartialContentWithRangeHeader) {
  FileServer fsrv;
  fsrv.set_static_dir(temp_dir);

  auto req = makeRequest("/hello.txt", HttpMethods::Type::GET, "bytes=0-4");
  auto resp = fsrv.try_serve(req);

  ASSERT_TRUE(resp.has_value());
  EXPECT_EQ(resp->get_status_code(), HttpStatus::Code::PARTIAL_CONTENT);
  EXPECT_EQ(resp->get_body(), "Hello");
  EXPECT_EQ(resp->get_headers().get()[HttpHeaders::Names::ContentRange],
            "bytes 0-4/12");
}

TEST_F(FileServerTest, RangeExceedingFileSizeClamped) {
  FileServer fsrv;
  fsrv.set_static_dir(temp_dir);

  auto req = makeRequest("/hello.txt", HttpMethods::Type::GET, "bytes=0-9999");
  auto resp = fsrv.try_serve(req);

  ASSERT_TRUE(resp.has_value());
  EXPECT_EQ(resp->get_status_code(), HttpStatus::Code::PARTIAL_CONTENT);
  EXPECT_EQ(resp->get_body(), "Hello world!");
  EXPECT_EQ(resp->get_headers().get()[HttpHeaders::Names::ContentRange],
            "bytes 0-11/12");
}

TEST_F(FileServerTest, RespectsMaxChunkSize) {
  FileServer fsrv;
  fsrv.set_static_dir(temp_dir);
  fsrv.set_max_chunk_size(10);

  auto req = makeRequest("/big.txt");
  auto resp = fsrv.try_serve(req);

  ASSERT_TRUE(resp.has_value());
  EXPECT_EQ(resp->get_body().size(), 10u);
}

TEST_F(FileServerTest, GetStaticDirReturnsCorrectPath) {
  FileServer fsrv;
  fsrv.set_static_dir(temp_dir);
  auto dir = fsrv.get_static_dir();

  ASSERT_TRUE(dir.has_value());
  EXPECT_EQ(*dir, temp_dir);
}
