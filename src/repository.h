#pragma once
#include "scanner.h"
#include <string>
#include <vector>

struct AuditEvent { long long id; std::string type,path,old_hash,new_hash,old_meta,new_meta,time,previous_hash,record_hash,message; };
class Repository {
 public:
  explicit Repository(const std::string& home);
  bool load_baseline(std::vector<FileRecord>& records, std::string& error) const;
  bool save_baseline(const std::vector<FileRecord>& records, std::string& error) const;
  bool save_roots(const std::vector<std::string>& roots, std::string& error) const;
  bool load_roots(std::vector<std::string>& roots, std::string& error) const;
  bool append_events(const std::vector<AuditEvent>& events, std::string& error) const;
  bool load_events(std::vector<AuditEvent>& events, std::string& error) const;
  std::string home() const;
 private: std::string home_;
};
