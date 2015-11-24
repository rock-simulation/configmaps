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
  public:
    ConfigItem();
    ConfigItem(const ConfigItem &item);
    ConfigItem(const ConfigBase &item);
    ~ConfigItem();
    ConfigItem& operator=(const ConfigItem&);
    ConfigItem& operator=(const ConfigBase&);

    operator const ConfigBase& () const {return *item;}
    operator ConfigBase& () {return *item;}

    void setParentName(std::string s) {
      parentName = s;
      if(item) {
        item->setParentName(s);
      }
    }

    const char* c_str();

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

    ConfigItem& operator=(int v);
    ConfigItem& operator=(unsigned int v);
    ConfigItem& operator=(double v);
    ConfigItem& operator=(unsigned long v);
    ConfigItem& operator=(std::string &v);
    ConfigItem& operator=(const char* v);
    ConfigItem& operator=(bool);
    // deprecated atom function
    std::string getString();

    // map access
    ConfigItem& operator[](std::string s);
    ConfigItem& operator[](const char* v);
    FIFOMap<std::string, ConfigItem>::iterator beginMap();
    FIFOMap<std::string, ConfigItem>::iterator endMap();

    // vector access
    ConfigItem& operator[](unsigned long v);
    ConfigItem& operator[](int v);
    std::vector<ConfigItem>::iterator begin();
    std::vector<ConfigItem>::iterator end();
    size_t append(const ConfigItem &item);
    ConfigItem& operator<<(const ConfigItem &item);
    ConfigItem& operator<<(const ConfigAtom &item);
    ConfigItem& operator+=(const ConfigItem &item);
    ConfigItem& operator+=(const ConfigAtom &item);

  private:
    ConfigBase *item;
    std::string parentName;
  };

  } // end of namespace configmaps

#endif // CONFIG_ITEM_H
