#include <libcosy/integral_set.hpp>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("IntegralSet: construction by segments", "[IntegralSet]") {
  SECTION("segment order") {
    REQUIRE(
      IntegralSet<int>{{7, 8}, {1, 2}, {4, 5}} ==
      IntegralSet<int>{{1, 2}, {4, 5}, {7, 8}}
    );
  }
  SECTION("merging adjacent segments") {
    REQUIRE(
      IntegralSet<int>{{1, 2}, {8, 9}, {11, 12}, {3, 4}, {6, 7}, {13, 14}} ==
      IntegralSet<int>{{1, 4}, {6, 9}, {11, 14}}
    );
  }
  SECTION("merging overlapping segments") {
    REQUIRE(
      IntegralSet<int>{
        {1, 3}, {6, 7}, {12, 14}, {18, 19}, {22, 23},
        {2, 4}, {6, 9}, {11, 13}, {16, 19}, {21, 24},
      } == IntegralSet<int>{
        {1, 4}, {6, 9}, {11, 14}, {16, 19}, {21, 24},
      }
    );
  }
  SECTION("joining segments") {
    REQUIRE(
      IntegralSet<int>{
        {2, 4}, {6, 8}, {5, 5},
        {12, 14}, {16, 18}, {13, 17},
        {22, 24}, {26, 28}, {21, 28},
        {32, 34}, {36, 38}, {32, 39},
      } == IntegralSet<int>{
        {2, 8}, {12, 18}, {21, 28}, {32, 39}}
    );
  }
  SECTION("joining multiple segments") {
    REQUIRE(
      IntegralSet<int>{{2, 4}, {6, 8}, {10, 12}, {3, 11}} ==
      IntegralSet<int>{{2, 12}});
  }
}

TEST_CASE("IntegralSet: construction by elements", "[IntegralSet]") {
  SECTION("element order") {
    REQUIRE(
      IntegralSet<int>{5, 1, 3} ==
      IntegralSet<int>{1, 3, 5}
    );
  }
  SECTION("element duplicates") {
    REQUIRE(
      IntegralSet<int>{1, 3, 3, 5, 5, 5, 1, 1, 3} ==
      IntegralSet<int>{1, 3, 5}
    );
  }
  SECTION("merging segments") {
    REQUIRE(
      IntegralSet<int>{1, 7, 10, 2, 3, 6, 5, 9, 11} ==
      IntegralSet<int>{{1, 3}, {5, 7}, {9, 11}}
    );
  }
  SECTION("joining segments") {
    REQUIRE(
      IntegralSet<int>{1, 3, 5, 7, 4, 2, 6} ==
      IntegralSet<int>{{1, 7}}
    );
  }
}

TEST_CASE("IntegralSet::insert", "[IntegralSet]") {
  IntegralSet<int> set;

  set.insert(2);
  set.insert(6);
  REQUIRE(set == IntegralSet<int>{2, 6});

  set.insert(1);
  set.insert(3);
  set.insert(5);
  set.insert(7);
  REQUIRE(set == IntegralSet<int>{{1, 3}, {5, 7}});

  set.insert(4);
  REQUIRE(set == IntegralSet<int>{{1, 7}});

  set.insert(1);
  set.insert(4);
  set.insert(7);
  REQUIRE(set == IntegralSet<int>{{1, 7}});
}

TEST_CASE("IntegralSet::merge", "[IntegralSet]") {
  IntegralSet<int> set{{4, 6}, {14, 16}, {24, 26}, {34, 36}, {44, 46}};

  SECTION("merge disjoint segments") {
    set.merge(IntegralSet<int>{{8, 12}, {18, 22}, {28, 32}, {38, 42}});
    REQUIRE(set == IntegralSet<int>{
      {4, 6}, {8, 12}, {14, 16},
      {18, 22}, {24, 26}, {28, 32},
      {34, 36}, {38, 42}, {44, 46}
    });
  }
  SECTION("merge adjacent segments") {
    set.merge(IntegralSet<int>{{2, 3}, {17, 18}, {47, 48}});
    REQUIRE(set == IntegralSet<int>{{2, 6}, {14, 18}, {24, 26}, {34, 36}, {44, 48}});
  }
  SECTION("merge overlapping segments") {
    set.merge(IntegralSet<int>{{6, 8}, {14, 18}, {22, 28}, {32, 36}, {42, 44}});
    REQUIRE(set == IntegralSet<int>{{4, 8}, {14, 18}, {22, 28}, {32, 36}, {42, 46}});
  }
  SECTION("join segments") {
    set.merge(IntegralSet<int>{{7, 15}, {17, 23}, {27, 33}, {37, 43}});
    REQUIRE(set == IntegralSet<int>{{4, 46}});
  }
  SECTION("join multiple segments") {
    set.merge(IntegralSet<int>{{6, 44}});
    REQUIRE(set == IntegralSet<int>{{4, 46}});
  }
}

