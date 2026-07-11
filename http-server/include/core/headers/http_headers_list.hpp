#pragma once

#include <algorithm>
#include <map>
#include <optional>
#include <string>

namespace HttpHeaders {
namespace Names {
enum Type {
  ContentType,
  ContentLength,
  Authorization,
  Accept,
  UserAgent,
  Host,
  Range,
  ContentRange,
  AcceptRanges
};

inline const std::map<Type, std::string> Strings = {
    {ContentType, "Content-Type"},
    {ContentLength, "Content-Length"},
    {Authorization, "Authorization"},
    {Accept, "Accept"},
    {UserAgent, "User-Agent"},
    {Host, "Host"},
    {Range, "Range"},
    {ContentRange, "Content-Range"},
    {AcceptRanges, "Accept-Ranges"}};

inline std::string to_string(Type h) {
  auto it = Strings.find(h);
  return it != Strings.end() ? it->second : "";
}

inline std::optional<Type> from_string(const std::string &s) {
  for (const auto &[h, str] : Strings) {
    if (str == s)
      return h;
  }
  return std::nullopt;
}
} // namespace Names

namespace Values {

namespace ContentType {
enum Type {
  TextPlain,
  TextHtml,
  TextCss,
  ApplicationJson,
  ApplicationJavascript,
  ImagePng,
  ImageJpeg,
  ImageSvg,
  MultipartFormData,
  VideoMp4
};

inline const std::map<Type, std::string> Strings = {
    {TextPlain, "text/plain"},
    {TextHtml, "text/html"},
    {TextCss, "text/css"},
    {ApplicationJson, "application/json"},
    {ApplicationJavascript, "application/javascript"},
    {ImagePng, "image/png"},
    {ImageJpeg, "image/jpeg"},
    {ImageSvg, "image/svg+xml"},
    {MultipartFormData, "multipart/form-data"},
    {VideoMp4, "video/mp4"}};

inline std::string to_string(Type ct) {
  auto it = Strings.find(ct);
  return it != Strings.end() ? it->second : "";
}

inline std::optional<Type> from_string(const std::string &s) {
  for (const auto &[h, str] : Strings) {
    if (str == s)
      return h;
  }
  return std::nullopt;
}

inline std::optional<Type> from_file_ext(const std::string &ext) {
  std::string lower_ext = ext;
  std::transform(lower_ext.begin(), lower_ext.end(), lower_ext.begin(),
                 ::tolower);

  if (lower_ext == ".html" || lower_ext == ".htm")
    return TextHtml;
  if (lower_ext == ".txt")
    return TextPlain;
  if (lower_ext == ".css")
    return TextCss;
  if (lower_ext == ".json")
    return ApplicationJson;
  if (lower_ext == ".js")
    return ApplicationJavascript;
  if (lower_ext == ".png")
    return ImagePng;
  if (lower_ext == ".jpg" || lower_ext == ".jpeg")
    return ImageJpeg;
  if (lower_ext == ".svg")
    return ImageSvg;
  if (lower_ext == ".form")
    return MultipartFormData;
  if (lower_ext == ".mp4")
    return VideoMp4;

  return std::nullopt;
}
} // namespace ContentType

} // namespace Values

} // namespace HttpHeaders
