#pragma once

#include "request/request.hpp"
#include "response/response.hpp"

class FileServer {
  struct ByteRange {
    std::streamsize start;
    std::streamsize end;
    bool is_partial;
  };

public:
  void set_static_dir(std::filesystem::path dir);
  void set_fallback_files(std::vector<std::string> files);
  void set_max_chunk_size(size_t size);
  std::optional<Response> try_serve(const Request &request) const;

  std::optional<std::filesystem::path> get_static_dir() const;

private:
  std::optional<std::filesystem::path> _static_dir;
  std::vector<std::string> _fallback_files;
  size_t _max_chunk_size = 5 * 1024 * 1024;

  std::optional<std::filesystem::path>
  transform_request_to_filename(const Request &request) const;
  std::optional<HttpHeaders::Values::ContentType::Type>
  get_mime_type(std::filesystem::path filepath) const;
  ByteRange parse_range_header(const std::optional<std::string> &range_header,
                               std::streamsize file_size) const;
  std::string read_file_chunk(std::ifstream &file, std::streamsize start,
                              std::streamsize chunk_size) const;
  Response make_response(
      const Request &request, const std::string &body, const ByteRange &range,
      std::streamsize file_size,
      std::optional<HttpHeaders::Values::ContentType::Type> mime_type) const;
};
