/**
 * Copyright (c) 2020 Grama Nicolae
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#include "Utils.hpp"

class Cookie {
   private:
    std::string key;
    std::string value;

   public:
    Cookie() : key(""), value(""){};
    Cookie(const std::string& key, const std::string& value)
        : key(key), value(value) {}

    friend std::ostream& operator<<(std::ostream& out, const Cookie& cookie) {
        out << cookie.key << "=" << cookie.value;
        return out;
    }
};

/**
 * @brief Create a HTTP/1.1 GET request
 * @param host The hostname
 * @param url The url
 * @param query_params The query parameters
 * @param cookies A list of cookies
 * @return std::string The request
 */
std::string create_get_request(
    std::string host, std::string url, std::string query_params = "",
    std::vector<Cookie> cookies = std::vector<Cookie>()) {
    std::stringstream ss;
    if (query_params.size() != 0) {
        ss << "GET " << url << "?" << query_params << " HTTP/1.1" << ENDL;
    } else {
        ss << "GET " << url << " HTTP/1.1" << ENDL;
    }

    ss << "Host: " << host << ENDL;

    if (cookies.size() != 0) {
        ss << "Cookie: ";
        uint i = 1;
        for (auto& cookie : cookies) {
            ss << cookie;
            if (i++ != cookies.size()) {
                ss << "; ";
            }
        }
        ss << ENDL;
    }
    ss << ENDL;
    return ss.str();
}

// POST
// DELETE