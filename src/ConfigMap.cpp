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

  static ConfigAtom parseConfigAtomFromYamlNode(const YAML::Node &n);
  static ConfigVector parseConfigVectorFromYamlNode(const YAML::Node &n);
  static ConfigMap parseConfigMapFromYamlNode(const YAML::Node &n);



    ConfigMap ConfigMap::fromYamlStream(std::istream &in) {
#ifdef YAML_03_API
      YAML::Parser parser(in);
      YAML::Node doc, node;
      while(parser.GetNextDocument(doc)) {
        if(doc.Type() == YAML::NodeType::Map) {
          return parseConfigMapFromYamlNode(doc);
        } else {
#else
      YAML::Node node = YAML::Load(in);
      {
        if(!node.IsMap()) {
#endif
          fprintf(stderr,
                  "ConfigData::ConfigMapFromYaml currently only supports "
                  "mapping types at the root level. please contact the "
                  "developers if you need support for other types.\n");
          return ConfigMap();
        }
      }
#ifdef YAML_03_API
      // if there is no valid document return a empty ConfigMap
      return ConfigMap();
#else
      return parseConfigMapFromYamlNode(node);
#endif
    }

    ConfigMap ConfigMap::fromYamlFile(const string &filename, bool loadURI) {
      std::ifstream fin(filename.c_str());
      ConfigMap map = fromYamlStream(fin);
      if(loadURI) {
        std::string pathToFile = getPathOfFile(filename);
        recursiveLoad(&map, pathToFile);
      }
      return map;
    }

    void ConfigMap::recursiveLoad(ConfigBase *item, std::string path) {
      ConfigMap *map = dynamic_cast<ConfigMap*>(item);
      if(map) {
        ConfigMap::iterator it = map->begin();
        std::list<ConfigMap::iterator> eraseList;
        std::list<ConfigMap::iterator>::iterator eraseIt;
        for(; it!=map->end(); ++it) {
          if(it->first == "URI") {
            /*
              fprintf(stderr, "ConfigMap::recursiveLoad: found uri: %s\n",
              it->second[0].getString().c_str());
            */
            std::string file = path + (std::string)it->second;
            std::string subPath = getPathOfFile(file);
            ConfigMap m2 = fromYamlFile(file, true);
            recursiveLoad(&m2, subPath);
            map->append(m2);
            eraseList.push_back(it);
          }
          else {
            recursiveLoad(&((ConfigBase&)it->second), path);
          }
        }
        for(eraseIt=eraseList.begin(); eraseIt!=eraseList.end(); ++eraseIt) {
          map->erase(*eraseIt);
        }
      }
      else {
        ConfigVector *v = dynamic_cast<ConfigVector*>(item);
        if(v) {
          ConfigVector::iterator it = v->begin();
          for(; it!= v->end(); ++it) {
            recursiveLoad(&((ConfigBase&)*it), path);
          }
        }
      }
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


    static ConfigAtom parseConfigAtomFromYamlNode(const YAML::Node &n) {
      ConfigAtom item;
      if(n.Type() == YAML::NodeType::Scalar) {
#ifdef YAML_03_API
        std::string s;
        n.GetScalar(s);
        item.setUnparsedString(s);
#else
        item.setUnparsedString(n.Scalar());
#endif
#ifdef VERBOSE
        fprintf(stderr, "parsed atom: %s\n", s.c_str());
#endif
      }
      return item;
    }

    static ConfigVector parseConfigVectorFromYamlNode(const YAML::Node &n) {
      ConfigVector vec;
      if(n.Type() == YAML::NodeType::Sequence) {
#ifdef YAML_03_API
        YAML::Iterator it;
#else
        YAML::const_iterator it;
#endif
        ConfigItem item;
          for(it = n.begin(); it != n.end(); ++it) {
          if(it->Type() == YAML::NodeType::Scalar) {
            ConfigAtom atom = parseConfigAtomFromYamlNode(*it);
            item = atom;
          } else if(it->Type() == YAML::NodeType::Sequence) {
            item = parseConfigVectorFromYamlNode(*it);
          } else if(it->Type() == YAML::NodeType::Map) {
            item = parseConfigMapFromYamlNode(*it);
          }
          vec.push_back(item);
        }
      }
      return vec;
    }

    static ConfigMap parseConfigMapFromYamlNode(const YAML::Node &n) {
      ConfigMap configMap;
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

        if(it->second.IsScalar()) {
          ConfigAtom atom = parseConfigAtomFromYamlNode(it->second);
          ConfigItem &w = configMap[key];
#ifdef VERBOSE
          fprintf(stderr, "that: %lx\n", &w);
#endif
          w = atom;
        } else if(it->second.IsSequence()) {
          configMap[key] = parseConfigVectorFromYamlNode(it->second);
        } else if(it->second.IsMap()) {
          configMap[key] = parseConfigMapFromYamlNode(it->second);
        } else if(it->second.IsNull()) {
          continue;
        } else {
          fprintf(stderr, "Unknown YAML::NodeType: %d\n", it->second.Type());
          continue;
        }
      }
#endif
      return configMap;
    }


//    static void dumpConfigItemToYaml(YAML::Emitter &emitter,
//                                     const ConfigBase &configItem) {
//      //std::string s = ((const ConfigAtom*)&configItem)->toString();
//#ifdef VERBOSE
//      fprintf(stderr, "try: %s\n", configItem.getParentName().c_str());
//#endif
//      const ConfigAtom *a = dynamic_cast<const ConfigAtom*>(&configItem);
//      if(a) {
//#ifdef VERBOSE
//        fprintf(stderr, "have atom\n");
//#endif
//        dumpConfigAtomToYaml(emitter, *a);
//      }
//      else {
//        const ConfigVector *v = dynamic_cast<const ConfigVector*>(&configItem);
//        if(v) {
//#ifdef VERBOSE
//          fprintf(stderr, "have vector\n");
//#endif
//          dumpConfigVectorToYaml(emitter, *v);
//        }
//        else {
//          const ConfigMap *m = dynamic_cast<const ConfigMap*>(&configItem);
//          if(m) {
//#ifdef VERBOSE
//            fprintf(stderr, "have map\n");
//#endif
//            dumpConfigMapToYaml(emitter, *m);
//          }
//        }
//      }
//    }

    // utility functions
    std::string getPathOfFile(const std::string &filename) {
      std::string path = "./";
      size_t pos;

      if((pos = filename.rfind('/')) != std::string::npos) {
        path = filename.substr(0, pos+1);
      }
      return path;
    }

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
