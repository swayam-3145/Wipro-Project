#include "scanner.h"
#include "sha256.h"
#include <algorithm>
#include <dirent.h>
#include <sys/stat.h>
#include <cerrno>
#include <cstring>
#ifdef _WIN32
#include <windows.h>
#endif

namespace {
#ifdef _WIN32
void walk(const std::string& root, std::vector<std::string>& out) {
  struct stat st;
  if (stat(root.c_str(), &st) != 0) { out.push_back(root); return; }
  if (!S_ISDIR(st.st_mode)) { out.push_back(root); return; }
  std::string pattern = root + (root.empty() || root[root.size()-1] == '/' || root[root.size()-1] == '\\' ? "" : "\\") + "*";
  WIN32_FIND_DATAA data;
  HANDLE handle = FindFirstFileA(pattern.c_str(), &data);
  if (handle == INVALID_HANDLE_VALUE) { out.push_back(root); return; }
  do {
    std::string name(data.cFileName);
    if (name == "." || name == "..") continue;
    std::string child = root + (root.empty() || root[root.size()-1] == '/' || root[root.size()-1] == '\\' ? "" : "\\") + name;
    if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) walk(child, out);
    else out.push_back(child);
  } while (FindNextFileA(handle, &data));
  FindClose(handle);
}
#else
void walk(const std::string& root, std::vector<std::string>& out) {
  struct stat st;
  if (stat(root.c_str(), &st) != 0) { out.push_back(root); return; }
  if (!S_ISDIR(st.st_mode)) { out.push_back(root); return; }
  DIR* dir = opendir(root.c_str());
  if (!dir) { out.push_back(root); return; }
  struct dirent* ent;
  while ((ent = readdir(dir)) != 0) {
    std::string name(ent->d_name);
    if (name == "." || name == "..") continue;
    std::string child = root + (root.empty() || root[root.size()-1] == '/' ? "" : "/") + name;
    walk(child, out);
  }
  closedir(dir);
}
#endif
}
std::vector<std::string> expand_paths(const std::vector<std::string>& roots) {
  std::vector<std::string> out; for (size_t i=0;i<roots.size();++i) walk(roots[i], out);
  std::sort(out.begin(), out.end()); out.erase(std::unique(out.begin(), out.end()), out.end()); return out;
}
FileRecord inspect_file(const std::string& path) {
  FileRecord r; r.path=path; r.size=0; r.modified=0; r.mode=r.uid=r.gid=0; r.readable=false;
  struct stat st;
  if (stat(path.c_str(), &st) != 0) { r.error=std::strerror(errno); return r; }
  if (!S_ISREG(st.st_mode)) { r.error="not a regular file"; return r; }
  r.size=static_cast<uint64_t>(st.st_size); r.modified=static_cast<int64_t>(st.st_mtime);
  r.mode=static_cast<uint32_t>(st.st_mode & 07777); r.uid=static_cast<uint32_t>(st.st_uid); r.gid=static_cast<uint32_t>(st.st_gid);
  std::string error; r.content_hash=sha256_file(path,error);
  if (!error.empty()) { r.error=error; return r; }
  r.metadata_hash=sha256(path + "|" + std::to_string(r.size) + "|" + std::to_string(r.modified) + "|" + std::to_string(r.mode) + "|" + std::to_string(r.uid) + "|" + std::to_string(r.gid));
  r.readable=true; return r;
}
