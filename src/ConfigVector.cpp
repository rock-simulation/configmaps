#include "ConfigVector.hpp"
#include <yaml-cpp/yaml.h>
#include <iostream>

using namespace configmaps;

void ConfigVector::dumpToYamlEmitter(YAML::Emitter &emitter) const {

  emitter << YAML::BeginSeq;

  if(!(emitter.good() && 1)) {
    std::string s = getParentName();
    std::cerr << "problem with ConfigVector for: " << std::endl <<  s.c_str() << std::endl;
  }
  assert(emitter.good() && 1);
  for(unsigned int i = 0; i < size(); ++i) {
    const ConfigItem &w = at(i);
    const ConfigBase &item = w;
    item.dumpToYamlEmitter(emitter);
  }
  //if(vec.size() > 1 || do_sequence) {
  emitter << YAML::EndSeq;
  //}
}
