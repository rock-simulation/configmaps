#include "catch.hpp"
#include "ConfigMap.hpp"
#include "ConfigAtom.hpp"
#include "ConfigVector.hpp"
#include "ConfigSchema.hpp"
#include <iostream>
using namespace configmaps;

TEST_CASE("ConfigMap", "boolean")
{
    using namespace std::string_literals;

    ConfigMap map;
    map["name"] = "RAM"s;
    map["namec"] = "RAM";
    map["weight"] = 99.99;
    map["weightf"] = 99.99f;
    map["brand_new"] = true;
    map["size"] = 8;
    map["length"] = 1000u;
    map["lengthl"] = 1000ul;
    map["abstract_positive"] = "true"; 
    map["abstract_negative"] = "false"; 
    REQUIRE((bool)map["abstract_positive"] == true);
    REQUIRE((bool)map["abstract_negative"] == false);

    map["abstract_literal"] = ConfigAtom(true); 
    REQUIRE((bool)map["abstract_literal"] == true);
    map["abstract_literal"] = ConfigAtom(false);
    REQUIRE((bool)map["abstract_literal"] == false);

    ConfigMap recovered = ConfigMap::fromJsonString(map.toJsonString());
    REQUIRE((bool)recovered["abstract_positive"] == true);
    REQUIRE((bool)recovered["abstract_negative"] == false);

    std::cout << "Export Json: " << map.toJsonString() << std::endl;
    std::cout << "Import Recovered: " << recovered.toJsonString() << std::endl;
}

TEST_CASE("ConfigSchema", "validate")
{

    ConfigMap schema = ConfigMap::fromYamlFile("schema/schema_example.yaml");
    ConfigSchema cs(schema);

    ConfigMap obj;
    obj["name"] = "RAM";
    obj["weight"] = 100;
    obj["brand_new"] = true;
    obj["size"] = 8;
    obj["capabilities"] = ConfigVector();
    obj["capabilities"][0]["chip_size"] = 100;
    obj["capabilities"][0]["ddr4"] = true;
    obj["capabilities"][1]["chip_size"] = 40;
    obj["capabilities"][1]["ddr4"] = true;
    obj["design"] = ConfigMap();
    obj["design"]["color"] = "red";
    obj["design"]["brand"] = "nike";
    obj["design"]["address"] = ConfigMap();
    obj["design"]["address"]["house"] = 2;
    obj["design"]["address"]["contact"] = ConfigMap();
    obj["design"]["address"]["contact"]["email"] = "gu@gu.com";
    obj["design"]["address"]["contact"]["phone"] = "441";

    obj["ints"] = ConfigVector();
    obj["ints"].push_back(1);
    obj["ints"].push_back(2);
    obj["ints"].push_back(3);

    obj["objects"] = ConfigVector();
    obj["objects"][0]["id"] = 100;
    obj["objects"][0]["title"] = "test test";
    obj["objects"][0]["description"] = "test description";
    obj["objects"][0]["is_new"] = true;
    obj["objects"][1]["id"] = 101;
    obj["objects"][1]["title"] = "test tes2t";
    obj["objects"][1]["description"] = "test description2";
    obj["objects"][1]["is_new"] = false;
    
    REQUIRE(cs.validate(obj));
}

TEST_CASE("ODEInertia_schema", "validate"){
    ConfigMap schema = ConfigMap::fromYamlFile("schema/ODEInertial_schema.yaml");
    ConfigSchema cs(schema);
    std::cout << "Schema:\n" << schema.toYamlString() << std::endl << std::endl;

    ConfigMap config;
    config["mass"] = 2.0;
    config["inertia"]["i00"] = 5.0;
    config["inertia"]["i01"] = 3.14;
    config["inertia"]["i02"] = 3.14;
    config["inertia"]["i10"] = 3.14;
    config["inertia"]["i11"] = 3.14;
    config["inertia"]["i12"] = 3.14;
    config["inertia"]["i20"] = 3.14;
    config["inertia"]["i21"] = 3.14;
    config["inertia"]["i22"] = 3.14;
    config["design"] = ConfigMap();
    config["design"]["color"] = "red";
    config["design"]["address"] = ConfigMap();
    config["design"]["address"]["house"] = 2;
    config["design"]["address"]["contact"] = ConfigMap();
    config["design"]["address"]["contact"]["email"] = "gu@gu.com";
    config["design"]["address"]["contact"]["phone"] = "441";
    config["objects"] = ConfigVector();
    config["objects"][0]["id"] = 100;
    config["objects"][0]["title"] = "test test";
    config["objects"][0]["description"] = "test description";
    config["objects"][0]["is_new"] = true;
    config["objects"][1]["id"] = 101;
    config["objects"][1]["title"] = "test tes2t";
    config["objects"][1]["description"] = "test description2";
    config["objects"][1]["is_new"] = false;
    
    std::cout << "Config:\n" << config.toYamlString() << std::endl;

    REQUIRE(cs.validate(config));

}
