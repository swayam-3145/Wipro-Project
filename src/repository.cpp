#include "repository.h"
#include <fstream>
#include <cstdlib>
#include <sys/stat.h>
#ifdef _WIN32
#include <direct.h>
#define mkdir(p,m) _mkdir(p)
#endif
namespace {
std::vector<std::string> split(const std::string& s, char delim) { std::vector<std::string> v; std::string x; for(size_t i=0;i<=s.size();++i){if(i==s.size()||s[i]==delim){v.push_back(x);x.clear();}else x+=s[i];} return v; }
std::string clean(std::string x) { for(size_t i=0;i<x.size();++i) if(x[i]=='\t'||x[i]=='\n'||x[i]=='\r') x[i]=' '; return x; }
}
Repository::Repository(const std::string& home):home_(home){mkdir(home_.c_str(),0750);}
std::string Repository::home() const{return home_;}
bool Repository::load_baseline(std::vector<FileRecord>& out,std::string& error) const {
  std::ifstream in((home_+"/baseline.tsv").c_str()); if(!in){error="baseline does not exist";return false;}
  std::string line; std::getline(in,line); while(std::getline(in,line)){auto v=split(line,'\t'); if(v.size()<8)continue; FileRecord r; r.path=v[0];r.content_hash=v[1];r.metadata_hash=v[2];r.size=std::strtoull(v[3].c_str(),0,10);r.modified=std::strtoll(v[4].c_str(),0,10);r.mode=std::strtoul(v[5].c_str(),0,10);r.uid=std::strtoul(v[6].c_str(),0,10);r.gid=std::strtoul(v[7].c_str(),0,10);r.readable=true;out.push_back(r);} return true;
}
bool Repository::save_baseline(const std::vector<FileRecord>& records,std::string& error) const {
  std::ofstream out((home_+"/baseline.tsv").c_str(),std::ios::trunc); if(!out){error="cannot write baseline";return false;}
  out<<"path\tcontent_hash\tmetadata_hash\tsize\tmodified\tmode\tuid\tgid\n"; for(auto&r:records) if(r.readable) out<<clean(r.path)<<"\t"<<r.content_hash<<"\t"<<r.metadata_hash<<"\t"<<r.size<<"\t"<<r.modified<<"\t"<<r.mode<<"\t"<<r.uid<<"\t"<<r.gid<<"\n"; return true;
}
bool Repository::save_roots(const std::vector<std::string>& roots,std::string& error) const {
  std::ofstream out((home_+"/roots.txt").c_str(),std::ios::trunc); if(!out){error="cannot write roots";return false;}
  for(auto& root:roots) out<<clean(root)<<"\n";
  return true;
}
bool Repository::load_roots(std::vector<std::string>& roots,std::string& error) const {
  std::ifstream in((home_+"/roots.txt").c_str()); if(!in){error="roots do not exist";return false;}
  std::string line; while(std::getline(in,line)) if(!line.empty()) roots.push_back(line);
  return true;
}
bool Repository::append_events(const std::vector<AuditEvent>& events,std::string& error) const {
  std::ofstream out((home_+"/events.tsv").c_str(),std::ios::app); if(!out){error="cannot write audit log";return false;}
  for(auto&e:events) {
    out<<e.id<<"\t"<<clean(e.type)<<"\t"<<clean(e.path)<<"\t"<<e.old_hash<<"\t"<<e.new_hash<<"\t"<<e.old_meta<<"\t"<<e.new_meta<<"\t"<<e.time<<"\t"<<e.previous_hash<<"\t"<<e.record_hash<<"\t"<<clean(e.message)<<"\n";
  }
  return true;
}
bool Repository::load_events(std::vector<AuditEvent>& out,std::string& error) const {
  (void)error;
  std::ifstream in((home_+"/events.tsv").c_str()); if(!in)return true; std::string line; while(std::getline(in,line)){auto v=split(line,'\t');if(v.size()<11)continue;AuditEvent e;e.id=std::strtoll(v[0].c_str(),0,10);e.type=v[1];e.path=v[2];e.old_hash=v[3];e.new_hash=v[4];e.old_meta=v[5];e.new_meta=v[6];e.time=v[7];e.previous_hash=v[8];e.record_hash=v[9];e.message=v[10];out.push_back(e);}return true;
}
