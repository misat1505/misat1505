#include "../../../include/core/request/form_data.hpp"
#include <gtest/gtest.h>

TEST(FormDataTest, StoresAndRetrievesStringItem) {
  FormData form;
  form.push_item("username", "alice");

  auto value = form.get("username");
  ASSERT_TRUE(value.has_value());
  ASSERT_TRUE(std::holds_alternative<std::string>(*value));
  EXPECT_EQ(std::get<std::string>(*value), "alice");
}

TEST(FormDataTest, StoresAndRetrievesBinaryItem) {
  FormData form;
  FormData::Binary file;
  file.filename = "hello.txt";
  file.content_type = HttpHeaders::Values::ContentType::TextPlain;
  file.data = {'h', 'e', 'l', 'l', 'o'};

  form.push_item("file1", file);

  auto value = form.get("file1");
  ASSERT_TRUE(value.has_value());
  ASSERT_TRUE(std::holds_alternative<FormData::Binary>(*value));

  auto bin = std::get<FormData::Binary>(*value);
  EXPECT_EQ(bin.filename, "hello.txt");
  EXPECT_EQ(bin.content_type, HttpHeaders::Values::ContentType::TextPlain);
  EXPECT_EQ(bin.data.size(), 5);
  EXPECT_EQ(std::string(bin.data.begin(), bin.data.end()), "hello");
}

TEST(FormDataTest, GetReturnsNulloptIfKeyNotFound) {
  FormData form;
  form.push_item("key", "value");

  auto missing = form.get("not_found");
  EXPECT_FALSE(missing.has_value());
}

TEST(FormDataTest, OverwritesExistingKey) {
  FormData form;
  form.push_item("key", "first");
  form.push_item("key", "second");

  auto value = form.get("key");
  ASSERT_TRUE(value.has_value());
  EXPECT_EQ(std::get<std::string>(*value), "second");
}

TEST(FormDataTest, CanStoreMultipleKeys) {
  FormData form;
  form.push_item("user", "bob");

  FormData::Binary img;
  img.filename = "avatar.png";
  img.content_type = HttpHeaders::Values::ContentType::ImagePng;
  img.data = {
      static_cast<char>(0x89), static_cast<char>(0x50), static_cast<char>(0x4E),
      static_cast<char>(0x47) // PNG magic bytes
  };

  form.push_item("avatar", img);

  auto userVal = form.get("user");
  auto avatarVal = form.get("avatar");

  ASSERT_TRUE(userVal.has_value());
  ASSERT_TRUE(avatarVal.has_value());

  EXPECT_EQ(std::get<std::string>(*userVal), "bob");

  auto bin = std::get<FormData::Binary>(*avatarVal);
  EXPECT_EQ(bin.filename, "avatar.png");
  EXPECT_EQ(bin.content_type, HttpHeaders::Values::ContentType::ImagePng);
  EXPECT_EQ(bin.data[0], char(0x89));
}
