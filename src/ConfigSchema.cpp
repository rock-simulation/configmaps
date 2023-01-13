#include "ConfigSchema.hpp"
#include "ConfigAtom.hpp"
#include "ConfigItem.hpp"
#include "ConfigVector.hpp"
#include "ConfigMap.hpp"
#include <optional>

namespace configmaps
{
    const std::map<std::string, std::vector<int>> ConfigSchema::SCHEMA_ATOM_TYPES = {
        {"integer", {ConfigAtom::INT_TYPE, ConfigAtom::UINT_TYPE, ConfigAtom::ULONG_TYPE}},
        {"string", {ConfigAtom::STRING_TYPE}},
        {"number", {ConfigAtom::DOUBLE_TYPE, ConfigAtom::INT_TYPE, ConfigAtom::UINT_TYPE, ConfigAtom::ULONG_TYPE}},
        {"boolean", {ConfigAtom::BOOL_TYPE}},
    };

    ConfigSchema::ConfigSchema(const ConfigMap &schema) : m_schema(schema) {}
    ConfigSchema::ConfigSchema() {}
    bool ConfigSchema::validate(ConfigMap &config)
    {
        // Check if we have a non-empty config first
        if (config.empty())
            return false;

        // Validate mandatory keys
        if (not validate_keys(config))
            return false;

        // Validate extra keys
        if (has_extra_keys(config, m_schema))
            return false;

        // Validate value types
        if (not validate_types(config))
            return false;

        // TODO:
        //  - type: enum
        //  - Validate constraints (minimum, maximum, maximum_elements)
        //  - object properties 

        return true;
    }

    bool ConfigSchema::has_corresponding_type(ConfigItem &config_item, const std::string &type)
    {
        if (type == "object")
            return config_item.isMap();
        if (type == "array")
            return config_item.isVector();
        // TODO: for some reason ConfigAtom STRING_TYPE needs to be tested with testType() instead of getType() to detect its type..
        if (type == "string")
            return config_item.getOrCreateAtom()->testType(ConfigAtom::STRING_TYPE);
        return std::find(SCHEMA_ATOM_TYPES.at(type).begin(), SCHEMA_ATOM_TYPES.at(type).end(),
                         config_item.getOrCreateAtom()->getType()) != SCHEMA_ATOM_TYPES.at(type).end();
    }

    bool ConfigSchema::is_known_type(const std::string &type)
    {
        if (SCHEMA_ATOM_TYPES.count(type))
            return true;
        return type == "object" or type == "array";
    }
    
    bool ConfigSchema::validate_keys(ConfigMap &config)
    {
        for (auto const &[key, value] : m_schema)
        {
            // Check if all keys in schema exist in our config
            if (!config.hasKey(key))
            {
                std::cerr << "ConfigSchema::validate_keys: Missing \"" << key << "\" from config" << std::endl;
                return false;
            }

            // If the item is an object, validate it recursively
            if (value["type"] == "object")
            {
                if (!config[key].isMap())
                {
                    std::cerr << "ConfigSchema::validate_keys: Expected \"" << key << "\" to be an object" << std::endl;
                    return false;
                }
            if (not value.hasKey("properties"))
                {
                    std::cerr << "ConfigSchema::validate_keys: Expected properties field in object \"" << key << "\"" << std::endl;
                    return false;
                }
                ConfigSchema sub_schema(value["properties"]);
                if (!sub_schema.validate_keys(config[key]))
                {
                    return false;
                }
            }

            
            // If the item is an array, validate its elements
            if (value["type"] == "array")
            {
                if (!config[key].isVector())
                {
                    std::cerr << "ConfigSchema::validate_keys: Expected \"" << key << "\" to be an array" << std::endl;
                    return false;
                }
                if (not value.hasKey("contains"))
                {
                    std::cerr << "ConfigSchema::validate_keys: Expected array \"" << key << "\" to have \"contains\" field" << std::endl;
                    return false;
                }
                // If its an array containing object elements, validate each object
                if (value["contains"]["type"] == "object")
                {
                    for (auto &obj : config[key])
                    {
                        ConfigMap sc = value["contains"]["properties"];
                        //sc.erase("type");
                        ConfigSchema sub_schema(sc);
                        if (not sub_schema.validate_keys(obj))
                        {
                            return false;
                        }
                    }
                }
            }
        }
        return true;
    }

