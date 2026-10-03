#pragma once

#include <charconv>
#include <concepts>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>

namespace core {

inline std::string_view UsTrim(std::string_view usText) {
    constexpr std::string_view rgchWhitespace = " \t\r\n";
    const auto offFirst = usText.find_first_not_of(rgchWhitespace);
    if (offFirst == std::string_view::npos)
        return {};
    return usText.substr(offFirst, usText.find_last_not_of(rgchWhitespace) - offFirst + 1);
}

template <std::integral Val> Val ValParseInteger(std::string_view usText) {
    usText = UsTrim(usText);
    if (usText.empty())
        throw std::invalid_argument("Expected an integer");
    Val valParsed{};
    const auto [pchLim, errParse] =
        std::from_chars(usText.data(), usText.data() + usText.size(), valParsed);
    if (errParse != std::errc{} || pchLim != usText.data() + usText.size()) {
        throw std::invalid_argument("Invalid integer: " + std::string(usText));
    }
    return valParsed;
}

} // namespace core
