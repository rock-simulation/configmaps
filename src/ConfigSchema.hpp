#pragma once
#include <map>
#include <vector>
#include "ConfigMap.hpp"

namespace configmaps
{
    class ConfigAtom;
    class ConfigItem;
    class ConfigVector;
    
    /**
     * @brief Represents a schema
     * Allowed schema types & constraints:
     *  Types:
     *      - integer (for integers)
     *      - number (for floating points and integers)
     *      - object (for maps)
     *      - array (for lists)
     * Constraints: 
     *      - min (set min value for integer or number)
     *      - max (set max value for integer or number)
     * TODO: enum, min-maxLength, min-maxElements,
     */
    class ConfigSchema
    {
    public:
        /**
         * @brief Construct a new ConfigSchema object by 
         * ConfigMap of schema structure 
         */
        explicit ConfigSchema(const ConfigMap &schema);
        ConfigSchema() ;
        
         /**
         * @brief Validates a ConfigMap whether its respecting this schema or not
         * 
         * @param config: ConfigMap config data
         * @return true if the config respects this schema, otherwise false.
         */
        bool validate(ConfigMap &config);

    private:
        ConfigMap m_schema;
        static const std::map<std::string, std::vector<int>> SCHEMA_ATOM_TYPES;
        
    private:
        bool has_corresponding_type(ConfigItem &config_item, const std::string &type);

        bool is_known_type(const std::string &type);

        bool validate_keys(ConfigMap &config, ConfigMap& schema);
        bool has_extra_keys(ConfigMap &config, ConfigMap& schema);

        bool validate_types(ConfigMap &config, ConfigMap& schema);

        bool validate_constraints(ConfigMap& config_item, const std::string& key, ConfigMap& schema);
    };
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
//          "min": 0,
//          "max": 100
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