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
#include <sstream>
#include <fstream>
#include <exception>

#ifdef _WIN32
#define POINTER void*
#else
#define POINTER unsigned long
#endif

namespace configmaps {

  ConfigItem::ConfigItem() {
    item = NULL;
    if(ConfigBase::debugLevel >= 2) {
      fprintf(stderr, "new d %lx %lx\n", (POINTER)this->item, (POINTER)this);
    }
  }

  ConfigItem::ConfigItem(const YAML::Node &n) {
    ///@todo implement me!
    //create correct object type for the item:
    if(n.IsScalar()) {
      item = new ConfigAtom(n);
    } else if(n.IsSequence()) {
      item = new ConfigVector(n);
    } else if(n.IsMap()) {
      item = new ConfigMap(n);
    } else {
      char error[128];
      sprintf(error, "Could not create ConfigItem from unknown YAML::NodeType! %d", n.Type());
      fprintf(stderr, "%s\n", error);
      throw std::runtime_error(error);
    }
  }

  ConfigItem::ConfigItem(const Json::Value &v) {
    ///@todo implement me!
    //create correct object type for the item:
    if(v.isArray()) {
      item = new ConfigVector(v);
    }
    else if(v.isObject()) {
      item = new ConfigMap(v);
    }
    else {
      item = new ConfigAtom(v);
    }
  }

  ConfigItem::ConfigItem(const ConfigItem &item) {
    this->item = NULL;
    if(item.item) {
      *this = *item.item;
    }
    if(ConfigBase::debugLevel >= 2) {
      fprintf(stderr, "new d %lx %lx\n", (POINTER)this->item, (POINTER)this);
    }
  }

  ConfigItem::ConfigItem(const ConfigBase &item) {
    this->item = NULL;
    *this = item;
    if(ConfigBase::debugLevel >= 2) {
      fprintf(stderr, "new d %lx %lx\n", (POINTER)this->item, (POINTER)this);
    }
  }

  ConfigItem& ConfigItem::operator=(const ConfigItem& item) {
    if(item.item) {
      return *this = *item.item;
    }
    else {
      if(this->item) {
        delete this->item;
        this->item = NULL;
      }
      return *this;
    }
  }

  ConfigItem& ConfigItem::operator=(const ConfigBase& item) {
    if(this->item) {
      if(ConfigBase::debugLevel >= 2) {
        fprintf(stderr, "delete %lx\n", (POINTER)this->item);
      }
      delete this->item;
      this->item = NULL;
    }
    if(ConfigBase::debugLevel >= 2) {
      fprintf(stderr, "new = old %lx\n", (POINTER)this->item);
    }
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
    if(ConfigBase::debugLevel >= 2) {
      fprintf(stderr, "new = %lx  %lx\n", (POINTER)this->item, (POINTER)this);
    }
    return *this;
  }

  ConfigItem::~ConfigItem() {
    if(ConfigBase::debugLevel >= 2) {
      fprintf(stderr, "delete %lx\n", (POINTER)item);
    }
    delete item;
  }

  ConfigItem ConfigItem::fromYamlStream(std::istream &in) {
    YAML::Node node = YAML::Load(in);
    return ConfigItem(node);
  }

  ConfigItem ConfigItem::fromYamlFile(const std::string &filename, bool loadURI) {
    std::ifstream fin(filename.c_str());
    if(fin.fail()){
      throw std::runtime_error("Failed to open File: " + filename);
    }

    ConfigItem retVal = fromYamlStream(fin);

    if(loadURI) {
      std::string pathToFile = getPathOfFile(filename);
      recursiveLoad(retVal, pathToFile);
    }
    return retVal;
  }

  ConfigItem ConfigItem::fromYamlString(const std::string &s) {
    std::istringstream sin(s);
    return fromYamlStream(sin);
  }

  ConfigItem ConfigItem::fromJsonStream(std::istream &in) {
    Json::Value v;
    in >> v;
    return ConfigItem(v);
  }

