//
// cloriConf joml parser header 
// Copyright 2018 James Wei (weijianlhp@163.com)
// version: 1.0
//

#ifndef CLORIS_LOADER_JOML_H_
#define CLORIS_LOADER_JOML_H_

#include <string> 
#include "config_keeper.h"

namespace cloris {

bool purgeLine(std::string& line, std::string* err_msg, std::vector<std::string>& comments);
bool parseLine(const std::string& buf, std::vector<TraceNode>& vec_trace, const ConfigInserter& handler, std::string* err_msg);

} // namespace cloris

#endif // CLORIS_LOADER_JOML_H_
