#include "ConfigVector.hpp"
#include <yaml-cpp/yaml.h>
#include <json/json.h>
#include <iostream>

using namespace configmaps;

ConfigVector::ConfigVector(const YAML::Node &n) {

  if(n.Type() != YAML::NodeType::Sequence){
    throw std::runtime_error("Failed to create config vector, given YAML::Node is not a sequence!");
  }

  //    YAML::const_iterator it;
  for(auto &it : n){ //it = n.begin(); it != n.end(); ++it){
    ConfigItem item(it);
    push_back(item);
  }
}

ConfigVector::ConfigVector(const Json::Value &v) {
  if(!v.isArray()){
    throw std::runtime_error("Failed to create config vector, given Json::Value is not a sequence!");
  }

  for(int i=0; i<v.size(); ++i) {
    ConfigItem item(v[i]);
    push_back(item);
  }
}

void ConfigVector::dumpToYamlEmitter(YAML::Emitter &emitter) const {

  emitter << YAML::BeginSeq;

  if(!(emitter.good() && 1)){
    std::string s = getParentName();
    std::cerr << "problem with ConfigVector for: " << std::endl << s.c_str() << std::endl;
  }
  assert(emitter.good() && 1);
  for(unsigned int i = 0; i < size(); ++i){
    const ConfigItem &w = at(i);
    const ConfigBase &item = w;
    item.dumpToYamlEmitter(emitter);
  }
  //if(vec.size() > 1 || do_sequence) {
  emitter << YAML::EndSeq;
  //}
}

void ConfigVector::dumpToJsonValue(Json::Value &root) const {

  for(unsigned int i = 0; i < size(); ++i){
    const ConfigItem &w = at(i);
    const ConfigBase &item = w;
    Json::Value n;
    item.dumpToJsonValue(n);
    root.append(n);
  }
}