  ConfigItem ConfigItem::fromJsonString(const std::string &s) {
    std::istringstream sin(s);
    return fromJsonStream(sin);
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
    fprintf(stderr, "(endMap) parent: %s\n", parentName.c_str());
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
    fprintf(stderr, "(map::find) parent: %s\n", parentName.c_str());
    throw wrongTypeExp;
  }

  bool ConfigItem::hasKey(std::string key) {
    return (find(key) != endMap());
  }

  void ConfigItem::appendMap(const ConfigMap &value) {
    if(!item) {
      item = new ConfigMap();
      item->setParentName(parentName);
    }
    ConfigMap *v = dynamic_cast<ConfigMap*>(item);
    if(v) {
      v->append(value);
      return;
    }
    fprintf(stderr, "(map::appendMap) parent: %s\n", parentName.c_str());
    throw wrongTypeExp;
  }

  void ConfigItem::updateMap(const ConfigMap &update) {
    if(!item) {
      item = new ConfigMap();
      item->setParentName(parentName);
    }
    ConfigMap *v = dynamic_cast<ConfigMap*>(item);
    if(v) {
      ConfigMap map = update;
      v->updateMap(map);
      return;
    }
    fprintf(stderr, "(map::updateMap) parent: %s\n", parentName.c_str());
    throw wrongTypeExp;
  }

  void ConfigItem::erase(FIFOMap<std::string, ConfigItem>::iterator &it) {
    if(!item) {
      item = new ConfigMap();
      item->setParentName(parentName);
    }
    ConfigMap *v = dynamic_cast<ConfigMap*>(item);
    if(v) {
      v->erase(it);
      return;
    }
    fprintf(stderr, "(map::erase) parent: %s\n", parentName.c_str());
    throw wrongTypeExp;
  }

  void ConfigItem::dumpToYamlEmitter(YAML::Emitter &emitter) const {
    if(!item){
      throw std::runtime_error("Item not set while toYamlStream was requested!");
    }
    item->dumpToYamlEmitter(emitter);
  }

  void ConfigItem::toYamlStream(std::ostream &out) const {
    YAML::Emitter emitter;
    dumpToYamlEmitter(emitter);
    if(!emitter.good()) {
      fprintf(stderr, "ERROR: ConfigMap::toYamlStream failed!\n");
      return;
    }
    out << emitter.c_str() << std::endl;
  }

  void ConfigItem::dumpToJsonValue(Json::Value &root) const{
    if(!item){
      throw std::runtime_error("Item not set while toYamlStream was requested!");
    }
    item->dumpToJsonValue(root);
  }

  void ConfigItem::toJsonStream(std::ostream &out) const {
    Json::Value root;
    dumpToJsonValue(root);
    out << root.toStyledString() << std::endl;
  }

  void ConfigItem::toYamlFile(const std::string &filename) const {
    std::ofstream f(filename.c_str());
    if(!f.good()) {
      fprintf(stderr,
              "ERROR: ConfigMap::toYamlFile failed! "
              "Could not open output file \"%s\"\n", filename.c_str());
      return;
    }
    toYamlStream(f);
  }

  std::string ConfigItem::toYamlString() const {
    std::ostringstream sout;
    toYamlStream(sout);
    return sout.str();
  }

  std::string ConfigItem::toJsonString() const {
    std::ostringstream sout;
    toJsonStream(sout);
    return sout.str();
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
    fprintf(stderr, "(map&) parent: %s\n", parentName.c_str());
    throw wrongTypeExp;
  }

  ConfigItem::operator ConfigMap& () const {
    if(!item) {
      fprintf(stderr, "(map&) parent: %s\n", parentName.c_str());
      throw wrongTypeExp;
    }
    ConfigMap *m = dynamic_cast<ConfigMap*>(item);
    if(m) return *m;
    fprintf(stderr, "(map&) parent: %s\n", parentName.c_str());
    throw wrongTypeExp;
  }

