#pragma once

#include <string>
#include <ostream>

namespace YAML{
    class Emitter;
}

namespace configmaps {

  class ConfigBase {
  public:
    virtual ~ConfigBase() {}
    ConfigBase(std::string s) : parentName(s) {}
    ConfigBase() : parentName("") {}

    inline void setParentName(std::string s) {
      parentName = s;
    }

    inline const std::string& getParentName() const {
      return parentName;
    }

    virtual void dumpToYamlEmitter(YAML::Emitter &emitter) const = 0;

    void toYamlStream(std::ostream &out) const;
    void toYamlFile(const std::string &filename) const;
    std::string toYamlString() const;

    protected:
      std::string parentName;

  }; // end of class ConfigBase

}
