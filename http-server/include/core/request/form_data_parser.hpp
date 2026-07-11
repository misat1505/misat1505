#pragma once

#include <string>
#include <vector>

#include "form_data.hpp"

class FormDataParser {
public:
  FormDataParser(const char *body, size_t body_size, std::string boundary);
  FormData parse();

private:
  FormData _form_data;
  size_t _body_size;
  const char *_body;
  std::string _boundary;

  std::vector<std::pair<size_t, size_t>> get_form_data_byte_ranges() const;
  void process_chunk(size_t start_byte_id, size_t end_byte_id);
};