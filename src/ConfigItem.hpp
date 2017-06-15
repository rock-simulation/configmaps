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
#include "ConfigBase.hpp"

//forwards:
namespace YAML{
    class Emitter;
    class Node;
}

namespace Json{
    class Value;
}

namespace configmaps {

  class ConfigMap;
  class ConfigAtom;
  class ConfigVector;


  /**
   * @brief Base object of the configmaps library!
   *
   * This is the basic object that manages elements in the configmaps data structure.
   * It can contain all ConfigBase items, generate, serialize and de-serialize them.
   * Possible items are: ConfigAtom (scalar values), ConfigVectors (lists), and ConfigMaps (key, Item).
   */
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
        return "Bad index exception.";
      }
    } badIndexExp;

  public:
    ConfigItem();
    /**
     * @brief Constructor to create a new Item during de-serialization process from a YAML::Node.
     * @param n The YAML::Node containing the data for the new item.
     * @throw Will throw runtime Error if the YAML::Node type is unknown!
     */
    ConfigItem(const YAML::Node &n);
    ConfigItem(const ConfigItem &item);
    ConfigItem(const ConfigBase &item);
    ~ConfigItem();
    ConfigItem& operator=(const ConfigItem&);
    ConfigItem& operator=(const ConfigBase&);

    /**
     * @brief Factory function, creating a ConfigItem out of a YAML stream.
     *
     * The function creates a new ConfigItem de-serialized from a YAML std::istream.
     * Returned Item could contain ConfigAtom, ConfigVector or ConfigMap, depending of the
     * root object of the YAML stream.
     * @param in The input stream from which the YAML parser will read.
     * @return ConfigItem filled by whatever we got from the stream.
     */
    static ConfigItem fromYamlStream(std::istream &in);
    /**
     * @brief Factory function, creating a ConfigItem out of a YAML File.
     * @see fromYamlStream(std::istream &in)
     * @param filename Path of the file that will be used as input.
     * @param loadURI No idea.
     * @return ConfigItem filled by whatever we got from the file.
     * @throw std::runtime_error, if the file could not be opened.
     */
    static ConfigItem fromYamlFile(const std::string &filename,
                                  bool loadURI = false);
    /**
     * @brief Factory function, creating a ConfigItem from a YAML String.
     * @see ConfigItem fromYamlStream(std::istream &in)
     * @param s The string containing the YAML.
     * @return ConfigItem filled by whatever we got from the string.
     */
    static ConfigItem fromYamlString(const std::string &s);

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
    /**
     * @brief Calls the emitter function of its base object (if any).
     * @param emitter The emitter to output the stream.
     */
    void dumpToYamlEmitter(YAML::Emitter &emitter) const;

    /**
     * @brief Writes YAML representation of the object to given output stream.
     * @param out Stream to write on.
     */
    void toYamlStream(std::ostream &out) const;

    /**
     * @brief Serialize the object to a YAML stream and output it to given filename.
     * @param filename The file will be created and the stream will be written into it.
     */
    void toYamlFile(const std::string &filename) const;

    /**
     * Writes a YAML serialization of the object into a string.
     * @return The resulting YAML string.
     */
    std::string toYamlString() const;

    /**
     * @brief Calls the emitter function of its base object (if any).
     * @param emitter The emitter to output the stream.
     */
    void dumpToJsonValue(Json::Value &root) const;

    /**
     * @brief Writes YAML representation of the object to given output stream.
     * @param out Stream to write on.
     */
    void toJsonStream(std::ostream &out) const;

    /**
     * Writes a JSON serialization of the object into a string.
     * @return The resulting JSON string.
     */
    std::string toJsonString() const;

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
    void erase(FIFOMap<std::string, ConfigItem>::iterator &it);

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

    static void recursiveLoad(ConfigItem &item, std::string &path);
    static std::string getPathOfFile(const std::string &filename);
  };


  template<typename T>
  T& operator<<(T &s, ConfigItem &v) {
    s = (T)v;
    return s;
  }

} // end of namespace configmaps

#endif // CONFIG_ITEM_H
