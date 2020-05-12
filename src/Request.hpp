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

class KeyValue {
   public:
    std::string key;
    std::string value;
    KeyValue() : key(""), value(""){};
    KeyValue(const std::string& key, const std::string& value)
        : key(key), value(value) {}
};

class Cookie {
   private:
    std::string key;
    std::string value;

   public:
    Cookie() : key(""), value(""){};
    Cookie(const std::string& key, const std::string& value)
        : key(key), value(value) {}

    bool is_null() const { return key == "" && value == ""; }

    void set_key(const std::string& _key) { key = _key; }

    void set_value(const std::string& _value) { value = _value; }

    friend std::ostream& operator<<(std::ostream& out, const Cookie& cookie) {
        out << cookie.key << "=" << cookie.value;
        return out;
    }
};

/**
 * @brief Create a HTTP/1.1 GET request
 * @param host The hostname
 * @param url The url
 * @param query_params The query parameters (can be "" if not needed)
 * @param cookies A list of cookies (the can be "not specified")
 * @param jwt_token The jwt used in the connection (this isn't generically
 * implemented)
 * @return std::string The request
 */
std::string create_get_request(
    const std::string& host, const std::string& url,
    const std::string& query_params = "",
    std::vector<Cookie> cookies = std::vector<Cookie>(),
    const std::string& jwt_token = "") {
    // Start building the request
    std::stringstream ss;
    if (query_params.size() != 0) {
        ss << "GET " << url << "?" << query_params << " HTTP/1.1" << ENDL;
    } else {
        ss << "GET " << url << " HTTP/1.1" << ENDL;
    }

    ss << "Host: " << host << ENDL;
    if (jwt_token != "") {
        ss << "Authorization: Bearer " << jwt_token << ENDL;
    }

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

/**
 * @brief Create a HTTP/1.1 DELETE request
 * @param host The hostname
 * @param url The url
 * @param cookies A list of cookies (the can be "not specified")
 * @param jwt_token The jwt used in the connection (this isn't generically
 * implemented)
 * @return std::string The request
 */
std::string create_delete_request(
    const std::string& host, const std::string& url,
    std::vector<Cookie> cookies = std::vector<Cookie>(),
    const std::string& jwt_token = "") {
    // Start building the request
    std::stringstream ss;

    ss << "DELETE " << url << " HTTP/1.1" << ENDL;

    ss << "Host: " << host << ENDL;
    if (jwt_token != "") {
        ss << "Authorization: Bearer " << jwt_token << ENDL;
    }

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

/**
 * @brief Create a HTTP/1.1 POST request
 * @param host The hostname
 * @param url The url
 * @param content_type The type of the data
 * @param body_data The data (json or x-www-form-urlenconded)
 * @param cookies A list of cookies (the can be "not specified")
 * @param jwt_token The jwt used in the connection (this isn't generically
 * implemented)
 * @return std::string The request
 */
std::string create_post_request(
    const std::string& host, const std::string& url,
    const std::string& content_type, std::vector<KeyValue> body_data,
    std::vector<Cookie> cookies = std::vector<Cookie>(),
    const std::string& jwt_token = "") {
    // Start building the request
    std::stringstream ss;

    ss << "POST " << url << " HTTP/1.1" << ENDL;
    ss << "Host: " << host << ENDL;
    if (jwt_token != "") {
        ss << "Authorization: Bearer " << jwt_token << ENDL;
    }
    ss << "Content-Type: " << content_type << ENDL;

    std::stringstream body;
    if (content_type == "application/json") {
        json data;
        for (auto& kv : body_data) {
            data[kv.key] = kv.value;
        }
        body << data.dump();
    } else {
        // We suppose it is application/x-www-form-urlenconded, as these two are
        // the only two data types supported
        uint i = 1;
        for (auto& kv : body_data) {
            body << kv.key << "=" << kv.value;
            if (i++ != body_data.size()) {
                body << "&";
            }
        }
    }
    ss << "Content-Length: " << body.str().size() << ENDL;
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
    ss << body.str();
    ss << ENDL;
    return ss.str();
}

// POST
// DELETE