#include "TableUpdater.hh"
using namespace std;
int WriteTableFile(std::string fname, std::map<std::string, std::string> *m) {
    int count = 0;
    if (m->empty())
        return 0;
 
    FILE *fp = fopen(fname.c_str(), "w");
    if (!fp)
        return -errno;
     
    for(std::map<std::string, std::string>::iterator it = m->begin(); it != m->end(); it++) {
        fprintf(fp, "%s=%s\n", it->first.c_str(), it->second.c_str());
        count++;
    }
     
    fclose(fp);
    return count;
}

int ReadTableFile(std::string fname, std::map<std::string, std::string> *m) {
    int count = 0;
    if (access(fname.c_str(), R_OK) < 0)
        return -errno;
     
    FILE *fp = fopen(fname.c_str(), "r");
    if (!fp)
        return -errno;
     
    m->clear();
     
    char *buf = 0;
    size_t buflen = 0;
     
    while(getline(&buf, &buflen, fp) > 0) {
        char *nl = strchr(buf, '\n');
        if (nl == NULL)
            continue;
        *nl = 0;
         
        char *sep = strchr(buf, '=');
        if (sep == NULL)
            continue;
        *sep = 0;
        sep++;
 
        std::string s1 = buf;
        std::string s2 = sep;
         
        (*m)[s1] = s2;
 
        count++;
    }
 
    if (buf)
        free(buf);
     
    fclose(fp);
    return count;
}
