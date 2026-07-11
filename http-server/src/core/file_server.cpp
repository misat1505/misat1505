#include "../../include/core/file_server.hpp"

#include <fstream>

void FileServer::set_static_dir(std::filesystem::path dir) {
  _static_dir = dir;
}

void FileServer::set_fallback_files(std::vector<std::string> files) {
  _fallback_files = std::move(files);
}

void FileServer::set_max_chunk_size(size_t size) { _max_chunk_size = size; }

std::optional<std::filesystem::path> FileServer::get_static_dir() const {
  return _static_dir;
}

std::optional<Response> FileServer::try_serve(const Request &request) const {
  auto full_path = this->transform_request_to_filename(request);
  if (!full_path)
    return std::nullopt;

  auto mime_type = this->get_mime_type(*full_path);

  std::ifstream file(*full_path, std::ios::binary | std::ios::ate);
  if (!file)
    return std::nullopt;

  std::streamsize file_size = file.tellg();
  file.seekg(0, std::ios::beg);

  auto range = this->parse_range_header(
      request.get_header(HttpHeaders::Names::Range), file_size);
  std::streamsize chunk_size =
      std::min((range.end - range.start) + 1,
               static_cast<std::streamsize>(_max_chunk_size));

  auto body = this->read_file_chunk(file, range.start, chunk_size);

  return this->make_response(request, body, range, file_size, mime_type);
}

std::optional<std::filesystem::path>
FileServer::transform_request_to_filename(const Request &request) const {
  namespace fs = std::filesystem;

  std::string path = request.get_start_line().get_request_target();
  if (!path.empty() && path[0] == '/')
    path.erase(0, 1);
  if (auto pos = path.find('?'); pos != std::string::npos) {
    path = path.substr(0, pos);
  }

  auto full_path = _static_dir.value() / path;

  if (fs::exists(full_path) && !fs::is_directory(full_path))
    return full_path;

  for (const auto &fallback : _fallback_files) {
    fs::path candidate = _static_dir.value() / fallback;
    if (fs::exists(candidate) && !fs::is_directory(candidate)) {
      return candidate;
    }
  }

  return std::nullopt;
}

std::optional<HttpHeaders::Values::ContentType::Type>
FileServer::get_mime_type(std::filesystem::path filepath) const {
  auto ext = filepath.extension().string();
  return HttpHeaders::Values::ContentType::from_file_ext(ext);
}

FileServer::ByteRange
FileServer::parse_range_header(const std::optional<std::string> &range_header,
                               std::streamsize file_size) const {
  ByteRange result{0, file_size - 1, false};

  if (range_header && range_header->rfind("bytes=", 0) == 0) {
    auto range_spec = range_header->substr(6);
    auto dash_pos = range_spec.find('-');
    if (dash_pos != std::string::npos) {
      std::string start_str = range_spec.substr(0, dash_pos);
      std::string end_str = range_spec.substr(dash_pos + 1);

      if (!start_str.empty())
        result.start = std::stoll(start_str);
      if (!end_str.empty())
        result.end = std::stoll(end_str);
      if (result.end >= file_size)
        result.end = file_size - 1;

      if (result.start <= result.end && result.start < file_size) {
        result.is_partial = true;
      }
    }
  }
  return result;
}

std::string FileServer::read_file_chunk(std::ifstream &file,
                                        std::streamsize start,
                                        std::streamsize chunk_size) const {
  std::vector<char> buffer(chunk_size);
  file.seekg(start, std::ios::beg);
  file.read(buffer.data(), chunk_size);
  return std::string(buffer.begin(), buffer.end());
}

Response FileServer::make_response(
    const Request &request, const std::string &body,
    const FileServer::ByteRange &range, std::streamsize file_size,
    std::optional<HttpHeaders::Values::ContentType::Type> mime_type) const {
  HttpStatus::Code status = range.is_partial ? HttpStatus::Code::PARTIAL_CONTENT
                                             : HttpStatus::Code::OK;

  ResponseStartLine rst(request.get_start_line().get_protocol(), status);
  Response response(rst);

  response.set_body(body);

  response.set_header(
      HttpHeaders::Names::ContentType,
      mime_type ? HttpHeaders::Values::ContentType::to_string(*mime_type)
                : HttpHeaders::Values::ContentType::to_string(
                      HttpHeaders::Values::ContentType::TextPlain));

  response.set_header(HttpHeaders::Names::ContentLength,
                      std::to_string(body.size()));
  response.set_header(HttpHeaders::Names::AcceptRanges, "bytes");

  if (range.is_partial) {
    response.set_header(HttpHeaders::Names::ContentRange,
                        "bytes " + std::to_string(range.start) + "-" +
                            std::to_string(range.end) + "/" +
                            std::to_string(file_size));
  }
  return response;
}