    bool ConfigSchema::has_extra_keys(ConfigMap &config, ConfigMap &schema)
    {
        for (auto const &[key, value] : config)
        {
            if (not schema.hasKey(key))
            {
                std::cerr << "ConfigSchema::has_extra_keys: Extra key \"" << key << "\" is not allowed" << std::endl;;
                return true;
            }
            // obj{}
            if (value.isMap())
            {
                ConfigMap sub_schema = schema[key];
                sub_schema.erase("type");
                if (has_extra_keys(value, sub_schema))
                {
                    return true;
                }
            }
            // [obj{}, obj{}, obj{}]
            else if (value.isVector() and schema[key]["contains"]["type"] == "object")
            {
                for (ConfigItem &obj : value)
                {
                    ConfigMap sub_schema = schema[key]["contains"];
                    sub_schema.erase("type");
                    if (has_extra_keys(obj, sub_schema))
                    {
                        return true;
                    }
                }
            }
        }
        return false;
    }

    bool ConfigSchema::validate_types(ConfigMap &config)
    {
        for (auto const &[key, value] : m_schema)
        {
            // Check if we have a type field in schema, its mandatory..
            if (not value.hasKey("type"))
            {
                std::cerr << "ConfigSchema::validate_types: Missing schema type for \"" << key << '\"' << std::endl;
                return false;
            }

            // Check first if the desired schema type is known
            if (not is_known_type(value["type"]))
            {
                std::cerr << "ConfigSchema::validate_types: Invalid schema type " << (std::string)value["type"] << " in \"" << key << '\"' << std::endl;
                return false;
            }
            // Check if the type defined in the schema is matching the type we have in config
            if (not has_corresponding_type(config[key], value["type"]))
            {
                std::cerr << "ConfigSchema::validate_types: Invalid value for \"" << key << "\", expected \"" << (std::string)value["type"] << '\"' << std::endl;
                return false;
            }
            // Check constraints
            if (not validate_constraints(config, key, value))
            {
                return false;
            }

            // Validate sub objects (recursively) obj{obj{}...}
            if (config[key].isMap())
            {
                ConfigMap sc = value;
                sc.erase("type");
                ConfigSchema sub_schema(sc);
                if (not sub_schema.validate_types(config[key]))
                    return false;
            }

            // Validate sub objects within the array if any [obj{}, obj{}, ...]
            if (config[key].isVector())
            {
                if (not value.hasKey("contains"))
                {
                    std::cerr << "ConfigSchema::validate_types: Missing \"contains\" field in array \"" << key << '\"' << std::endl;
                    return false;
                }
                if (not value["contains"].hasKey("type"))
                {
                    std::cerr << "ConfigSchema::validate_types: Missing \"type\" field in array \"" << key << '\"' << std::endl;
                    return false;
                }
                for (auto &o : config[key])
                {
                    if (value["contains"]["type"] == "object")
                    {
                        ConfigMap sc = value["contains"];
                        sc.erase("type");
                        ConfigSchema sub_schema(sc);
                        if (not sub_schema.validate_types(o))
                            return false;
                    }
                    else
                    {
                        if (not has_corresponding_type(o, value["contains"]["type"]))
                        {
                            std::cerr << "ConfigSchema::validate_types: Invalid type for \"" << key << '\"' << std::endl;
                            return false;
                        }
                    }
                }
            }
        }
        return true;
    }

    bool ConfigSchema::validate_constraints(ConfigMap &config, const std::string &key, ConfigMap &schema)
    {
        // minimum and maximum:
        if (schema.hasKey("minimum") or schema.hasKey("maximum"))
        {
            // Check atom constraints, floating points and integers
            if (schema["type"] != "number" and schema["type"] != "integer")
            {
                std::cerr << "ConfigSchema::validate_constraints: Invalid minimum,maximum schema in non-atom type in \"" << key << '\"' << std::endl;
                return false;
            }

            // Handle minimum constrains
            double minimum, maximum;
            if (schema.hasKey("minimum"))
                minimum = schema["minimum"].getOrCreateAtom()->getType() == ConfigAtom::DOUBLE_TYPE ? (double)schema["minimum"] : static_cast<double>((int)(schema["minimum"]));
            if (schema.hasKey("maximum"))
                maximum = schema["maximum"].getOrCreateAtom()->getType() == ConfigAtom::DOUBLE_TYPE ? (double)schema["maximum"] : static_cast<double>((int)(schema["maximum"]));

            // Check if the config item's value is in range [minimum, maximum]
            double value = config[key].getOrCreateAtom()->getType() == ConfigAtom::DOUBLE_TYPE ? (double)config[key] : static_cast<double>((int)(config[key]));
            if (schema.hasKey("minimum") and schema.hasKey("maximum"))
            {
                // Check if minimum is less than maximum
                if (minimum > maximum)
                {
                    std::cerr << "ConfigSchema::validate_constraints: minimum value is greater than the maximum value in \"" << key << '\"' << std::endl;
                    return false;
                }
                if (value < minimum or value > maximum)
                {
                    std::cerr << "ConfigSchema::validate_constraints: value " << value << " is out of range [" << minimum << ", " << maximum << "] in \"" << key << '\"' << std::endl;
                    return false;
                }
            }
            else if (schema.hasKey("minimum") and not schema.hasKey("maximum"))
            {
                if (value < minimum)
                {
                    std::cerr << "ConfigSchema::validate_constraints: value " << value << " is out of range [" << minimum << ", " << std::numeric_limits<double>::maximum() << "] in \"" << key << '\"' << std::endl;
                    return false;
                }
            }
            else if (schema.hasKey("maximum") and not schema.hasKey("minimum"))
            {
                if (value > maximum)
                {
                    std::cerr << "ConfigSchema::validate_constraints: value " << value << " is out of range [" << std::numeric_limits<double>::minimum() << ", " << maximum << "] in \"" << key << '\"' << std::endl;
                    return false;
                }
            }
        }

        //
        return true;
    }

}
// #if 0
// bool ConfigMap::validate(ConfigMap &schema)
//   {
//     ConfigSchema cs(schema);
//      ConfigMap &obj = *this;
//     return cs.validate(obj);