TEST_CASE("IntegralSet::unite", "[IntegralSet]") {
  SECTION("unite disjoint segments") {
    REQUIRE(
      IntegralSet<int>{1, 5, 9}.unite(
        IntegralSet<int>{3, 7}
      ) == IntegralSet<int>{1, 3, 5, 7, 9}
    );
  }
  SECTION("merge adjacent segments") {
    REQUIRE(
      IntegralSet<int>{2, 4, 8, 10}.unite(
        IntegralSet<int>{1, 5, 7, 11}
      ) == IntegralSet<int>{{1, 2}, {4, 5}, {7, 8}, {10, 11}}
    );
  }
  SECTION("merge overlapping segments") {
    REQUIRE(
      IntegralSet<int>{{1, 3}, {6, 7}, {12, 13}, {17, 19}, {23, 24}}.unite(
        IntegralSet<int>{{3, 4}, {6, 9}, {11, 14}, {16, 17}, {21, 24}}
      ) == IntegralSet<int>{{1, 4}, {6, 9}, {11, 14}, {16, 19}, {21, 24}}
    );
  }
  SECTION("join segments") {
    REQUIRE(
      IntegralSet<int>{1, 3, 5, 7, 9}.unite(
        IntegralSet<int>{2, 4, 6, 8}
      ) == IntegralSet<int>{{1, 9}}
    );
  }
  SECTION("join multiple segments") {
    REQUIRE(
      IntegralSet<int>{1, 3, 5, 7, 9}.unite(
        IntegralSet<int>{{2, 8}}
      ) == IntegralSet<int>{{1, 9}}
    );
  }
}

TEST_CASE("IntegralSet::intersect", "[IntegralSet]") {
  SECTION("no overlap") {
    REQUIRE(
      IntegralSet<int>{1, 4, 7, 10}.intersect(
        IntegralSet<int>{{2, 2}, {5, 6}, {9, 9}}
      ) == IntegralSet<int>{}
    );
  }
  SECTION("overlap") {
    REQUIRE(
      IntegralSet<int>{{1, 1}, {3, 3}, {5, 9}}.intersect(
        IntegralSet<int>{{1, 5}, {7, 7}, {9, 9}}
      ) == IntegralSet<int>{1, 3, 5, 7, 9}
    );
  }
}

TEST_CASE("IntegralSet::takeHead", "[IntegralSet]") {
  IntegralSet<int> set = {{1, 2}, {4, 5}, {7, 8}};

  SECTION("single elements") {
    std::vector<int> elems;
    for (int n = 0; n < 5; n++) elems.push_back(set.takeHead());

    REQUIRE(elems == std::vector<int>{1, 2, 4, 5, 7});
    REQUIRE(set == IntegralSet<int>{8});

    SECTION("out of elements") {
      set.takeHead();
      REQUIRE_THROWS_AS(set.takeHead(), std::out_of_range);
    }
  }
  SECTION("multiple elements") {
    SECTION("take to segment end") {
      REQUIRE(set.takeHead(4) == IntegralSet<int>{{1, 2}, {4, 5}});
      REQUIRE(set == IntegralSet<int>{{7, 8}});
    }
    SECTION("take to middle of segment") {
      REQUIRE(set.takeHead(3) == IntegralSet<int>{{1, 2}, {4, 4}});
      REQUIRE(set == IntegralSet<int>{{5, 5}, {7, 8}});
    }
    SECTION("take all") {
      REQUIRE(set.takeHead(6) == IntegralSet<int>{{1, 2}, {4, 5}, {7, 8}});
      REQUIRE(set == IntegralSet<int>{});
    }
    SECTION("take more than all") {
      REQUIRE(set.takeHead(7) == IntegralSet<int>{{1, 2}, {4, 5}, {7, 8}});
      REQUIRE(set == IntegralSet<int>{});
    }
  }
}

