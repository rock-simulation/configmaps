#include "ConfigBase.hpp"
#include <iostream>
#include <yaml-cpp/yaml.h>
#include <json/json.h>
#include <fstream>

using namespace configmaps;

void ConfigBase::toYamlStream(std::ostream &out) const{
  YAML::Emitter emitter;
  dumpToYamlEmitter(emitter);
  if(!emitter.good()){
    fprintf(stderr, "ERROR: ConfigMap::toYamlStream failed!\n");
    return;
  }
  out << emitter.c_str() << std::endl;
}

void ConfigBase::toYamlFile(const std::string &filename) const{
  std::ofstream f(filename.c_str());
  if(!f.good()){
    fprintf(stderr, "ERROR: ConfigMap::toYamlFile failed! "
            "Could not open output file \"%s\"\n", filename.c_str());
    return;
  }
  toYamlStream(f);
}

std::string ConfigBase::toYamlString() const{
  std::ostringstream sout;
  toYamlStream(sout);
  return sout.str();
}

void ConfigBase::toJsonStream(std::ostream &out) const {
  Json::Value root;
  dumpToJsonValue(root);
  out << root.toStyledString() << std::endl;
}

std::string ConfigBase::toJsonString() const {
  std::ostringstream sout;
  toJsonStream(sout);
  return sout.str();
}
