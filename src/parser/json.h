//
// cloriConf json parser header 
// version: 1.0 
// Copyright (C) 2018 James Wei (weijianlhp@163.com). All rights reserved
//
#ifndef CLORIS_LOADER_JSON_H_
#define CLORIS_LOADER_JSON_H_

#include <string> 
#include "config_keeper.h"

namespace cloris {

bool ParseJsonConfig(const std::string& input, bool is_file, const ConfigInserter& handler, std::string* err_msg);

} // namespace cloris

#endif // CLORIS_LOADER_JSON_H_
