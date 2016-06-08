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
    return getOrCreateVector()->begin();
  }

  std::vector<ConfigItem>::iterator ConfigItem::end() {
    return getOrCreateVector()->end();
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
    throw wrongTypeExp;
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
    throw wrongTypeExp;
  }

  bool ConfigItem::hasKey(std::string key) {
    return (find(key) != endMap());
  }


  void ConfigItem::dumpToYamlEmitter(YAML::Emitter &emitter) const{
      item->dumpToYamlEmitter(emitter);
  }

  bool ConfigItem::isAtom() const {
    if(item) {
      ConfigAtom *m = dynamic_cast<ConfigAtom*>(item);
      if(m) return true;
    }
    return false;
  }

  bool ConfigItem::isMap() const {
    if(item) {
      ConfigMap *m = dynamic_cast<ConfigMap*>(item);
      if(m) return true;
    }
    return false;
  }

  bool ConfigItem::isVector() const {
    if(item) {
      ConfigVector *m = dynamic_cast<ConfigVector*>(item);
      if(m) return true;
    }
    return false;
  }

  ConfigItem::operator ConfigMap& () {
    if(!item) {
      item = new ConfigMap();
      item->setParentName(parentName);
    }
    ConfigMap *m = dynamic_cast<ConfigMap*>(item);
    if(m) return *m;
    throw wrongTypeExp;
  }

  ConfigItem::operator ConfigMap* () {
    if(!item) {
      item = new ConfigMap();
      item->setParentName(parentName);
    }
    ConfigMap *m = dynamic_cast<ConfigMap*>(item);
    if(m) return m;
    throw wrongTypeExp;
  }

  ConfigItem::operator ConfigVector& () {
    return *getOrCreateVector();
  }

  ConfigItem::operator ConfigVector* () {
    return getOrCreateVector();
  }

  ConfigItem::operator ConfigAtom& () {
    return *getOrCreateAtom();
  }

  ConfigItem::operator ConfigAtom* () {
    return getOrCreateAtom();
  }

  ConfigItem::operator int () {
    return (int)*getOrCreateAtom();
  }

  ConfigItem::operator unsigned int () {
    return (unsigned int)*getOrCreateAtom();
  }

  ConfigItem::operator double () {
    return (double)*getOrCreateAtom();
  }

  ConfigItem::operator unsigned long () {
    return (unsigned long)*getOrCreateAtom();
  }

  ConfigItem::operator std::string () {
    return (std::string)*getOrCreateAtom();
  }

  ConfigItem::operator bool () {
    return (bool)*getOrCreateAtom();
  }

  ConfigItem& ConfigItem::operator=(const int v) {
    getOrCreateAtom()->setInt(v);
    return *this;
  }

  ConfigItem& ConfigItem::operator=(const unsigned int v) {
    getOrCreateAtom()->setUInt(v);
    return *this;
  }

  ConfigItem& ConfigItem::operator=(const double v) {
    getOrCreateAtom()->setDouble(v);
    return *this;
  }

  ConfigItem& ConfigItem::operator=(const unsigned long v) {
    getOrCreateAtom()->setULong(v);
    return *this;
  }

  ConfigItem& ConfigItem::operator=(const std::string &v) {
    getOrCreateAtom()->setUnparsedString(v);
    return *this;
  }

  ConfigItem& ConfigItem::operator=(const char *v) {
    getOrCreateAtom()->setUnparsedString(v);
    return *this;
  }

  ConfigItem& ConfigItem::operator=(const bool v) {
    getOrCreateAtom()->setBool(v);
    return *this;
  }

  ConfigItem& ConfigItem::operator[](const char* s) {
    return (*this)[std::string(s)];
  }

  ConfigItem& ConfigItem::operator[](std::string s) {
    if(!item) {
      item = new ConfigMap();
      item->setParentName(parentName);
    }
    ConfigMap *m = dynamic_cast<ConfigMap*>(item);
    if(m) {
      return (*m)[s];
    }
    ConfigVector *v = dynamic_cast<ConfigVector*>(item);
    if(v) {
      return (*v)[0][s];
    }
    throw wrongTypeExp;
  }

  ConfigItem& ConfigItem::operator[](int s) {
    if(s < 0) throw badIndexExp;
    return (*this)[(unsigned long)s];
  }

  ConfigItem& ConfigItem::operator[](unsigned long s) {
    ConfigVector *v = getOrCreateVector();
    if(v) {
      if(v->size() > s) {
        return (*v)[s];
      }
      else if(v->size() == s) {
        *v += ConfigItem();
        return (*v)[s];
      }
    }
    // allow support for the old api [0] access
    if(s==0) {
      return *this;
    }
    throw wrongTypeExp;
  }

  size_t ConfigItem::append(const ConfigItem &value) {
    return getOrCreateVector()->append(value);
  }

  std::vector<ConfigItem>::iterator ConfigItem::erase(std::vector<ConfigItem>::iterator &it) {
    return getOrCreateVector()->erase(it);
  }

  /*
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
  */

  ConfigItem& ConfigItem::operator+=(const ConfigItem &value) {
    *getOrCreateVector() += value;
    return *this;
  }
  /*
  ConfigItem& ConfigItem::operator+=(const ConfigAtom &value) {
    return *this += (ConfigItem)value;
  }
  */
  std::string ConfigItem::getString() {
    return getOrCreateAtom()->getString();
  }

  const char* ConfigItem::c_str() {
    return getString().c_str();
  }

  std::string ConfigItem::toString() const {
    if(item) {
      ConfigAtom *m = dynamic_cast<ConfigAtom*>(item);
      if(m) return m->toString();
    }
    throw wrongTypeExp;
  }

  size_t ConfigItem::size() const {
    if(item) {
      ConfigVector *v = dynamic_cast<ConfigVector*>(item);
      if(v) return v->size();
      ConfigMap *m = dynamic_cast<ConfigMap*>(item);
      if(m) return v->size();
      ConfigAtom *a = dynamic_cast<ConfigAtom*>(item);
      if(a) return 1;
    }
    throw noTypeExp;
  }

  ConfigAtom* ConfigItem::getOrCreateAtom() {
    ConfigAtom *atom;
    if(!item) {
      atom = new ConfigAtom();
      item = atom;
      item->setParentName(parentName);
      return atom;
    }
    atom = dynamic_cast<ConfigAtom*>(item);
    if(atom) return atom;
    ConfigVector *v = dynamic_cast<ConfigVector*>(item);
    if(v) return (*v)[0].getOrCreateAtom();
    throw wrongTypeExp;
  }

  ConfigVector* ConfigItem::getOrCreateVector() {
    ConfigVector *v;
    if(!item) {
      v = new ConfigVector();
      item = v;
      item->setParentName(parentName);
      return v;
    }
    v = dynamic_cast<ConfigVector*>(item);
    if(v) return v;
    // we can convert the atom item to a vector
    v = new ConfigVector();
    *v += *item;
    item = v;
    item->setParentName(parentName);
    return v;
  }

} // end of namespace configmaps
