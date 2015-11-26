Documentation can be found at [doc](doc/configmaps.md).

In the restructured branch the internal representation is refactored. Instead
of using the map/vector/map/vector principle we now have a map of ConfigItems
and a ConfigItem is of type map, vector, or atom. The api is mainly kept the
same. Main difference is that the children property is removed. The master api
is also adapted to allow code that compiles against the restructured and the
"old" master branch. Changes that are needed to compile against both branches
are listed below:


Remove use of the children property:

```
   map["foo"][0].children["blub"] = "test";
   ->  map["foo"]["blub"] = "test";

   it = map["foo"][0].children.begin();
   ->  it = map["foo"].beginMap();

   The same for end() and find().

   A hasKey() function is added:
   map.hasKey("foo");          // for map["foo"]
   map["foo"].hasKey("blub");  // for map["foo"]["blub"]
```

Remove use of getInt(), getULong(), etc. methods:

```
   map["foo"][0].getInt();
   ->  map["foo"];
   map["foo"][0].getStirng();
   ->  (std::string)map["foo"];
```

Do not use the first vector element to access the first item:

```
   int i = map["foo"][0];
   ->  int i = map["foo"]
```

Operators provide pointers to subelements:

```
   ConfigMap &m = map["foo"][0].children;
   -> ConfigMap &m = map["foo"];

   ConfigMap *m = &(map["foo"][0].children);
   -> ConfigMap *m = map["foo"];

   The same for ConfigVector and ConfigItem;
```

To get the size of a submap:

```
   map["foo"][0].children.size();
   -> ((ConfigMap&)map["foo"]).size();
   The cast is only needed to be compatible to the olde implementation.
```

Don't use the ConfigItem constructor:

```
   map["foo"].push_back(ConfigItem("blub"));
   ->  map["foo"].push_back("blub");

   map["foo"] = ConfigItem("blub");
   ->  map["foo"] = "blub";
```