  ConfigItem::operator ConfigMap* () {
    if(!item) {
      item = new ConfigMap();
      item->setParentName(parentName);
    }
    ConfigMap *m = dynamic_cast<ConfigMap*>(item);
    if(m) return m;
    fprintf(stderr, "(map*) parent: %s\n", parentName.c_str());
    throw wrongTypeExp;
  }

  ConfigItem::operator ConfigMap* () const {
    if(!item) {
      fprintf(stderr, "(map&) parent: %s\n", parentName.c_str());
      throw wrongTypeExp;
    }
    ConfigMap *m = dynamic_cast<ConfigMap*>(item);
    if(m) return m;
    fprintf(stderr, "(map*) parent: %s\n", parentName.c_str());
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

  bool ConfigItem::operator==(const std::string& s) {
    return (getOrCreateAtom()->getString() == s);
  }

  bool ConfigItem::operator!=(const std::string& s) {
    return (getOrCreateAtom()->getString() != s);
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
    fprintf(stderr, "([s]) parent: %s\n", parentName.c_str());
    throw wrongTypeExp;
  }

  ConfigItem& ConfigItem::operator[](int s) {
    if(s < 0) throw badIndexExp;
    return (*this)[(size_t)s];
  }

  ConfigItem& ConfigItem::operator[](size_t s) {
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
    fprintf(stderr, "([ul]) parent: %s\n", parentName.c_str());
    throw wrongTypeExp;
  }

  size_t ConfigItem::append(const ConfigItem &value) {
    return getOrCreateVector()->append(value);
  }

  std::vector<ConfigItem>::iterator ConfigItem::erase(std::vector<ConfigItem>::iterator &it) {
    return getOrCreateVector()->erase(it);
  }


  ConfigItem& ConfigItem::operator+=(const ConfigItem &value) {
    *getOrCreateVector() += value;
    return *this;
  }


  std::string ConfigItem::getString() {
    return getOrCreateAtom()->getString();
  }

  std::string ConfigItem::toString() const {
    if(item) {
      ConfigAtom *m = dynamic_cast<ConfigAtom*>(item);
      if(m) return m->toString();
    }
    fprintf(stderr, "([atom::toString]) parent: %s\n", parentName.c_str());
    throw wrongTypeExp;
  }

  size_t ConfigItem::size() const {
    if(item) {
      ConfigVector *v = dynamic_cast<ConfigVector*>(item);
      if(v) return v->size();
      ConfigMap *m = dynamic_cast<ConfigMap*>(item);
      if(m) return m->size();
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
    fprintf(stderr, "([atom::getOrCreateAtom]) parent: %s\n", parentName.c_str());
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


  /*/**********************
   * Private Methods
   ************************/

  void ConfigItem::recursiveLoad(ConfigItem &item, std::string &path) {

    if(item.isMap()) {
      ConfigMap &map = item;
      ConfigMap::iterator it = item.beginMap();
      std::list<ConfigMap::iterator> eraseList;
      std::list<ConfigMap::iterator>::iterator eraseIt;
      for(; it!=item.endMap(); ++it) {
        if(it->first == "URI") {

          std::string file = path + (std::string)it->second;
          std::string subPath = getPathOfFile(file);
          ConfigItem m2 = fromYamlFile(file, true);
          recursiveLoad(m2, subPath);
          map.append(m2);
          eraseList.push_back(it);
        }else {
          recursiveLoad(it->second, path);
        }
      }
      for(eraseIt=eraseList.begin(); eraseIt!=eraseList.end(); ++eraseIt) {
        map.erase(*eraseIt);
      }
    } else {
      if(item.isVector()) {
        std::vector<ConfigItem>::iterator it = ((ConfigVector&)item).begin();
        for(; it!= ((ConfigVector&)item).end(); ++it) {
          recursiveLoad(*it, path);
        }
      }
    }
  }


  // utility functions
  std::string ConfigItem::getPathOfFile(const std::string &filename) {
    std::string path = "./";
    size_t pos;

    if((pos = filename.rfind('/')) != std::string::npos) {
      path = filename.substr(0, pos+1);
    }
    return path;
  }

} // end of namespace configmaps
