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

#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include <algorithm>
#include <cctype>
#include <cstring>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>
#include "../lib/json.hpp"

using json = nlohmann::json;

#define lint uint64_t  // Long Int
#define uint uint32_t  // Unsigned Int
#define sint uint16_t  // Short Int
#define bint uint8_t   // Byte Int
#define uchar unsigned char

#define ENDL "\r\n"
#define HEADER_TERMINATOR "\r\n\r\n"

#define FOREVER while (1)

// Settings
#define BUFLEN 8192     // Response buffer size
#define HIDE_PASS false // Hide password input

/**
 * @brief Check if the condition is met. If it doesn't, print message and exit
 */
#define MUST(condition, message) \
    if (!(condition)) {          \
        std::cerr << message;    \
        exit(-1);                \
    }

/**
 * @brief Check if the error happens. If it does, print it
 */
#define CERR(condition)                                   \
    if (condition) {                       \
        std::cerr << __FILE__ << ", " << __LINE__ << ": " \
                  << std::strerror(errno) << "\n";        \
    }

/**
 * @brief DNS Lookup to find the associated to the hostname
 * @param hostname The hostname
 * @param port The port
 * @return in_addr The address of the host
 */
in_addr getIpFromHostname(std::string hostname, int port) {
    addrinfo *res;
    std::string service = std::to_string(port);

    getaddrinfo(hostname.c_str(), service.c_str(), nullptr, &res);
    MUST(res->ai_family == AF_INET, "Invalid ip adress!\n");

    sockaddr_in addr = *(sockaddr_in *)res->ai_addr;
    freeaddrinfo(res);
    return addr.sin_addr;
}

/**
 * @brief Check if the string is a positive integer
 * @param s A string
 * @return true The string represents a positive integer
 * @return false The string doesn't represent a positive integer
 */
bool is_uint(const std::string &s) {
    std::size_t found = s.find_first_not_of("0123456789");
    if (found != std::string::npos) {
        return false;
    }
    return true;
}

/**
 * @brief A simple way to disting good http response codes (2xx) from
 * bad ones (4xx, 5xx)
 * @param code The response/status code
 * @return true Is a success code
 * @return false It isn't a sucess code
 */
bool is_code_success(const uint code) {
    if (code / 100 == 2) {
        return true;
    } else {
        return false;
    }
}