#pragma once

#include <charconv>
#include <concepts>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>

namespace core {

inline std::string_view Trim(std::string_view stringText) {
    constexpr std::string_view stringWhitespace = " \t\r\n";
    const auto uFirstNonWhitespace = stringText.find_first_not_of(stringWhitespace);
    if (uFirstNonWhitespace == std::string_view::npos)
        return {};
    return stringText.substr(uFirstNonWhitespace, stringText.find_last_not_of(stringWhitespace) -
                                                      uFirstNonWhitespace + 1);
}

template <std::integral INTEGER> INTEGER ParseInteger(std::string_view stringText) {
    stringText = Trim(stringText);
    if (stringText.empty())
        throw std::invalid_argument("Expected an integer");
    INTEGER integerValue{};
    const auto [pbszParsedEnd, eParseError] =
        std::from_chars(stringText.data(), stringText.data() + stringText.size(), integerValue);
    if (eParseError != std::errc{} || pbszParsedEnd != stringText.data() + stringText.size()) {
        throw std::invalid_argument("Invalid integer: " + std::string(stringText));
    }
    return integerValue;
}

} // namespace core