//     ConfigMap &obj = *this;
//     // 1. Check for null
//     if (obj.empty())
//       return false;
//     for (auto const &[key, value] : schema)
//     {
//       // 2. Check validity of keys in schema and our obj
//       if (!obj.hasKey(key))
//       {
//         std::fprintf(stderr, "ConfigMap::validate failed: Missing schema key \"%s\"\n", key.c_str());
//         return false;
//       }

//       // 3. Check validity for the corresponding type
//       if (!value.isMap())
//       {
//         std::fprintf(stderr, "ConfigMap::validate failed: Missing schema definition for \"%s\"\n", key.c_str());
//         return false;
//       }
//       if (!value.hasKey("type"))
//       {
//         std::fprintf(stderr, "ConfigMap::validate failed: Missing schema type for \"%s\"\n", key.c_str());
//         return false;
//       }

//       std::string desired_type = (std::string)value["type"];
//       if (desired_type == "string")
//       {
//         // if(obj[key].getOrCreateAtom()->getType() != ConfigAtom::STRING_TYPE)
//         if (not obj[key].getOrCreateAtom()->testType(ConfigAtom::STRING_TYPE))
//         {
//           std::fprintf(stderr, "ConfigMap::validate failed: Wrong type for \"%s\"\n", key.c_str());
//           return false;
//         }
//       }
//       else if (desired_type == "number")
//       {
//         switch (obj[key].getOrCreateAtom()->getType())
//         {
//         case ConfigAtom::INT_TYPE:
//         case ConfigAtom::DOUBLE_TYPE:
//         case ConfigAtom::ULONG_TYPE:
//         case ConfigAtom::UINT_TYPE:
//           break;

//         default:
//           std::fprintf(stderr, "ConfigMap::validate failed: Wrong type for \"%s\"\n", key.c_str());
//           return false;
//         }
//       }
//       else if (desired_type == "integer")
//       {
//         switch (obj[key].getOrCreateAtom()->getType())
//         {
//         case ConfigAtom::INT_TYPE:
//         case ConfigAtom::ULONG_TYPE:
//         case ConfigAtom::UINT_TYPE:
//           break;

//         default:
//           std::fprintf(stderr, "ConfigMap::validate failed: Wrong type for \"%s\"\n", key.c_str());
//           return false;
//         }
//       }
//       else if (desired_type == "boolean")
//       {
//         if (obj[key].getOrCreateAtom()->getType() != ConfigAtom::BOOL_TYPE)
//         {
//           std::fprintf(stderr, "ConfigMap::validate failed: Wrong type for \"%s\"\n", key.c_str());
//           return false;
//         }
//       }
//       else if (desired_type == "object")
//       {
//         if (!obj[key].isMap())
//         {
//           std::fprintf(stderr, "ConfigMap::validate failed: Wrong type for \"%s\"\n", key.c_str());
//           return false;
//         }
//         else if (not((ConfigMap &)obj[key]).validate(value[key]))
//           return false;
//       }
//       else if (desired_type == "array")
//       {
//         if (!obj[key].isVector())
//         {
//           std::fprintf(stderr, "ConfigMap::validate failed: Wrong type for \"%s\"\n", key.c_str());
//           return false;
//         }
//         else
//         {
//           // now, we have an array, lets validate its elements
//           if (!value.hasKey("contains"))
//           {
//             std::fprintf(stderr, "ConfigMap::validate failed: Missing field contains in array \"%s\"\n", key.c_str());
//             return false;
//           }
//           // validate each array element with the desired schema type
//           for (ConfigItem &element : (ConfigVector&)obj[key])
//           {
//             if(value["contains"].getString() == "object")
//             {
//               ConfigMap sub_schema = value;
//               sub_schema.erase("type");
//               sub_schema.erase("contains");
//               if (not ((ConfigMap&)element).validate(sub_schema))
//               {
//                 std::fprintf(stderr, "ConfigMap::validate failed: Missing field contains in array \"%s\"\n", key.c_str());
//                 return false;
//               }
//             }
//             else {
//               // TODO: create a function that verifies a specifc item if its valid
//             }
//           }
//         }
//       }
//       else if (desired_type == "enum")
//       {
//         //TODO
//         throw std::runtime_error("Not implemented yet ");
//             /*
// schema
// "domain" == value
//   "type": "enum",
//   "values": ["HARDWARE", "SOFTWARE"]
// }