TEST_CASE("IntegralSet::take", "[IntegralSet]") {
  IntegralSet<int> set = {{1, 3}, {5, 8}, {10, 12}};

  SECTION("head") {
    REQUIRE(set.take(0, 2) == IntegralSet<int>{{1, 2}});
    REQUIRE(set == IntegralSet<int>{{3, 3}, {5, 8}, {10, 12}});
  }
  SECTION("head segment") {
    REQUIRE(set.take(0, 3) == IntegralSet<int>{{1, 3}});
    REQUIRE(set == IntegralSet<int>{{5, 8}, {10, 12}});
  }
  SECTION("head multiple segments") {
    REQUIRE(set.take(0, 5) == IntegralSet<int>{{1, 3}, {5, 6}});
    REQUIRE(set == IntegralSet<int>{{7, 8}, {10, 12}});
  }
  SECTION("half segment") {
    REQUIRE(set.take(3, 2) == IntegralSet<int>{{5, 6}});
    REQUIRE(set == IntegralSet<int>{{1, 3}, {7, 8}, {10, 12}});
  }
  SECTION("latter half segment") {
    REQUIRE(set.take(5, 2) == IntegralSet<int>{{7, 8}});
    REQUIRE(set == IntegralSet<int>{{1, 3}, {5, 6}, {10, 12}});
  }
  SECTION("remove segment") {
    REQUIRE(set.take(3, 4) == IntegralSet<int>{{5, 8}});
    REQUIRE(set == IntegralSet<int>{{1, 3}, {10, 12}});
  }
  SECTION("split segment") {
    REQUIRE(set.take(4, 2) == IntegralSet<int>{6, 7});
    REQUIRE(set == IntegralSet<int>{{1, 3}, {5, 5}, {8, 8}, {10, 12}});
  }
  SECTION("split multiple segments") {
    REQUIRE(set.take(1, 8) == IntegralSet<int>{{2, 3}, {5, 8}, {10, 11}});
    REQUIRE(set == IntegralSet<int>{1, 12});
  }
  SECTION("split tail") {
    REQUIRE(set.take(8, 2) == IntegralSet<int>{{11, 12}});
    REQUIRE(set == IntegralSet<int>{{1, 3}, {5, 8}, {10, 10}});
  }
  SECTION("drop tail") {
    REQUIRE(set.take(7, 3) == IntegralSet<int>{{10, 12}});
    REQUIRE(set == IntegralSet<int>{{1, 3}, {5, 8}});
  }
  SECTION("drop over tail") {
    REQUIRE(set.take(7, 4) == IntegralSet<int>{{10, 12}});
    REQUIRE(set == IntegralSet<int>{{1, 3}, {5, 8}});
  }
  SECTION("drop nothing") {
    REQUIRE(set.take(10, 1) == IntegralSet<int>{});
    REQUIRE(set == IntegralSet<int>{{1, 3}, {5, 8}, {10, 12}});
  }
}

TEST_CASE("IntegralSet::nth", "[IntegralSet]") {
  IntegralSet<int> set{{1, 2}, {4, 5}, {7, 8}};

  SECTION("successes") {
    REQUIRE(set.nth(0) == 1);
    REQUIRE(set.nth(1) == 2);
    REQUIRE(set.nth(2) == 4);
    REQUIRE(set.nth(3) == 5);
    REQUIRE(set.nth(4) == 7);
    REQUIRE(set.nth(5) == 8);
  }
  SECTION("failure") {
    REQUIRE_THROWS_AS(set.nth(6), std::out_of_range);
  }
}

TEST_CASE("IntegralSet::size", "[IntegralSet]") {
  REQUIRE(IntegralSet<int>{{1, 1}, {3, 4}, {6, 8}}.size() == 6);
}

TEST_CASE("IntegralSet::contains", "[IntegralSet]") {
  IntegralSet<int> set{{1, 3}, {5, 7}};
  REQUIRE(set.contains(1));
  REQUIRE(set.contains(2));
  REQUIRE(set.contains(3));
  REQUIRE(set.contains(5));
  REQUIRE(set.contains(6));
  REQUIRE(set.contains(7));
  REQUIRE(!set.contains(0));
  REQUIRE(!set.contains(4));
  REQUIRE(!set.contains(8));
}
