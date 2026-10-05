#pragma once
#include "repository.h"
#include <string>
#include <vector>
std::string now_utc();
std::vector<AuditEvent> detect(const std::vector<FileRecord>& baseline,const std::vector<FileRecord>& current,const std::vector<AuditEvent>& prior);
bool verify_chain(const std::vector<AuditEvent>& events,std::string& error);
