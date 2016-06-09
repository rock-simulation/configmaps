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



    ConfigMap ConfigMap::fromYamlStream(std::istream &in) {
        ConfigItem item = ConfigItem::fromYamlStream(in);
        if(!item.isMap()){
            throw std::invalid_argument("Given input stream does not have map as root element in YAML!");
        }
        ConfigMap map = item;
        return map;
    }

    ConfigMap ConfigMap::fromYamlFile(const string &filename, bool loadURI) {

        ConfigItem item = ConfigItem::fromYamlFile(filename, loadURI);
        if(!item.isMap()){
            throw std::invalid_argument("Given input stream does not have map as root element in YAML!");
        }
        ConfigMap map = item;
        return map;
    }

    ConfigMap ConfigMap::fromYamlString(const string &s) {
      std::istringstream sin(s);
      return fromYamlStream(sin);
    }


    void ConfigMap::toYamlStream(std::ostream &out) const {
      YAML::Emitter emitter;
      dumpToYamlEmitter(emitter);
      if(!emitter.good()) {
        fprintf(stderr, "ERROR: ConfigMap::toYamlStream failed!\n");
        return;
      }
      out << emitter.c_str() << endl;
    }

    void ConfigMap::toYamlFile(const std::string &filename) const {
      std::ofstream f(filename.c_str());
      if(!f.good()) {
        fprintf(stderr,
                "ERROR: ConfigMap::toYamlFile failed! "
                "Could not open output file \"%s\"\n", filename.c_str());
        return;
      }
      toYamlStream(f);
    }

    std::string ConfigMap::toYamlString() const {
      std::ostringstream sout;
      toYamlStream(sout);
      return sout.str();
    }


    void ConfigMap::dumpToYamlEmitter(YAML::Emitter &emitter) const {
      emitter << YAML::BeginMap;
      ConfigMap::const_iterator it;
      for(it = this->begin(); it != this->end(); ++it) {
        emitter << YAML::Key << it->first;
        if(!(emitter.good())) {
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


    /***************************
     * static helper functions *
     ***************************/

    void ConfigMap::parseFromYamlNode(const YAML::Node &n) {
#ifdef YAML_03_API
      for(YAML::Iterator it = n.begin(); it != n.end(); ++it) {
        std::string s = it.first().to<std::string>();
#ifdef VERBOSE
        fprintf(stderr, "key: %s\n", s.c_str());
#endif
        if(it.second().Type() == YAML::NodeType::Scalar) {
          ConfigAtom atom = parseConfigAtomFromYamlNode(it.second());
          ConfigItem &w = configMap[it.first().to<std::string>()];
#ifdef VERBOSE
          fprintf(stderr, "that: %lx\n", &w);
#endif
          w = atom;
          //configMap[it.first().to<std::string>()] = atom;
        } else if(it.second().Type() == YAML::NodeType::Sequence) {
          configMap[it.first().to<std::string>()] = parseConfigVectorFromYamlNode(it.second());
        } else if(it.second().Type() == YAML::NodeType::Map) {
          configMap[it.first().to<std::string>()] = parseConfigMapFromYamlNode(it.second());
        } else if(it.second().Type() == YAML::NodeType::Null) {
          continue;
        } else {
          fprintf(stderr, "Unknown YAML::NodeType: %d\n", it.second().Type());
          continue;
        }
      }
#else
      for(YAML::const_iterator it = n.begin(); it != n.end(); ++it) {
        std::string key = it->first.as<std::string>();
#ifdef VERBOSE
        fprintf(stderr, "key: %s\n", key.c_str());
#endif

        if(it->second.IsNull())
            continue;
        //if not null:
        this->emplace(key, ConfigItem(it->second));
      }
#endif
    }

    // utility functions
    std::string trim(const std::string& str) {
      int front_idx, back_idx, len;

      front_idx = 0;
      back_idx = ( len = str.size() ) - 1;

      while (isspace(str[front_idx]) && front_idx < len ) front_idx++;
      while (isspace(str[back_idx]) && back_idx > 0 ) back_idx--;

      if ( front_idx >= back_idx )
        return "";
      else
        return str.substr(front_idx, back_idx-front_idx+1);
    }

  bool ConfigMap::hasKey(std::string key) {
    return (find(key) != end());
  }

} // end of namespace configmaps
