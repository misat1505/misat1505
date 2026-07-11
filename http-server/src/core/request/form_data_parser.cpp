#include "../../../include/core/request/form_data_parser.hpp"

#include <algorithm>
#include <iostream>
#include <optional>
#include <regex>

std::vector<size_t> find_all_bytes(const char *data, size_t data_len,
                                   const char *pattern, size_t pat_len) {
  std::vector<size_t> positions;

  if (pat_len == 0 || data_len < pat_len)
    return positions;

  const char *start = data;
  const char *end = data + data_len;

  while (true) {
    const char *it = std::search(start, end, pattern, pattern + pat_len);
    if (it == end)
      break;

    positions.push_back(it - data);

    start = it + 1;
  }
  return positions;
}

FormDataParser::FormDataParser(const char *body, size_t body_size,
                               std::string boundary)
    : _body(body), _body_size(body_size), _boundary(boundary) {}

FormData FormDataParser::parse() {
  auto byte_ranges = this->get_form_data_byte_ranges();

  for (const auto &range : byte_ranges) {
    auto start_byte = range.first + 2; // omit \n\r
    auto end_byte = range.second - 4;  // omit \n\r--
    this->process_chunk(start_byte, end_byte);
  }

  return _form_data;
}

std::vector<std::pair<size_t, size_t>>
FormDataParser::get_form_data_byte_ranges() const {
  auto boundaries_positions =
      find_all_bytes(_body, _body_size, _boundary.c_str(), _boundary.size());

  std::vector<std::pair<size_t, size_t>> byte_ranges;

  for (size_t i = 0; i < boundaries_positions.size() - 1; ++i) {
    byte_ranges.push_back({boundaries_positions[i] + _boundary.size(),
                           boundaries_positions[i + 1] - 1});
  }

  return byte_ranges;
}

void FormDataParser::process_chunk(size_t start_byte_id, size_t end_byte_id) {
  const char *chunk = _body + start_byte_id;
  size_t chunk_size = end_byte_id - start_byte_id + 1;

  const char *sep =
      std::search(chunk, chunk + chunk_size, "\r\n\r\n", "\r\n\r\n" + 4);

  std::vector<std::string> headers;
  const char *value_ptr = nullptr;
  size_t value_size = 0;

  if (sep != chunk + chunk_size) {
    const char *line_start = chunk;
    while (line_start < sep) {
      const char *line_end = std::search(line_start, sep, "\r\n", "\r\n" + 2);
      headers.emplace_back(line_start, line_end - line_start);
      line_start = line_end + 2; // move past CRLF
    }

    // value starts after \r\n\r\n
    value_ptr = sep + 4;
    value_size = chunk + chunk_size - value_ptr;
  } else {
    value_ptr = chunk;
    value_size = chunk_size;
  }

  // --- Parse Content-Disposition ---
  std::string name;
  std::optional<std::string> filename;

  if (!headers.empty()) {
    const std::string &cd_line =
        headers[0]; // first line is Content-Disposition

    std::regex re_name(R"(name="([^"]+)\")");
    std::regex re_filename(R"(filename="([^"]+)\")");
    std::smatch match;

    if (std::regex_search(cd_line, match, re_name)) {
      name = match[1];
    }
    if (std::regex_search(cd_line, match, re_filename)) {
      filename = match[1];
    }
  }

  std::optional<HttpHeaders::Values::ContentType::Type> content_type;
  if (headers.size() > 1) {
    const std::string &ct_line = headers[1];
    const std::string prefix = "Content-Type: ";
    if (ct_line.compare(0, prefix.size(), prefix) == 0) {
      content_type = HttpHeaders::Values::ContentType::from_string(
          ct_line.substr(prefix.size()));
    }
  }

  if (content_type.has_value()) {
    // it's a file
    std::vector<char> buffer(value_ptr, value_ptr + value_size);

    FormData::Binary value = {filename.value(), content_type.value(),
                              std::move(buffer)};

    _form_data.push_item(name, value);
  } else {
    // just a string
    std::vector<char> buffer(value_ptr, value_ptr + value_size);
    std::string str_value(buffer.data(), buffer.size());
    _form_data.push_item(name, str_value);
  }
}
