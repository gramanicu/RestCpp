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

#include "Request.hpp"
#include "Utils.hpp"

/**
 * @brief This class is used to process a HTTP/1.1 response
 */
class Response {
   private:
    uint code;
    Cookie session_id;
    json data_j;
    std::string jwt_token;

   public:
    Response(const std::string& response) {
        std::vector<std::string> tokens;
        std::string data;

        std::size_t curr, prev = 0;
        curr = response.find(ENDL);
        while (curr != std::string::npos) {
            tokens.push_back(response.substr(prev, curr - prev));
            prev = curr + 1;
            curr = response.find(ENDL, prev);
        }

        // Extract first line of the header (version, code...)
        std::istringstream iss(tokens[0]);
        std::vector<std::string> status(
            (std::istream_iterator<std::string>(iss)),
            std::istream_iterator<std::string>());

        code = std::stoi(status[1]);
        tokens.erase(tokens.begin());

        // Extract data
        curr = response.find(HEADER_TERMINATOR);
        data = response.substr(curr + sizeof(HEADER_TERMINATOR) - 1);

        bool hasData = false;
        bool isJson = false;

        for (auto& token : tokens) {
            std::size_t pos;
            std::string val;

            if ((pos = token.find("connect.sid=")) != std::string::npos) {
                val = token.substr(pos + sizeof("connect.sid=") - 1);
                val = val.substr(0, val.find(';'));

                session_id.set_key("connect.sid");
                session_id.set_value(val);
            } else if ((pos = token.find("Content-Length: ")) !=
                       std::string::npos) {
                val = token.substr(pos + sizeof("Content-Length: ") - 1);

                if (val != "0") {
                    hasData = true;
                }
            } else if ((pos = token.find("Content-Type: ")) !=
                       std::string::npos) {
                std::string val =
                    token.substr(pos + sizeof("Content-Type: ") - 1);
                val = val.substr(0, val.find(';'));

                if (val == "application/json") {
                    isJson = true;
                }
            }
        }

        if(data == "Too many requests, please try again later.") {
            data_j["error"] = "Too many requests, please try again later.";
        } else 
        // If the content-length is set and we actually have data
        if (hasData && data.size() != 0) {
            if (isJson) {
                data_j = json::parse(data);
            } else {
                // Parse into key-value vector
            }
        }
    }

    uint get_response_code() const { return code; }

    Cookie& get_session_id() { return session_id; }

    json& get_json_data() { return data_j; }
};