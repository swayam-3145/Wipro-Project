#pragma once
#include <string>
#include <vector>
#include <cstdint>

struct FileRecord {
  std::string path, content_hash, metadata_hash, error;
  uint64_t size;
  int64_t modified;
  uint32_t mode, uid, gid;
  bool readable;
};
std::vector<std::string> expand_paths(const std::vector<std::string>& roots);
FileRecord inspect_file(const std::string& path);