// obj
// {
//   "domain": "SOFTWARE"  == obj[key]
// }
//         */
//         if (not value.hasKey("values"))
//         {
//           std::fprintf(stderr, "ConfigMap::validate failed: Missing field values in enum \"%s\"\n", key.c_str());
//           return false;
//         }
//         //std::cout << "values: " << value["values"].toYamlString() << std::endl;
//         //std::cout << "obj[key]: " << obj[key].toYamlString() << std::endl;
//         bool found = false;
//         // ConfigVector::iterator it = value["values"].begin();
//         // for (; it != value["values"].end(); it++) {
//         //   if(*it.base() == o)
//         //   funcs[0].arguments.push_back(it.base()->getString());
//         // }

//         for(auto& v : (ConfigVector&)value["values"]){
//           if((std::string)v == (std::string)obj[key]) {
//             found = true;
//             break;
//           }
//         }

//         if(not found){
//            std::fprintf(stderr, "ConfigMap::validate failed: Invalid enum value %s for \"%s\"\n", obj[key].c_str(), key.c_str());
//             return false;
//         }

//         // if(not std::any_of(((ConfigVector&)value["values"]).begin(),
//         // ((ConfigVector)value["values"]).end(), obj[key]))
//         // {
//         //   std::fprintf(stderr, "ConfigMap::validate failed: Invalid enum value %s for \"%s\"\n", obj[key].c_str(), key.c_str());
//         //     return false;

//         // }
//         // Check if enum value exists in schema values
//         //cc
//         // {
//         //   std::fprintf(stderr, "ConfigMap::validate failed: Invalid enum value for \"%s\"\n", key.c_str());
//         //   return false;
//         // }
//       }
//       else
//       {
//         std::fprintf(stderr, "ConfigMap::validate failed: Invalid schema type for \"%s\"\n", key.c_str());
//         return false;
//       }
//     }
//     // 3.
//     // 4.

//     /*
//     types:
//     string - represents a string/text, e.g. "a string", "other string"
//     number - represents an integer or a float, e.g. -5, 10, -5.8, 10.2
//     integer - represents an integer, e.g. -100, 125, 0
//     boolean - represents a boolean value, e.g. true or false
//     null - indicates that a value is missing, e.g. null
//     object - a key-value map, where the key must be a string and the value can be any type, e.g. {"key": "value", "other-key": 5}
//     array - an ordered list of any data types, e.g. [1, -2.5, "some string", null]

//     */
//     /*

// Obj
//     {
//      "name": "Bridha",
//      "age": 100,
//      "weight": 650.0
//      "isSleepy": true,
//      "contact":{
//        "email": "qsdqsd@hmail",
//        "phone": "06452415"
//      },
//      "degrees": [
//        1,
//        5,
//        9
//      ],
//      "addresses":
//      [
//       {
//         "street":10,
//         "house": "1"
//       },
//       {
//         ""
//       }
//      ]
//     }
//             return obj[key].validate(value[key]);

//  Schema of the obj above
//      {
//        "name": {
//          "type": "string",
//        },
//        "age": {
//          "type": "integer",
//          "minimum": 0,
//          "maximum": 100
//        },
//        "weight": {
//          "type": "number"
//        },
//        "isSleepy":{
//          "type": "boolean"
//        },
//        "contact":{
//          "type": "object",
//          "name": {
//            "type": "string",
//          },
//          "phone": {
//            "type": "string",
//          }
//        },
//        "degrees":{
//          "type": "array",
//           "contains": {
//                "type": "integer"
//              }
//        }
//      }

//     */

//     return true;
//   }
// } // end of namespace configmaps
// #endif