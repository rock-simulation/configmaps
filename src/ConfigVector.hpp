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

#ifndef CONFIG_VECTOR_HPP
#define CONFIG_VECTOR_HPP

#ifdef _PRINT_HEADER_
#warning "ConfigVector.hpp"
#endif

#include <string>
#include <vector>

#include "ConfigItem.hpp"
#include "ConfigBase.hpp"

//forwards:
namespace YAML {
  class Node;
}

namespace Json {
  class Value;
}

namespace configmaps {

  class ConfigVector : public ConfigBase,
                       public std::vector<ConfigItem> {
  public:

    ConfigVector(std::string s) : ConfigBase(s) {}
    ConfigVector() {}
    /**
     * @brief Create and fill the object with values from given YAML::Node.
     * @param n The YAML::Node containing the serialized data for this object.
     */
    ConfigVector(const YAML::Node &n);

    /**
     * @brief Create and fill the object with values from given Json::Value.
     * @param v The Json::Value containing the serialized data for this object.
     */
    ConfigVector(const Json::Value &n);

    size_t append(const ConfigItem &item) {
      this->push_back(item);
      this->back().setParentName(parentName);
      return this->size() - 1;
    }

    ConfigVector& operator<<(const ConfigItem &item) {
      this->push_back(item);
      this->back().setParentName(parentName);
      return *this;
    }

    ConfigVector& operator+=(const ConfigItem &item) {
      this->push_back(item);
      this->back().setParentName(parentName);
      return *this;
    }

    /**
     * @brief Create YAML representation dumped into the given emitter.
     * @param emitter The emitter to wich the configVector yaml will be added (modifies parameter!).
     */
    virtual void dumpToYamlEmitter(YAML::Emitter &emitter) const;

    /**
     * @brief Create JSON representation dumped into the given value.
     * @param root The value to wich the configVector json will be added (modifies parameter!).
     */
    virtual void dumpToJsonValue(Json::Value &root) const;

  }; // end of class ConfigVector

} // end of namespace configmaps

#endif // CONFIG_VECTOR_H
