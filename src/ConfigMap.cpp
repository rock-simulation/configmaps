#include "ConfigMap.hpp"

#include <yaml-cpp/yaml.h>
#include <fstream>
#include <sstream>

#include "ConfigAtom.hpp"
#include "ConfigVector.hpp"

//#define VERBOSE

namespace configmaps {

    using std::string;
    using std::endl;

    /************************
     * Implementation
     ************************/

    ConfigMap::ConfigMap(const YAML::Node &n){
        for(YAML::const_iterator it = n.begin(); it != n.end(); ++it){
            std::string key = it->first.as<std::string>();

            if(it->second.IsNull())
                continue;
            //if not null:
            (*this).emplace(key, ConfigItem(it->second));
        }
    }

  ConfigMap::ConfigMap(const Json::Value &v){

    for(Json::Value::const_iterator it = v.begin(); it != v.end(); ++it){

      std::string key = it.name();

      if(it->isNull())
        continue;
      //if not null:
      (*this).emplace(key, ConfigItem(*it));
    }
  }

    ConfigMap ConfigMap::fromYamlStream(std::istream &in){
        ConfigItem item = ConfigItem::fromYamlStream(in);
        if(!item.isMap()){
            throw std::invalid_argument("Given input stream does not have map as root element in YAML!");
        }
        ConfigMap map = item;
        return map;
    }

    ConfigMap ConfigMap::fromYamlFile(const string &filename, bool loadURI){

        ConfigItem item = ConfigItem::fromYamlFile(filename, loadURI);
        if(!item.isMap()){
            throw std::invalid_argument("Given input stream does not have map as root element in YAML!");
        }
        ConfigMap map = item;
        return map;
    }

    ConfigMap ConfigMap::fromYamlString(const string &s){
        std::istringstream sin(s);
        return fromYamlStream(sin);
    }

  ConfigMap ConfigMap::fromJsonStream(std::istream &in){
    ConfigItem item = ConfigItem::fromJsonStream(in);
    if(!item.isMap()){
      throw std::invalid_argument("Given input stream does not have map as root element in YAML!");
    }
    ConfigMap map = item;
    return map;
  }

  ConfigMap ConfigMap::fromJsonString(const string &s){
    std::istringstream sin(s);
    return fromJsonStream(sin);
  }

    void ConfigMap::dumpToYamlEmitter(YAML::Emitter &emitter) const{
        emitter << YAML::BeginMap;
        ConfigMap::const_iterator it;
        for(it = this->begin(); it != this->end(); ++it){
            emitter << YAML::Key << it->first;
            if(!(emitter.good())){
                fprintf(stderr, "problem with ConfigMap for: %s\n", it->first.c_str());
            }
            emitter << YAML::Value;
    #ifdef VERBOSE
            fprintf(stderr, "dump map: %s\n", it->first.c_str());
    #endif
            it->second.dumpToYamlEmitter(emitter);
        }
        emitter << YAML::EndMap;
    }

    void ConfigMap::dumpToJsonValue(Json::Value &root) const{
        ConfigMap::const_iterator it;
        for(it = this->begin(); it != this->end(); ++it){
          Json::Value n;
          it->second.dumpToJsonValue(n);
          root[it->first] = n;
        }
    }

    /***************************
     * static helper functions *
     ***************************/

    // utility functions
    std::string trim(const std::string& str){
        int front_idx, back_idx, len;

        front_idx = 0;
        back_idx = (len = str.size()) - 1;

        while(isspace(str[front_idx]) && front_idx < len)
            front_idx++;
        while(isspace(str[back_idx]) && back_idx > 0)
            back_idx--;

        if(front_idx >= back_idx)
            return "";
        else
            return str.substr(front_idx, back_idx - front_idx + 1);
    }

    bool ConfigMap::hasKey(std::string key){
        return (find(key) != end());
    }

} // end of namespace configmaps
