/*
 * Copyright (C) 2014-2017  Andrew Gunnerson <andrewgunnerson@gmail.com>
 *
 * This file is part of DualBootPatcher
 *
 * DualBootPatcher is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * DualBootPatcher is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with DualBootPatcher.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "mbutil/string.h"

#include <algorithm>
#include <functional>
#include <memory>

#include <cstdarg>
#include <cstring>

namespace mb::util
{

static void replace_internal(std::string &source,
                             const std::string &from, const std::string &to,
                             bool replace_first_only)
{
    if (from.empty()) {
        return;
    }

    std::size_t pos = 0;
    while ((pos = source.find(from, pos)) != std::string::npos) {
        source.replace(pos, from.size(), to);
        if (replace_first_only) {
            return;
        }
        pos += to.size();
    }
}

void replace(std::string &source,
             const std::string &from, const std::string &to)
{
    replace_internal(source, from, to, true);
}

void replace_all(std::string &source,
                 const std::string &from, const std::string &to)
{
    replace_internal(source, from, to, false);
}

std::vector<std::string> tokenize(const std::string &str,
                                  const std::string &delims)
{
    std::vector<char> linebuf(str.begin(), str.end());
    linebuf.resize(linebuf.size() + 1);
    std::vector<std::string> tokens;

    char *temp;
    char *token;

    token = strtok_r(linebuf.data(), delims.c_str(), &temp);
    while (token != nullptr) {
        tokens.emplace_back(token);

        token = strtok_r(nullptr, delims.c_str(), &temp);
    }

    return tokens;
}

template<typename Iterator>
static Iterator find_first_non_space(Iterator begin, Iterator end)
{
    return std::find_if(begin, end, [](char c) {
        return !std::isspace(c);
    });
}

/*!
 * \brief Trim whitespace from the left (in place)
 */
void trim_left(std::string &s)
{
    s.erase(s.begin(), find_first_non_space(s.begin(), s.end()));
}

/*!
 * \brief Trim whitespace from the right (in place)
 */
void trim_right(std::string &s)
{
    s.erase(find_first_non_space(s.rbegin(), s.rend()).base(), s.end());
}

/*!
 * \brief Trim whitespace from the left and the right (in place)
 */
void trim(std::string &s)
{
    trim_left(s);
    trim_right(s);
}

/*!
 * \brief Trim whitespace from the left (one copy)
 */
std::string trimmed_left(const std::string &s)
{
    return {find_first_non_space(s.begin(), s.end()), s.end()};
}

/*!
 * \brief Trim whitespace from the right (one copy)
 */
std::string trimmed_right(const std::string &s)
{
    return {s.begin(), find_first_non_space(s.rbegin(), s.rend()).base()};
}

/*!
 * \brief Trim whitespace from the left and the right (one copy)
 */
std::string trimmed(const std::string &s)
{
    auto begin = find_first_non_space(s.begin(), s.end());
    auto search_end = std::string::const_reverse_iterator(begin);
    auto end = find_first_non_space(s.rbegin(), search_end);
    return {begin, end.base()};
}

/*!
 * \brief Convert binary data to its hex string representation
 *
 * The size of the output string should be at least `2 * size + 1` bytes.
 * (Two characters in string for each byte in the binary data + one byte for
 * the NULL terminator.)
 *
 * \param data Binary data
 * \param size Size of binary data
 */
std::string hex_string(const unsigned char *data, size_t size)
{
    static const char digits[] = "0123456789abcdef";

    std::string result;
    result.resize(size * 2);

    for (unsigned int i = 0; i < size; ++i) {
        result[2 * i] = digits[(data[i] >> 4) & 0xf];
        result[2 * i + 1] = digits[data[i] & 0xf];
    }

    return result;
}

char ** dup_cstring_list(const char * const *list)
{
    char **copy = nullptr;

    size_t items;
    for (items = 0; list[items]; ++items);
    size_t size = (items + 1) * sizeof(list[0]);

    copy = static_cast<char **>(malloc(size));
    if (!copy) {
        return nullptr;
    }
    memset(copy, 0, size);

    for (size_t i = 0; i < items; ++i) {
        copy[i] = strdup(list[i]);
        if (!copy[i]) {
            free_cstring_list(copy);
            return nullptr;
        }
    }
    copy[items] = nullptr;

    return copy;
}

void free_cstring_list(char **list)
{
    // Free environ copy
    if (list) {
        for (char **ptr = list; *ptr; ++ptr) {
            free(*ptr);
        }
        free(list);
    }
}

}
