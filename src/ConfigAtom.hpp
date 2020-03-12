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

#ifndef CONFIG_ATOM_HPP
#define CONFIG_ATOM_HPP

#ifdef _PRINT_HEADER_
#warning "ConfigAtom.hpp"
#endif

#include <string>
#include <stdexcept>
#include <cstdio>
#ifndef Q_MOC_RUN
#include <yaml-cpp/yaml.h>

#ifdef LINUX
#include <jsoncpp/json/json.h>
#else
#include <json/json.h>
#endif

#endif
#include "ConfigBase.hpp"


namespace configmaps {

  class ConfigAtom : public ConfigBase {
  public:
    enum ItemType {UNDEFINED_TYPE, INT_TYPE, UINT_TYPE, DOUBLE_TYPE,
                   ULONG_TYPE, STRING_TYPE, BOOL_TYPE};

    ConfigAtom() : luValue(0), iValue(0), uValue(0), dValue(0.0),
                   parsed(false), type(UNDEFINED_TYPE) {}


    ConfigAtom(int val) : luValue(0), iValue(val),
                          uValue(0), dValue(0.0),
                          parsed(true), type(INT_TYPE) {}

    ConfigAtom(bool val) : luValue(0), iValue(val),
                           uValue(0), dValue(0.0),
                           parsed(true), type(BOOL_TYPE) {}

    ConfigAtom(unsigned int val) : luValue(0), iValue(0),
                                   uValue(val), dValue(0.0),
                                   parsed(true), type(UINT_TYPE) {}

    ConfigAtom(double val) : luValue(0), iValue(0),
                             uValue(0), dValue(val),
                             parsed(true), type(DOUBLE_TYPE) {}

    ConfigAtom(unsigned long val) : luValue(val), iValue(0),
                                    uValue(0), dValue(0.0),
                                    parsed(true), type(ULONG_TYPE) {}

    ConfigAtom(std::string val) : luValue(0), iValue(0),
                                  uValue(0), dValue(0.0),
                                  sValue(val.c_str()), parsed(false),
                                  type(UNDEFINED_TYPE) {}

    ConfigAtom(const char *val) : luValue(0), iValue(0),
                                  uValue(0), dValue(0.0),
                                  sValue(val), parsed(false),
                                  type(UNDEFINED_TYPE) {}
    /**
     * @brief Fills ConfigAtom item from YAML::Node.
     * @param n The node containing the informations for the object.
     * @throw Throws std::runtime_error if the type of the node is not scalar.
     */
    ConfigAtom(const YAML::Node &n) {
      if(n.Type() != YAML::NodeType::Scalar) {
        throw std::runtime_error("Failed to create ConfigAtom Item, YAML::Node was not a scalar type!");
      }
      setUnparsedString(n.Scalar());
      if(ConfigMap::debugLevel >= 1) {
        fprintf(stderr, " %s", this->toString().c_str());
      }
    }

    ConfigAtom(const Json::Value &v) {
      setUnparsedString(v.asString());
    }

    operator int () {
      return getInt();
    }

    operator double () {
      return getDouble();
    }

    operator unsigned int () {
      return getUInt();
    }

    operator unsigned long () {
      return getULong();
    }

    operator std::string () {
      return (std::string)getString().c_str();
    }

    operator bool () {
      return getBool();
    }

    inline ItemType getType() const {
      return type;
    }

    inline bool testType(ItemType _type) {
      if(type == UNDEFINED_TYPE) {
        switch(_type) {
        case INT_TYPE:
          return parseInt();
        case DOUBLE_TYPE:
          return parseDouble();
        case UINT_TYPE:
          return parseUInt();
        case ULONG_TYPE:
          return parseULong();
        case STRING_TYPE:
          return parseString();
        case BOOL_TYPE:
          return parseBool();
        default:
          return false;
        }
      }
      else if(type == _type) return true;
      return false;
    }

    inline int getInt() {
      if(type != UNDEFINED_TYPE && type != INT_TYPE) {
        char text[50];
        sprintf(text, "ConfigAtom parsing wrong type getInt: %s - %s",
                parentName.c_str(), sValue.c_str());
        throw std::runtime_error(text);
      }

      if(!parsed) parseInt();
      if(parsed) type = INT_TYPE;

      return iValue;
    }

    inline double getDouble() {
      if(type != UNDEFINED_TYPE && type != DOUBLE_TYPE) {
        char text[50];
        sprintf(text, "ConfigAtom parsing wrong type getDouble: %s - %s",
                parentName.c_str(), sValue.c_str());
        throw std::runtime_error(text);
      }
      if(!parsed) parseDouble();
      if(parsed) type = DOUBLE_TYPE;
      return dValue;
    }

    inline unsigned int getUInt() {
      if(type != UNDEFINED_TYPE && type != UINT_TYPE) {
        char text[50];
        sprintf(text, "ConfigAtom parsing wrong type getInt: %s - %s",
                parentName.c_str(), sValue.c_str());
        throw std::runtime_error(text);
      }

      if(!parsed) parseUInt();
      if(parsed) type = UINT_TYPE;

      return uValue;
    }

