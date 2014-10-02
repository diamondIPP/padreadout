#include <string>
#include <map>
#include <errno.h>
#include <stdlib.h> 
#include <unistd.h>
#include <fcntl.h>           /* Definition of AT_* constants */
#include <stdio.h>
#include <string.h>
int WriteTableFile(std::string fname, std::map<std::string, std::string> *m);
int ReadTableFile(std::string fname, std::map<std::string, std::string> *m);
