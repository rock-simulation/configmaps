/*
 *  Copyright 2011, 2012, 2015, DFKI GmbH Robotics Innovation Center
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


#include "ConfigMap.hpp"
#include "ConfigVector.hpp"
#include "ConfigAtom.hpp"

//#define VERBOSE

namespace configmaps {

  ConfigItem::ConfigItem() {
    item = NULL;
#ifdef VERBOSE
    fprintf(stderr, "new d %lx %lx\n", (unsigned long)this->item, (unsigned long)this);
#endif
  }

  ConfigItem::ConfigItem(const ConfigItem &item) {
    this->item = NULL;
    *this = *item.item;
#ifdef VERBOSE
    fprintf(stderr, "new d %lx %lx\n", (unsigned long)this->item, (unsigned long)this);
#endif
  }

  ConfigItem::ConfigItem(const ConfigBase &item) {
    this->item = NULL;
    *this = item;
#ifdef VERBOSE
    fprintf(stderr, "new d %lx %lx\n", (unsigned long)this->item, (unsigned long)this);
#endif
  }
  
  ConfigItem& ConfigItem::operator=(const ConfigItem& item) {
    return *this = *item.item;
  }
  
  ConfigItem& ConfigItem::operator=(const ConfigBase& item) {
    if(this->item) {
#ifdef VERBOSE
      fprintf(stderr, "delete %lx\n", (unsigned long)this->item);
#endif
      delete this->item;
      this->item = NULL;
    }
#ifdef VERBOSE
    fprintf(stderr, "new = old %lx\n", (unsigned long)this->item);
#endif
    const ConfigAtom* a = dynamic_cast<const ConfigAtom*>(&item);
    if(a) {
      this->item = new ConfigAtom(*a);
      this->item->setParentName(parentName);
    }
    else {
      const ConfigMap* m = dynamic_cast<const ConfigMap*>(&item);
      if(m) {
        this->item = new ConfigMap(*m);
        this->item->setParentName(parentName);
      }
      else {
        const ConfigVector *v = dynamic_cast<const ConfigVector*>(&item);
        if(v) {
          this->item = new ConfigVector(*v);
          this->item->setParentName(parentName);
        }
      }
    }
#ifdef VERBOSE
    fprintf(stderr, "new = %lx  %lx\n", (unsigned long)this->item, (unsigned long)this);
#endif
    return *this;
  }
  
  ConfigItem::~ConfigItem() {
#ifdef VERBOSE
    fprintf(stderr, "delete %lx\n", (unsigned long)item);
#endif
    delete item;
  }
  
  std::vector<ConfigItem>::iterator ConfigItem::begin() {
    if(!item) {
      item = new ConfigVector();
      item->setParentName(parentName);
    }
    ConfigVector *v = dynamic_cast<ConfigVector*>(item);
    if(v) {
      return v->begin();
    }
    throw 3;
  }

  std::vector<ConfigItem>::iterator ConfigItem::end() {
    if(!item) {
      item = new ConfigVector();
      item->setParentName(parentName);
    }
    ConfigVector *v = dynamic_cast<ConfigVector*>(item);
    if(v) {
      return v->end();
    }
    throw 3;
  }

  FIFOMap<std::string, ConfigItem>::iterator ConfigItem::beginMap() {
    if(!item) {
      item = new ConfigMap();
      item->setParentName(parentName);
    }
    ConfigMap *v = dynamic_cast<ConfigMap*>(item);
    if(v) {
      return v->begin();
    }
    return FIFOMap<std::string, ConfigItem>::iterator();
  }

  FIFOMap<std::string, ConfigItem>::iterator ConfigItem::endMap() {
    if(!item) {
      item = new ConfigMap();
      item->setParentName(parentName);
    }
    ConfigMap *v = dynamic_cast<ConfigMap*>(item);
    if(v) {
      return v->end();
    }
    throw 2;
  }

  FIFOMap<std::string, ConfigItem>::iterator ConfigItem::find(std::string key) {
    if(!item) {
      item = new ConfigMap();
      item->setParentName(parentName);
    }
    ConfigMap *v = dynamic_cast<ConfigMap*>(item);
    if(v) {
      return v->find(key);
    }
    throw 2;
  }

  bool ConfigItem::hasKey(std::string key) {
    return (find(key) != endMap());
  }

  bool ConfigItem::isAtom() {
    if(item) {
      ConfigAtom *m = dynamic_cast<ConfigAtom*>(item);
      if(m) return true;
    }
    return false;
  }

  bool ConfigItem::isMap() {
    if(item) {
      ConfigMap *m = dynamic_cast<ConfigMap*>(item);
      if(m) return true;
    }
    return false;
  }

  bool ConfigItem::isVector() {
    if(item) {
      ConfigVector *m = dynamic_cast<ConfigVector*>(item);
      if(m) return true;
    }
    return false;
  }

  ConfigItem::operator int () {
    if(!item) {
      item = new ConfigAtom();
      item->setParentName(parentName);
    }
    ConfigAtom *m = dynamic_cast<ConfigAtom*>(item);
    if(m) return (int)*m;
    throw 1;
  }

  ConfigItem::operator unsigned int () {
    if(!item) {
      item = new ConfigAtom();
      item->setParentName(parentName);
    }
    ConfigAtom *m = dynamic_cast<ConfigAtom*>(item);
    if(m) return (unsigned int)*m;
    throw 1;
  }

  ConfigItem::operator double () {
    if(!item) {
      item = new ConfigAtom();
      item->setParentName(parentName);
    }
    ConfigAtom *m = dynamic_cast<ConfigAtom*>(item);
    if(m) return (double)*m;
    throw 1;
  }

  ConfigItem::operator unsigned long () {
    if(!item) {
      item = new ConfigAtom();
      item->setParentName(parentName);
    }
    ConfigAtom *m = dynamic_cast<ConfigAtom*>(item);
    if(m) return (unsigned long)*m;
    throw 1;
  }

  ConfigItem::operator std::string () {
    if(!item) {
      item = new ConfigAtom();
      item->setParentName(parentName);
    }
    ConfigAtom *m = dynamic_cast<ConfigAtom*>(item);
    if(m) return (std::string)*m;
    throw 1;
  }

  ConfigItem::operator bool () {
    if(!item) {
      item = new ConfigAtom();
      item->setParentName(parentName);
    }
    ConfigAtom *m = dynamic_cast<ConfigAtom*>(item);
    if(m) return (bool)*m;
    throw 1;
  }

  ConfigItem& ConfigItem::operator=(int v) {
    if(!item) {
      item = new ConfigAtom();
      item->setParentName(parentName);
    }
    ConfigAtom *m = dynamic_cast<ConfigAtom*>(item);
    if(m) m->setInt(v);
    return *this;
  }

  ConfigItem& ConfigItem::operator=(unsigned int v) {
    if(!item) {
      item = new ConfigAtom();
      item->setParentName(parentName);
    }
    ConfigAtom *m = dynamic_cast<ConfigAtom*>(item);
    if(m) m->setUInt(v);
    return *this;
  }

  ConfigItem& ConfigItem::operator=(double v) {
    if(!item) {
      item = new ConfigAtom();
      item->setParentName(parentName);
    }
    ConfigAtom *m = dynamic_cast<ConfigAtom*>(item);
    if(m) m->setDouble(v);
    return *this;
  }

  ConfigItem& ConfigItem::operator=(unsigned long v) {
    if(!item) {
      item = new ConfigAtom();
      item->setParentName(parentName);
    }
    ConfigAtom *m = dynamic_cast<ConfigAtom*>(item);
    if(m) m->setULong(v);
    return *this;
  }

  ConfigItem& ConfigItem::operator=(std::string &v) {
    if(!item) {
      item = new ConfigAtom();
      item->setParentName(parentName);
    }
    ConfigAtom *m = dynamic_cast<ConfigAtom*>(item);
    if(m) m->setString(v);
    return *this;
  }

  ConfigItem& ConfigItem::operator=(const char *v) {
    if(!item) {
      item = new ConfigAtom();
      item->setParentName(parentName);
    }
    ConfigAtom *m = dynamic_cast<ConfigAtom*>(item);
    if(m) m->setString(v);
    return *this;
  }

  ConfigItem& ConfigItem::operator=(bool v) {
    if(!item) {
      item = new ConfigAtom();
      item->setParentName(parentName);
    }
    ConfigAtom *m = dynamic_cast<ConfigAtom*>(item);
    if(m) m->setBool(v);
    return *this;
  }

  ConfigItem& ConfigItem::operator[](const char* s) {
    if(!item) {
      item = new ConfigMap();
      item->setParentName(parentName);
    }
    ConfigMap *v = dynamic_cast<ConfigMap*>(item);
    if(v) {
      return (*v)[s];
    }
    throw 2;
  }

  ConfigItem& ConfigItem::operator[](std::string s) {
    if(!item) {
      item = new ConfigMap();
      item->setParentName(parentName);
    }
    ConfigMap *v = dynamic_cast<ConfigMap*>(item);
    if(v) {
      return (*v)[s];
    }
    throw 2;
  }

  ConfigItem& ConfigItem::operator[](int s) {
    if(s < 0) throw 4;
    return (*this)[(unsigned long)s];
  }

  ConfigItem& ConfigItem::operator[](unsigned long s) {
    if(!item) {
      item = new ConfigVector();
      item->setParentName(parentName);
    }
    ConfigVector *v = dynamic_cast<ConfigVector*>(item);
    if(v) {
      if(v->size() > s) {
        return (*v)[s];
      }
      else if(v->size() == s) {
        *v << ConfigItem();
        return (*v)[s];
      }
    }
    throw 2;
  }

  size_t ConfigItem::append(const ConfigItem &value) {
    if(!item) {
      item = new ConfigVector();
      item->setParentName(parentName);
    }
    ConfigVector *v = dynamic_cast<ConfigVector*>(item);
    if(v) {
      return v->append(value);
    }
    throw 2;
  }

  ConfigItem& ConfigItem::operator<<(const ConfigItem &value) {
    if(!item) {
      item = new ConfigVector();
      item->setParentName(parentName);
    }
    ConfigVector *v = dynamic_cast<ConfigVector*>(item);
    if(v) {
      *v << value;
      return *this;
    }
    throw 2;
  }

  ConfigItem& ConfigItem::operator<<(const ConfigAtom &value) {
    return *this << (ConfigItem)value;
  }

  ConfigItem& ConfigItem::operator+=(const ConfigItem &value) {
    if(!item) {
      item = new ConfigVector();
      item->setParentName(parentName);
    }
    ConfigVector *v = dynamic_cast<ConfigVector*>(item);
    if(v) {
      *v += value;
      return *this;
    }
    throw 2;
  }

  ConfigItem& ConfigItem::operator+=(const ConfigAtom &value) {
    return *this += (ConfigItem)value;
  }

  std::string ConfigItem::getString() {
    if(!item) {
      item = new ConfigAtom();
      item->setParentName(parentName);
    }
    ConfigAtom *m = dynamic_cast<ConfigAtom*>(item);
    if(m) return m->getString();
    throw 2;
  }

  const char* ConfigItem::c_str() {
    return getString().c_str();
  }

  std::string ConfigItem::toString() {
    if(!item) {
      item = new ConfigAtom();
      item->setParentName(parentName);
    }
    ConfigAtom *m = dynamic_cast<ConfigAtom*>(item);
    if(m) return m->toString();
    throw 2;
  }

  size_t ConfigItem::size() {
    if(item) {
      ConfigVector *v = dynamic_cast<ConfigVector*>(item);
      if(v) return v->size();
      ConfigMap *m = dynamic_cast<ConfigMap*>(item);
      if(m) return v->size();
    }
    throw 2;
  }

} // end of namespace configmaps
