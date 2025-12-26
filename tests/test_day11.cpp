#include <gtest/gtest.h>
#include "days/day11.h"

static std::vector<std::string> split_lines(const std::string& s) {
    std::vector<std::string> out;
    std::istringstream iss(s);
    std::string line;
    while (std::getline(iss, line)) {
        if (!line.empty()) {
            out.push_back(line);
        }
    }
    return out;
}

static const char* example_part1 = R"(
aaa: you hhh
you: bbb ccc
bbb: ddd eee
ccc: ddd eee fff
ddd: ggg
eee: out
fff: out
ggg: out
hhh: ccc fff iii
iii: out
)";

static const char* example_part2 = R"(
svr: aaa bbb
aaa: fft
fft: ccc
bbb: tty
tty: ccc
ccc: ddd eee
ddd: hub
hub: fff
eee: dac
dac: fff
fff: ggg hhh
ggg: out
hhh: out
)";

TEST(Day11, ExamplePart1) {
    Day11 d;
    d.set_input(split_lines(example_part1));
    EXPECT_EQ(d.part1(), "5");
}

TEST(Day11, ExamplePart2) {
    Day11 d;
    d.set_input(split_lines(example_part2));
    EXPECT_EQ(d.part2(), "2");
}