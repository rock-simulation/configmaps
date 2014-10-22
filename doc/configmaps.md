Config Maps {#config_maps}
===========

## Overview

The ConfigMap is a nested implementation of std::maps and std::vectors to provide a python dictionary-like data storage structure. Datatypes that can be stored are:

 - std::string
 - int
 - unsigned int
 - unsinged long
 - double
 - bool

An important feature is the loading and storing of ConfigMaps in yaml files. The example shows the usage of ConfigMaps for further details check [ConfigMap](@ref mars::utils::ConfigData.h).

Note: While reading and writing yaml files the order of dictionaries is not necessarily kept.

Yaml example file:

    list: [1, 2, a, b]
    name: foo
    value1: 1.3
    value2: 1
    foo:
        foo2: Hello world!
        foo3: blub

Example program:

    #include <mars/utils/ConfigData.h>

    int main(int argc, char *argv[]) {

      // some ConfigMap test
      mars::utils::ConfigMap map;
      map = mars::utils::ConfigMap::fromYamlFile("test.yml");
      mars::utils::ConfigVector::iterator it;
      for(it=map["list"].begin(); it!=map["list"].end(); ++it) {
        printf("list entry: %s\n", it->getString().c_str());
      }
      double value1 = map["value1"];
      printf("value1: %g\n", value1);
      int value2 =  map["value2"];
      printf("value2: %d\n", value2);
      map["value2"] = 3;
      printf("value2 modified: %d\n", value2);

      std::string foo2 = map["foo"]["foo2"];
      std::string foo3 = map["foo"][0]["foo3"];

      printf("%s\n", foo2.c_str());
      printf("%s\n", foo3.c_str());

      // add some keys:
      map["value3"] = 3.14;
      printf("value3: %g\n", (double)map["value3"]);

      map["blub"]["foo"] = "3.14";
      printf("blub/foo: %g\n", (double)map["blub"]["foo"]);

      map.toYamlFile("result.yml");

      return 0;
    }

result.yml:

    foo:
      foo2: Hello world!
      foo3: blub
    list:
      - 1
      - 2
      - a
      - b
    name: foo
    value1: 1.3
    value2: 3
    value3: 3.14
    blub:
      foo: 3.14


\[26.08.2014\]

