#pragma once

#include <charconv>
#include <concepts>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>

namespace core {

inline std::string_view trim(std::string_view text) {
    constexpr std::string_view whitespace = " \t\r\n";
    const auto first_non_whitespace = text.find_first_not_of(whitespace);
    if (first_non_whitespace == std::string_view::npos)
        return {};
    return text.substr(first_non_whitespace,
                       text.find_last_not_of(whitespace) - first_non_whitespace + 1);
}

template <std::integral Integer> Integer parse_integer(std::string_view text) {
    text = trim(text);
    if (text.empty())
        throw std::invalid_argument("Expected an integer");
    Integer value{};
    const auto [parsed_end, parse_error] =
        std::from_chars(text.data(), text.data() + text.size(), value);
    if (parse_error != std::errc{} || parsed_end != text.data() + text.size()) {
        throw std::invalid_argument("Invalid integer: " + std::string(text));
    }
    return value;
}

} // namespace core
