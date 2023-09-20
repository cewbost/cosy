#include <libcosy/mock/attribute_type.hpp>
#include <libcosy/record_type.hpp>

#include <catch2/catch_test_macros.hpp>

#include <stdexcept>

TEST_CASE("RecordType::RecordType", "[RecordType]") {
  AttributeTypeMock atm {"type mock", 1, 1};
  REQUIRE_THROWS_AS(RecordType({
    Attribute{.name = "attr 1", .type = &atm},
    Attribute{.name = "",       .type = &atm},
    Attribute{.name = "attr 3", .type = &atm},
  }), std::invalid_argument);
  REQUIRE_THROWS_AS(RecordType({
    Attribute{.name = "attr 1", .type = &atm},
    Attribute{.name = "attr 2", .type = &atm},
    Attribute{.name = "attr 1", .type = &atm},
  }), std::invalid_argument);
}

TEST_CASE("RecordType::size", "[RecordType]") {
  AttributeTypeMock atm1x1 {"type mock 1x1", 1, 1};
  AttributeTypeMock atm2x1 {"type mock 1x2", 2, 1};
  AttributeTypeMock atm4x1 {"type mock 1x4", 4, 1};
  AttributeTypeMock atm1x4 {"type mock 4x4", 4, 4};

  REQUIRE(RecordType{
    Attribute{.name = "attr 1", .type = &atm1x1},
    Attribute{.name = "attr 2", .type = &atm1x1},
    Attribute{.name = "attr 3", .type = &atm1x1},
    Attribute{.name = "attr 4", .type = &atm2x1},
    Attribute{.name = "attr 5", .type = &atm2x1},
    Attribute{.name = "attr 6", .type = &atm4x1},
  }.size() == 11);
  REQUIRE(RecordType{
    Attribute{.name = "attr 1", .type = &atm1x4},
    Attribute{.name = "attr 2", .type = &atm2x1},
    Attribute{.name = "attr 3", .type = &atm2x1},
    Attribute{.name = "attr 4", .type = &atm1x1},
  }.size() == 12);
}