    inline unsigned long getULong() {
      if(type != UNDEFINED_TYPE && type != ULONG_TYPE) {
        char text[50];
        sprintf(text, "ConfigAtom parsing wrong type getULong: %s - %s",
                parentName.c_str(), sValue.c_str());
        throw std::runtime_error(text);
      }
      if(!parsed) parseULong();
      if(parsed) type = ULONG_TYPE;
      return luValue;
    }

    inline std::string getString() {
      if(type != UNDEFINED_TYPE && type != STRING_TYPE) {
        char text[50];
        sprintf(text, "ConfigAtom parsing wrong type getString: %s - %s",
                parentName.c_str(), sValue.c_str());
        throw std::runtime_error(text);
      }
      if(!parsed) parseString();
      if(parsed) type = STRING_TYPE;
      return sValue.c_str();
    }

    inline std::string getUnparsedString() const {
      return sValue.c_str();
    }

    inline bool getBool() {
      if(type != UNDEFINED_TYPE && type != BOOL_TYPE) {
        char text[50];
        sprintf(text, "ConfigAtom parsing wrong type %d getBool: %s - %s",
                type, parentName.c_str(), sValue.c_str());
        throw std::runtime_error(text);
      }

      if(!parsed) parseBool();
      if(parsed) type = BOOL_TYPE;

      return iValue;
    }

    inline void setInt(int value) {
      iValue = value;
      parsed = true;
      type = INT_TYPE;
    }

    inline void setDouble(double value) {
      dValue = value;
      parsed = true;
      type = DOUBLE_TYPE;
    }

    inline void setUInt(unsigned int value) {
      uValue = value;
      parsed = true;
      type = UINT_TYPE;
    }

    inline void setULong(unsigned long value) {
      luValue = value;
      parsed = true;
      type = ULONG_TYPE;
    }

    inline void setString(const std::string &value) {
      sValue = value.c_str();
      parsed = true;
      type = STRING_TYPE;
    }

    inline void setBool(bool value) {
      iValue = value;
      parsed = true;
      type = BOOL_TYPE;
    }

    inline void setUnparsedString(const std::string &value) {
      sValue = value.c_str();
      parsed = false;
      type = UNDEFINED_TYPE;
    }

    inline std::string toString() const {
      char text[64];

      if(type == UNDEFINED_TYPE) {
        return sValue;
      } else if(type == INT_TYPE) {
        sprintf(text, "%d", iValue);
        return text;
      } else if(type == UINT_TYPE) {
        sprintf(text, "%u", uValue);
        return text;
      } else if(type == DOUBLE_TYPE) {
        sprintf(text, "%g", dValue);
        return text;
      } else if(type == ULONG_TYPE) {
        sprintf(text, "%lu", luValue);
        return text;
      } else if(type == STRING_TYPE) {
        return sValue;
      } else if(type == BOOL_TYPE) {
        if(iValue) return "true";
        else return "false";
      }
      return std::string();
    }

    virtual void dumpToYamlEmitter(YAML::Emitter &emitter) const {
      std::string s = toString();
      if(ConfigBase::debugLevel >= 1) {
        fprintf(stderr, "dump: %s\n", s.c_str());
      }
      if(s.empty())
        emitter << " ";
      else
        emitter << s;
    }

    virtual void dumpToJsonValue(Json::Value &root) const {
      std::string s = toString();
      if(ConfigBase::debugLevel >= 1) {
        fprintf(stderr, "dump: %s\n", s.c_str());
      }
      if(s.empty())
        root = " ";
      else
        root = s;
    }

  private:
    unsigned long luValue;
    int iValue;
    unsigned int uValue;
    double dValue;
    std::string sValue;
    std::string parentName;
    bool parsed;
    ItemType type;

    inline bool parseInt() {
      if(type != UNDEFINED_TYPE) {
        throw std::runtime_error("ConfigAtom parsing wrong type line ...");
      }
      return parsed = sscanf(sValue.c_str(), "%d", &iValue);
    }

    inline bool parseUInt() {
      if(type != UNDEFINED_TYPE) {
        throw std::runtime_error("ConfigAtom parsing wrong type line ...");
      }
      return parsed = sscanf(sValue.c_str(), "%u", &uValue);
    }

    inline bool parseULong() {
      if(type != UNDEFINED_TYPE) {
        throw std::runtime_error("ConfigAtom parsing wrong type line ...");
      }
      return parsed = sscanf(sValue.c_str(), "%lu", &luValue);
    }

    inline bool parseDouble() {
      if(type != UNDEFINED_TYPE) {
        throw std::runtime_error("ConfigAtom parsing wrong type line ...");
      }
      return parsed = sscanf(sValue.c_str(), "%lf", &dValue);
    }

    inline bool parseString() {
      if(type != UNDEFINED_TYPE) {
        throw std::runtime_error("ConfigAtom parsing wrong type line ...");
      }
      return parsed = true;
    }

    inline bool parseBool() {
      if(type != UNDEFINED_TYPE) {
        throw std::runtime_error("ConfigAtom parsing wrong type line ...");
      }
      sValue = trim(sValue);
      if(sValue == "true" || sValue == "True" || sValue == "TRUE") {
        iValue = 1;
        return parsed = true;
      }

      if(sValue == "false" || sValue == "False" || sValue == "FALSE") {
        iValue = 0;
        return parsed = true;
      }

      return parsed = sscanf(sValue.c_str(), "%d", &iValue);
    }
  };

} // end of namespace configmaps

#endif // CONFIG_ATOM_H
