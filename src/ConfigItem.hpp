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

#ifndef CONFIG_ITEM_HPP
#define CONFIG_ITEM_HPP

#ifdef _PRINT_HEADER_
  #warning "ConfigItem.hpp"
#endif


#include <string>
#include <vector>
#include "FIFOMap.h"

namespace configmaps {

  class ConfigMap;
  class ConfigVector;
  class ConfigAtom;
  class ConfigItem;

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

    protected:
      std::string parentName;

    }; // end of class ConfigVectorTemplate

  class ConfigItem {
    class NoTypeException: public std::exception {
      virtual const char* what() const throw() {
        return "Item has no type.";
      }
    } noTypeExp;

    class WrongTypeException: public std::exception {
      virtual const char* what() const throw() {
        return "Item is of wrong type.";
      }
    } wrongTypeExp;

    class BadIndexException: public std::exception {
      virtual const char* what() const throw() {
        return "Item is of wrong type.";
      }
    } badIndexExp;

  public:
    ConfigItem();
    ConfigItem(const ConfigItem &item);
    ConfigItem(const ConfigBase &item);
    ~ConfigItem();
    ConfigItem& operator=(const ConfigItem&);
    ConfigItem& operator=(const ConfigBase&);

    operator const ConfigBase& () const {return *item;}
    operator ConfigBase& () {return *item;}
    operator ConfigMap& ();
    operator ConfigMap* ();
    operator ConfigVector& ();
    operator ConfigVector* ();
    operator ConfigAtom& ();
    operator ConfigAtom* ();
    operator ConfigItem* () {return this;}

    void setParentName(std::string s) {
      parentName = s;
      if(item) {
        item->setParentName(s);
      }
    }

    bool isAtom() const;
    bool isMap() const;
    bool isVector() const;

    /* Atom types:
     *  - int
     *  - uint
     *  - double
     *  - ulong
     *  - string
     *  - bool
     */
    operator int ();
    operator unsigned int ();
    operator double ();
    operator unsigned long ();
    operator std::string ();
    operator bool ();

    ConfigItem& operator=(const int v);
    ConfigItem& operator=(const unsigned int v);
    ConfigItem& operator=(const double v);
    ConfigItem& operator=(const unsigned long v);
    ConfigItem& operator=(const std::string &v);
    ConfigItem& operator=(const char* v);
    ConfigItem& operator=(const bool);
    const char* c_str();
    std::string toString() const;
    // deprecated atom function
    std::string getString();

    template<typename T>
    ConfigItem& operator>>(T &s) {
      s = (T)*this;
      return *this;
    }

    // map access
    ConfigItem& operator[](std::string s);
    ConfigItem& operator[](const char* v);
    FIFOMap<std::string, ConfigItem>::iterator beginMap();
    FIFOMap<std::string, ConfigItem>::iterator endMap();
    FIFOMap<std::string, ConfigItem>::iterator find(std::string key);
    bool hasKey(std::string key);

    // vector access
    ConfigItem& operator[](unsigned long v);
    ConfigItem& operator[](int v);
    std::vector<ConfigItem>::iterator begin();
    std::vector<ConfigItem>::iterator end();
    size_t append(const ConfigItem &item);
    std::vector<ConfigItem>::iterator erase(std::vector<ConfigItem>::iterator &it);
    /*
      ConfigItem& operator<<(const ConfigItem &item);
      ConfigItem& operator<<(const ConfigAtom &item);
    */
    template<typename T>
    ConfigItem& operator<<(const T &value) {
      return (*this = value);
    }

    ConfigItem& operator+=(const ConfigItem &item);
    ConfigItem& operator+=(const ConfigAtom &item) {
      return *this += ConfigItem((ConfigBase&)item);
    }
    ConfigItem& operator+=(const ConfigMap &item) {
      return *this += ConfigItem((ConfigBase&)item);
    }

    template<typename T>
    void push_back(const T &value) {
      *this += value;
    }

    template<typename T>
    ConfigItem& operator+=(const T &value) {
      return *this += ConfigItem(ConfigAtom(value));
    }

    /*
    template<typename T>
    void push_back(const T &value) {
      *this << (ConfigItem)ConfigAtom(value);
    }
    */
    size_t size() const;
    ConfigAtom* getOrCreateAtom();
    ConfigVector* getOrCreateVector();

  private:
    ConfigBase *item;
    std::string parentName;
  };


  template<typename T>
  T& operator<<(T &s, ConfigItem &v) {
    s = (T)v;
    return s;
  }

} // end of namespace configmaps

#endif // CONFIG_ITEM_H
