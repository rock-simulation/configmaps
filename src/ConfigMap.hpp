/*
 *  Copyright 2015, DFKI GmbH Robotics Innovation Center
 *
 *  This file is part of the MARS simulation framework.
 *
 *  MARS is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License
 *  as published by the Free Software Foundation, either version 3
 *  of the License, or (at your option) any later version.
 *
 *  MARS is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public License
 *   along with MARS.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef CONFIG_MAP_HPP
#define CONFIG_MAP_HPP

#ifdef _PRINT_HEADER_
  #warning "ConfigMap.hpp"
#endif

#include <string>

#include "FIFOMap.h"
#include "ConfigItem.hpp"

namespace configmaps {

  // only functions used from misc.h
  std::string getPathOfFile(const std::string &filename);
  std::string trim(const std::string& str);

  class ConfigMap : public ConfigBase,
                    public FIFOMap<std::string, ConfigItem> {
  public:
    ConfigItem& operator[](const std::string &name) {
      ConfigItem &w = FIFOMap<std::string, ConfigItem>::operator[](name);
      
      if(find(name) == end()) {
        w.setParentName(name);
      }
      
      return w;
    }

    ConfigItem& operator[](const char *name) {
      ConfigItem &w = FIFOMap<std::string, ConfigItem>::operator[](name);
      if(find(name) == end()) {
        w.setParentName(name);
      }

      return w;
    }

    bool hasKey(std::string key);
    void toYamlStream(std::ostream &out) const;
    void toYamlFile(const std::string &filename) const;
    std::string toYamlString() const;
    static ConfigMap fromYamlStream(std::istream &in);
    static ConfigMap fromYamlFile(const std::string &filename,
                                  bool loadURI = false);
    static ConfigMap fromYamlString(const std::string &s);
    static void recursiveLoad(ConfigBase *item, std::string path);

    /**
     * @brief Create YAML representation of this ConfigMap to a YAML::Emmitter.
     * @param emitter The output emitter. (parameter gets modified!).
     */
    virtual void dumpToYamlEmitter(YAML::Emitter &emitter) const;

    // checks if the key is in the list, if not return the given default value
    template<typename T> T get(const std::string &key,
                               const T &defaultValue) {
      if(find(key) != end()) {
        return (T)(*this)[key];
      }
      return defaultValue;
    }

    // checks if the key is in the list, if not add the given default value
    // and return it
    template<typename T> T getOrCreate(const std::string &key,
                                       const T &defaultValue) {
      if(find(key) != end()) {
        return (T)(*this)[key];
      }
      (*this)[key] = defaultValue;
      return defaultValue;
    }

  };
} // end of namespace configmaps

#endif // CONFIG_MAP_H
