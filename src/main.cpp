#include "audit.h"
#include "scanner.h"
#include <cstdlib>
#include <iostream>
#include <map>

static void usage(){std::cout<<"fiaudit init <path>... | scan | status | report [--json] | verify-log\n";}
static Repository repo(){const char* e=std::getenv("FIAUDIT_HOME");return Repository(e?e:".fiaudit");}
int main(int argc,char**argv){
 if(argc<2){usage();return 2;} Repository r=repo();std::string err;std::string cmd=argv[1];
 if(cmd=="init"){if(argc<3){std::cerr<<"init needs a path\n";return 2;}std::vector<std::string> roots;for(int i=2;i<argc;++i)roots.push_back(argv[i]);auto paths=expand_paths(roots);std::vector<FileRecord> records;for(auto&p:paths){auto f=inspect_file(p);if(!f.readable)std::cerr<<"warning: "<<p<<": "<<f.error<<"\n";records.push_back(f);}if(!r.save_baseline(records,err)||!r.save_roots(roots,err)){std::cerr<<err<<"\n";return 1;}std::cout<<"Baseline created for "<<records.size()<<" path(s).\n";return 0;}
 if(cmd=="scan"){std::vector<FileRecord>b;if(!r.load_baseline(b,err)){std::cerr<<err<<". Run init first.\n";return 1;}std::vector<std::string> roots;if(!r.load_roots(roots,err)){std::cerr<<err<<". Re-run init.\n";return 1;}auto paths=expand_paths(roots);std::vector<FileRecord>c;for(auto&p:paths)c.push_back(inspect_file(p));std::vector<AuditEvent>old;r.load_events(old,err);auto events=detect(b,c,old);if(!r.append_events(events,err)){std::cerr<<err<<"\n";return 1;}std::cout<<"Scanned "<<c.size()<<" path(s); "<<events.size()<<" change(s) detected.\n";for(auto&e:events)std::cout<<e.type<<": "<<e.path<<"\n";return events.empty()?0:3;}
 if(cmd=="status"){std::vector<FileRecord>b;if(!r.load_baseline(b,err)){std::cerr<<err<<"\n";return 1;}std::vector<AuditEvent>e;r.load_events(e,err);std::cout<<"Repository: "<<r.home()<<"\nBaseline files: "<<b.size()<<"\nAudit events: "<<e.size()<<"\n";return 0;}
 if(cmd=="report"){std::vector<AuditEvent>e;r.load_events(e,err);bool json=argc>2&&std::string(argv[2])=="--json";if(json){std::cout<<"[\n";for(size_t i=0;i<e.size();++i)std::cout<<"  {\"id\":"<<e[i].id<<",\"type\":\""<<e[i].type<<"\",\"path\":\""<<e[i].path<<"\",\"time\":\""<<e[i].time<<"\",\"record_hash\":\""<<e[i].record_hash<<"\"}"<<(i+1<e.size()?",":"")<<"\n";std::cout<<"]\n";}else for(auto&x:e)std::cout<<x.id<<" "<<x.time<<" "<<x.type<<" "<<x.path<<" "<<x.message<<"\n";return 0;}
 if(cmd=="verify-log"){std::vector<AuditEvent>e;r.load_events(e,err);if(!verify_chain(e,err)){std::cerr<<"INVALID: "<<err<<"\n";return 1;}std::cout<<"Audit chain valid ("<<e.size()<<" event(s)).\n";return 0;}
 usage();return 2;
}
