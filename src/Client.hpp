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

namespace RestCpp {
class Client {
   private:
    int sockfd;
    sockaddr_in serv_addr;

    /**
     * @brief Send a HTTP request to the server 
     * @param sockfd The connection socket
     * @param message The request
     */
    void send_to_server(const std::string& message) const {
        int bytes, sent = 0;
        int total = message.size();

        do {
            bytes = write(sockfd, message.c_str() + sent, total - sent);
            CERR(bytes < 0);

            if (bytes == 0) {
                break;
            }

            sent += bytes;
        } while (sent < total);
    }

    /**
     * @brief Receive a HTTP response from the server
     * @param sockfd The connection socket
     * @return std::string The response
     */
    std::string receive_from_server() const {
        std::stringstream ss;
        // There is another byte at the end to be sure the buffer is always
        // null-terminated
        char response[BUFLEN + 1];
        int content_length = 0;
        int header_end = 0;

        // Here the header is read
        do {
            bzero(response, BUFLEN + 1);
            int bytes = read(sockfd, response, BUFLEN);
            CERR(bytes < 0);

            if (bytes == 0) {
                break;
            }

            ss << response;

            // Check if we received all the header data
            // Search for the CONTENT-LENGTH
            header_end = ss.str().find(HEADER_TERMINATOR);

            if (header_end >= 0) {
                header_end += sizeof(HEADER_TERMINATOR) - 1;

                int clen_start = ss.str().find("Content-Length: ");

                if (clen_start < 0) {
                    continue;
                }

                clen_start += sizeof("Content-Length: ") - 1;
                int clen_size = ss.str().find(ENDL, clen_start) - clen_start;

                // Extract content lenght (the number) from the field
                content_length =
                    atoi(ss.str().substr(clen_start, clen_size).c_str());
                std::cout << content_length << "\n";
                break;
            }
        }
        FOREVER;

        size_t total = content_length + (size_t) header_end;


        // Receive the DATA contained
        while(ss.str().size() < total) {
            bzero(response, BUFLEN + 1);
            int bytes = read(sockfd, response, BUFLEN);

            CERR(bytes < 0);

            if (bytes == 0) {
                break;
            }

            ss << response;
        }

        // Return the full HTTP Response
        return ss.str();
    }

    /**
     * @brief Connect to the REST server
     */
    void connect_to_server() const {
        MUST(::connect(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr)) >= 0,
             "Couldn't connect");
    }

    /**
     * @brief Disconnect from the REST server
     * 
     */
    void disconnect_from_server() const {
        close(sockfd);
    }

   public:
    /**
     * @brief Initalise the client for communications
     * @param host The host to connect to
     * @param port The port on which the connection will be established
     */
    Client(const std::string& host, const int port) {
        in_addr ip = getIpFromHostname(host, port);

        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        MUST(sockfd >= 0, "Couldn't create socket\n");

        bzero(&serv_addr, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(port);
        serv_addr.sin_addr = ip;

        // Test the connection
        connect_to_server();
        disconnect_from_server();
    }

    ~Client() { disconnect_from_server(); }
};
}  // namespace RestCpp