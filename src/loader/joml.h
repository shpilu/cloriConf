//
// cloriConf joml parser header 
// version: 1.0 
// Copyright (C) 2018 James Wei (weijianlhp@163.com). All rights reserved
//
#ifndef CLORIS_LOADER_JOML_H_
#define CLORIS_LOADER_JOML_H_

#include <string> 
#include "config_keeper.h"

namespace cloris {

void LoadCommentChars(int format, std::vector<std::string>& comments);
bool PurgeLine(std::string& line, std::string* err_msg, std::vector<std::string>& comments);
bool ParseLine(const std::string& buf, std::vector<TraceNode>& vec_trace, const ConfigInserter& handler, std::string* err_msg);

} // namespace cloris

#endif // CLORIS_LOADER_JOML_H_
