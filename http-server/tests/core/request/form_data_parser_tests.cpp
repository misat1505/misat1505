#include "../../../include/core/request/form_data_parser.hpp"
#include <gtest/gtest.h>

std::string make_multipart_body(const std::string &boundary,
                                const std::vector<std::string> &parts) {
  std::string body;
  for (const auto &part : parts) {
    body += "--" + boundary + "\r\n";
    body += part + "\r\n";
  }
  body += "--" + boundary + "--\r\n";
  return body;
}

TEST(FormDataParserTest, ParsesSimpleTextField) {
  std::string boundary = "----testboundary";
  std::string body = make_multipart_body(
      boundary,
      {"Content-Disposition: form-data; name=\"username\"\r\n\r\nJohnDoe"});

  FormDataParser parser(body.c_str(), body.size(), boundary);
  FormData form = parser.parse();

  auto val = form.get("username");
  ASSERT_TRUE(val.has_value());
  ASSERT_TRUE(std::holds_alternative<std::string>(*val));
  EXPECT_EQ(std::get<std::string>(*val), "JohnDoe");
}

TEST(FormDataParserTest, ParsesFileUploadField) {
  std::string boundary = "----testboundary";
  std::string file_content = "PNGDATA";
  std::string body = make_multipart_body(
      boundary, {"Content-Disposition: form-data; name=\"avatar\"; "
                 "filename=\"avatar.png\"\r\n"
                 "Content-Type: image/png\r\n\r\n" +
                 file_content});

  FormDataParser parser(body.c_str(), body.size(), boundary);
  FormData form = parser.parse();

  auto val = form.get("avatar");
  ASSERT_TRUE(val.has_value());
  ASSERT_TRUE(std::holds_alternative<FormData::Binary>(*val));

  const auto &bin = std::get<FormData::Binary>(*val);
  EXPECT_EQ(bin.filename, "avatar.png");
  EXPECT_EQ(bin.content_type, HttpHeaders::Values::ContentType::ImagePng);
  std::string parsed_data(bin.data.begin(), bin.data.end());
  EXPECT_EQ(parsed_data, file_content);
}

TEST(FormDataParserTest, ParsesMultipleFields) {
  std::string boundary = "----testboundary";
  std::string body = make_multipart_body(
      boundary,
      {"Content-Disposition: form-data; name=\"field1\"\r\n\r\nvalue1",
       "Content-Disposition: form-data; name=\"field2\"\r\n\r\nvalue2"});

  FormDataParser parser(body.c_str(), body.size(), boundary);
  FormData form = parser.parse();

  auto v1 = form.get("field1");
  auto v2 = form.get("field2");
  ASSERT_TRUE(v1.has_value());
  ASSERT_TRUE(v2.has_value());
  EXPECT_EQ(std::get<std::string>(*v1), "value1");
  EXPECT_EQ(std::get<std::string>(*v2), "value2");
}

TEST(FormDataParserTest, HandlesEmptyFieldValue) {
  std::string boundary = "----testboundary";
  std::string body = make_multipart_body(
      boundary, {"Content-Disposition: form-data; name=\"empty\"\r\n\r\n"});

  FormDataParser parser(body.c_str(), body.size(), boundary);
  FormData form = parser.parse();

  auto val = form.get("empty");
  ASSERT_TRUE(val.has_value());
  EXPECT_EQ(std::get<std::string>(*val), "");
}

TEST(FormDataParserTest, HandlesBinaryData) {
  std::string boundary = "----testboundary";
  std::vector<char> binary_data = {
      static_cast<char>(0x89), static_cast<char>(0x50), static_cast<char>(0x4E),
      static_cast<char>(0x47)};

  std::string file_content(binary_data.begin(), binary_data.end());

  std::string body =
      make_multipart_body(boundary, {"Content-Disposition: form-data; "
                                     "name=\"file\"; filename=\"test.png\"\r\n"
                                     "Content-Type: image/png\r\n\r\n" +
                                     file_content});

  FormDataParser parser(body.c_str(), body.size(), boundary);
  FormData form = parser.parse();

  auto val = form.get("file");
  ASSERT_TRUE(val.has_value());
  ASSERT_TRUE(std::holds_alternative<FormData::Binary>(*val));

  const auto &bin = std::get<FormData::Binary>(*val);
  EXPECT_EQ(bin.filename, "test.png");
  EXPECT_EQ(bin.content_type, HttpHeaders::Values::ContentType::ImagePng);
  EXPECT_EQ(bin.data, binary_data);
}
