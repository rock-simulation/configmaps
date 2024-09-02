#include "catch.hpp"
#include "ConfigMap.hpp"
#include "ConfigAtom.hpp"
#include "ConfigVector.hpp"
#include "ConfigSchema.hpp"
#include <iostream>
using namespace configmaps;

TEST_CASE("ConfigMap", "boolean")
{

    ConfigMap map;
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

TEST_CASE("min_max_tests", "checks valid and invalid values for integers and numbers regarding minium and maximum values")
{
    ConfigMap schema = ConfigMap::fromYamlFile("schema/min_max_test_schema.yaml");
    ConfigSchema cs(schema);
    std::cout << "Schema:\n" << schema.toYamlString() << std::endl << std::endl;


    { // valids
        ConfigMap config;
        config["only_min_number_intmin"] = 15.5;
        config["only_min_number_doublemin"] = 15.5;
        config["only_min_int_intmin"] = 15;
        config["only_min_int_doublemin"] = 16;
        config["only_max_number_intmax"] = 1.0;
        config["only_max_number_doublemax"] = 1.5;
        config["only_max_int_intmax"] = 1;
        config["only_max_int_doublemax"] = 1;
        config["both_number_intvals"] = 7.0;
        config["both_number_doublevals"] = 7.5;
        config["both_int_intvals"] = 7;
        config["both_int_doublevals"] = 7;
        std::cout << "Config:\n" << config.toYamlString() << std::endl;

        REQUIRE(cs.validate(config));
    }

    { // valid min boundaries
        ConfigMap config;
        config["only_min_number_intmin"] = 5.0;
        config["only_min_number_doublemin"] = 5.5;
        config["only_min_int_intmin"] = 5;
        config["only_min_int_doublemin"] = 6;
        config["both_number_intvals"] = 5.0;
        config["both_number_doublevals"] = 5.5;
        config["both_int_intvals"] = 5;
        config["both_int_doublevals"] = 6;
        std::cout << "Config:\n" << config.toYamlString() << std::endl;

        REQUIRE(cs.validate(config));
    }

    { // valid max boundaries
        ConfigMap config;
        config["only_max_number_intmax"] = 5.0;
        config["only_max_number_doublemax"] = 5.5;
        config["only_max_int_intmax"] = 5;
        config["only_max_int_doublemax"] = 5;
        config["both_number_intvals"] = 10.0;
        config["both_number_doublevals"] = 10.5;
        config["both_int_intvals"] = 10;
        config["both_int_doublevals"] = 10;
        std::cout << "Config:\n" << config.toYamlString() << std::endl;

        REQUIRE(cs.validate(config));
    }

    { // invalid min number int
        ConfigMap config;
        config["only_min_number_intmin"] = 4.9;
        std::cout << "Config:\n" << config.toYamlString() << std::endl;

        REQUIRE(!cs.validate(config));
    }

    { // invalid min number double
        ConfigMap config;
        config["only_min_number_doublemin"] = 5.4;
        std::cout << "Config:\n" << config.toYamlString() << std::endl;

        REQUIRE(!cs.validate(config));
    }

    { // invalid min int int
        ConfigMap config;
        config["only_min_int_intmin"] = 4;
        std::cout << "Config:\n" << config.toYamlString() << std::endl;

        REQUIRE(!cs.validate(config));
    }

    { // invalid min int double
        ConfigMap config;
        config["only_min_int_doublemin"] = 5;
        std::cout << "Config:\n" << config.toYamlString() << std::endl;

        REQUIRE(!cs.validate(config));
    }

    { // invalid max number int
        ConfigMap config;
        config["only_max_number_intmax"] = 5.1;
        std::cout << "Config:\n" << config.toYamlString() << std::endl;

        REQUIRE(!cs.validate(config));
    }

    { // invalid max number double
        ConfigMap config;
        config["only_max_number_doublemax"] = 5.6;
        std::cout << "Config:\n" << config.toYamlString() << std::endl;

        REQUIRE(!cs.validate(config));
    }

    { // invalid max int int
        ConfigMap config;
        config["only_max_int_intmax"] = 6;
        std::cout << "Config:\n" << config.toYamlString() << std::endl;

        REQUIRE(!cs.validate(config));
    }

    { // invalid max int double
        ConfigMap config;
        config["only_max_int_doublemax"] = 6;
        std::cout << "Config:\n" << config.toYamlString() << std::endl;

        REQUIRE(!cs.validate(config));
    }

    { // invalid both number int low
        ConfigMap config;
        config["both_number_intvals"] = 4.9;
        std::cout << "Config:\n" << config.toYamlString() << std::endl;

        REQUIRE(!cs.validate(config));
    }

    { // invalid both number double low
        ConfigMap config;
        config["both_number_doublevals"] = 5.4;
        std::cout << "Config:\n" << config.toYamlString() << std::endl;

        REQUIRE(!cs.validate(config));
    }

    { // invalid both int int low
        ConfigMap config;
        config["both_int_intvals"] = 4;
        std::cout << "Config:\n" << config.toYamlString() << std::endl;

        REQUIRE(!cs.validate(config));
    }

    { // invalid both int double low
        ConfigMap config;
        config["both_int_doublevals"] = 5;
        std::cout << "Config:\n" << config.toYamlString() << std::endl;

        REQUIRE(!cs.validate(config));
    }

    { // invalid both number int high
        ConfigMap config;
        config["both_number_intvals"] = 10.1;
        std::cout << "Config:\n" << config.toYamlString() << std::endl;

        REQUIRE(!cs.validate(config));
    }

    { // invalid both number double high
        ConfigMap config;
        config["both_number_doublevals"] = 10.6;
        std::cout << "Config:\n" << config.toYamlString() << std::endl;

        REQUIRE(!cs.validate(config));
    }

    { // invalid both int int high
        ConfigMap config;
        config["both_int_intvals"] = 11;
        std::cout << "Config:\n" << config.toYamlString() << std::endl;

        REQUIRE(!cs.validate(config));
    }

    { // invalid both int double high
        ConfigMap config;
        config["both_int_doublevals"] = 11;
        std::cout << "Config:\n" << config.toYamlString() << std::endl;

        REQUIRE(!cs.validate(config));
    }
}

