#include "audit.h"
#include "sha256.h"
#include <ctime>
#include <map>
#include <sstream>
std::string now_utc(){std::time_t t=std::time(0);char b[32];std::strftime(b,sizeof(b),"%Y-%m-%dT%H:%M:%SZ",std::gmtime(&t));return b;}
std::vector<AuditEvent> detect(const std::vector<FileRecord>& base,const std::vector<FileRecord>& cur,const std::vector<AuditEvent>& prior){
 std::map<std::string,FileRecord> b,c;for(auto&r:base)b[r.path]=r;for(auto&r:cur)c[r.path]=r;std::vector<AuditEvent> out;long long id=prior.empty()?1:prior.back().id+1;std::string prev=prior.empty()?"GENESIS":prior.back().record_hash;
 for(auto& p:b){auto it=c.find(p.first);AuditEvent e;e.id=id++;e.path=p.first;e.time=now_utc();e.old_hash=p.second.content_hash;e.old_meta=p.second.metadata_hash;e.new_hash=it==c.end()?"":it->second.content_hash;e.new_meta=it==c.end()?"":it->second.metadata_hash;
  if(it==c.end() || !it->second.readable)e.type=it==c.end()?"DELETED":"ERROR";else if(p.second.content_hash!=it->second.content_hash)e.type="MODIFIED";else if(p.second.mode!=it->second.mode)e.type="PERMISSION_CHANGED";else if(p.second.uid!=it->second.uid||p.second.gid!=it->second.gid)e.type="OWNER_CHANGED";else continue;
  e.message=it==c.end()?"file missing":(it->second.error.empty()?"integrity mismatch":it->second.error);e.previous_hash=prev;e.record_hash=sha256(std::to_string(e.id)+"|"+e.type+"|"+e.path+"|"+e.old_hash+"|"+e.new_hash+"|"+e.old_meta+"|"+e.new_meta+"|"+e.time+"|"+e.previous_hash);prev=e.record_hash;out.push_back(e);}
 for(auto& p:c) {
  if(!b.count(p.first)) {
   AuditEvent e;e.id=id++;e.type=p.second.readable?"ADDED":"ERROR";e.path=p.first;e.new_hash=p.second.content_hash;e.new_meta=p.second.metadata_hash;e.time=now_utc();e.message=p.second.readable?"new file":p.second.error;e.previous_hash=prev;
   e.record_hash=sha256(std::to_string(e.id)+"|"+e.type+"|"+e.path+"|"+e.old_hash+"|"+e.new_hash+"|"+e.old_meta+"|"+e.new_meta+"|"+e.time+"|"+e.previous_hash);
   prev=e.record_hash;out.push_back(e);
  }
 }
 return out;
}
bool verify_chain(const std::vector<AuditEvent>& ev,std::string& error){std::string prev="GENESIS";for(auto&e:ev){std::string expected=sha256(std::to_string(e.id)+"|"+e.type+"|"+e.path+"|"+e.old_hash+"|"+e.new_hash+"|"+e.old_meta+"|"+e.new_meta+"|"+e.time+"|"+e.previous_hash);if(e.previous_hash!=prev||e.record_hash!=expected){error="chain failure at event "+std::to_string(e.id);return false;}prev=e.record_hash;}return true;}
