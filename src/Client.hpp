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
#include "Response.hpp"
#include "Utils.hpp"

namespace RestCpp {
class Client {
   private:
    int sockfd;
    int port;
    sockaddr_in serv_addr;
    std::string host;

    // The session id cookie
    Cookie session_id;
    std::string library_token;

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
                break;
            }
        }
        FOREVER;

        size_t total = content_length + (size_t)header_end;

        // Receive the DATA contained
        while (ss.str().size() < total) {
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
    void connect_to_server() {
        in_addr ip = getIpFromHostname(host, port);

        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        MUST(sockfd >= 0, "Couldn't create socket\n");

        bzero(&serv_addr, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(port);
        serv_addr.sin_addr = ip;
        MUST(::connect(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr)) >= 0,
             "Couldn't connect");
    }

    /**
     * @brief Disconnect from the REST server
     *
     */
    void disconnect_from_server() const { close(sockfd); }

    /**
     * @brief Read a number from STDIN and validate it. It should be a positive
     * number
     * @param prompt The message to show before reading it
     * @return int The number
     */
    uint read_number(std::string prompt) {
        std::string idS;
        do {
            std::cout << prompt;
            std::cin >> idS;

            if (!is_uint(idS)) {
                std::cerr << "Invalid value!\n";
            } else {
                int id = std::stoi(idS);
                return (uint)id;
            }
        }
        FOREVER;
    }

    /**
     * @brief Register a new account using a POST request
     * @param user The username
     * @param pass The password
     */
    void registration(const std::string& user, const std::string& pass) {
        connect_to_server();
        std::vector<KeyValue> body_data;
        body_data.push_back(KeyValue("username", user));
        body_data.push_back(KeyValue("password", pass));

        std::string request = create_post_request(
            host, "/api/v1/tema/auth/register", "application/json", body_data);

        send_to_server(request);
        std::string response = receive_from_server();
        disconnect_from_server();

        Response r(response);

        if (is_code_success(r.get_response_code())) {
            std::cout << "Registration succeded!\n";
        } else {
            // TODO - show the recv data
            std::cout << "Registration failed (" << r.get_response_code()
                      << ")\n";
        }
    }

    /**
     * @brief Login into an account. If the operation is successfull, the
     * session id cookie will be set
     * @param user The username
     * @param pass The password
     */
    void login(const std::string& user, const std::string& pass) {
        connect_to_server();
        std::vector<KeyValue> body_data;
        body_data.push_back(KeyValue("username", user));
        body_data.push_back(KeyValue("password", pass));

        std::string request = create_post_request(
            host, "/api/v1/tema/auth/login", "application/json", body_data);

        send_to_server(request);
        std::string response = receive_from_server();
        disconnect_from_server();

        Response r(response);
        session_id = r.get_session_id();

        if (is_code_success(r.get_response_code())) {
            std::cout << "Login succeded!\n";
        } else {
            // TODO - show the recv data
            std::cout << "Login failed (" << r.get_response_code() << ")\n";
        }
    }

    void enter_library() {
        // Check if this application has received a session id (user has logged
        // in succesfully)
        if (session_id.is_null()) {
            std::cerr << "Login into the account first!\n";
            return;
        }

        connect_to_server();
        std::vector<Cookie> cookies;
        cookies.push_back(session_id);

        std::string request = create_get_request(
            host, "/api/v1/tema/library/access", "", cookies);

        send_to_server(request);
        std::string response = receive_from_server();
        disconnect_from_server();

        Response r(response);
        library_token = r.get_json_data()["token"];
        if (is_code_success(r.get_response_code())) {
            std::cout << "Authorized!\n";
        } else {
            // TODO - show the recv data
            std::cout << "Couldn't enter the library (" << r.get_response_code()
                      << ")\n";
        }
    }

    void get_books() {
        // Check if this application has received a session id (user has logged
        // in succesfully)
        if (session_id.is_null()) {
            std::cerr << "Login into the account first!\n";
            return;
        }

        if (library_token == "") {
            std::cerr << "Enter the library first\n";
            return;
        }

        connect_to_server();
        std::vector<Cookie> cookies;
        cookies.push_back(session_id);

        std::string request = create_get_request(
            host, "/api/v1/tema/library/books", "", cookies, library_token);

        send_to_server(request);
        std::string response = receive_from_server();
        disconnect_from_server();

        Response r(response);
        if (is_code_success(r.get_response_code())) {
            std::cout << "Received the books!\n";
            std::cout << r.get_json_data().dump() << "\n";
        } else {
            // TODO - show the recv data
            std::cout << "The books weren't received (" << r.get_response_code()
                      << ")\n";
        }
    }

    void get_book(const uint id) {
        // Check if this application has received a session id (user has logged
        // in succesfully)
        if (session_id.is_null()) {
            std::cerr << "Login into the account first!\n";
            return;
        }

        if (library_token == "") {
            std::cerr << "Enter the library first\n";
            return;
        }

        connect_to_server();
        std::vector<Cookie> cookies;
        cookies.push_back(session_id);

        std::string url = "/api/v1/tema/library/books/";
        url.append(std::to_string(id));
        std::string request =
            create_get_request(host, url, "", cookies, library_token);

        send_to_server(request);
        std::string response = receive_from_server();
        disconnect_from_server();

        Response r(response);
        if (is_code_success(r.get_response_code())) {
            std::cout << "Received the book!\n";
            std::cout << r.get_json_data().dump() << "\n";
        } else {
            // TODO - show the recv data
            std::cout << "The book wasn't received (" << r.get_response_code()
                      << ")\n";
            std::cout << r.get_json_data().dump() << "\n";
        }
    }

    void add_book(const std::string& title, const std::string& author,
                  const std::string& genre, const std::string& publisher,
                  const uint page_count) {
        // Check if this application has received a session id (user has logged
        // in succesfully)
        if (session_id.is_null()) {
            std::cerr << "Login into the account first!\n";
            return;
        }

        if (library_token == "") {
            std::cerr << "Enter the library first\n";
            return;
        }

        connect_to_server();
        std::vector<Cookie> cookies;
        cookies.push_back(session_id);

        connect_to_server();
        std::vector<KeyValue> body_data;
        body_data.push_back(KeyValue("title", title));
        body_data.push_back(KeyValue("author", author));
        body_data.push_back(KeyValue("genre", genre));
        body_data.push_back(KeyValue("page_count", std::to_string(page_count)));
        body_data.push_back(KeyValue("publisher", publisher));

        std::string request =
            create_post_request(host, "/api/v1/tema/library/books",
                                "application/json", body_data, cookies, library_token);

        send_to_server(request);
        std::string response = receive_from_server();
        disconnect_from_server();

        Response r(response);
        if (is_code_success(r.get_response_code())) {
            std::cout << "Added book to the library!\n";
        } else {
            // TODO - show the recv data
            std::cout << "Couldn't add the book (" << r.get_response_code()
                      << ")\n";
            std::cout << r.get_json_data().dump() << "\n";
        }
    }

    void delete_book(const uint id) {
        // Check if this application has received a session id (user has logged
        // in succesfully)
        if (session_id.is_null()) {
            std::cerr << "Login into the account first!\n";
            return;
        }

        if (library_token == "") {
            std::cerr << "Enter the library first\n";
            return;
        }

        connect_to_server();
        std::vector<Cookie> cookies;
        cookies.push_back(session_id);

        std::string url = "/api/v1/tema/library/books/";
        url.append(std::to_string(id));
        std::string request =
            create_delete_request(host, url, cookies, library_token);

        send_to_server(request);
        std::string response = receive_from_server();
        disconnect_from_server();

        Response r(response);
        if (is_code_success(r.get_response_code())) {
            std::cout << "Removed the book from the library!\n";
        } else {
            // TODO - show the recv data
            std::cout << "Couldn't remove the book (" << r.get_response_code()
                      << ")\n";
        }
    }

    void logout() {
        // Check if this application has received a session id (user has logged
        // in succesfully)
        if (session_id.is_null()) {
            std::cerr << "Login into the account first!\n";
            return;
        }

        connect_to_server();
        std::vector<Cookie> cookies;
        cookies.push_back(session_id);

        std::string request =
            create_get_request(host, "/api/v1/tema/auth/logout", "", cookies);

        send_to_server(request);
        std::string response = receive_from_server();
        disconnect_from_server();

        Response r(response);
        if (is_code_success(r.get_response_code())) {
            std::cout << "You logged out!\n";

            // Delete the cookie
            session_id.set_key("");
            session_id.set_value("");
        } else {
            // TODO - show the recv data
            std::cout << "Couldn't log out (" << r.get_response_code() << ")\n";
        }
    }

   public:
    /**
     * @brief Initalise the client for communications
     * @param host The host to connect to
     * @param port The port on which the connection will be established
     */
    Client(const std::string& host, const int port) : port(port), host(host) {
        connect_to_server();
        disconnect_from_server();
    }

    void run() {
        do {
            std::string command;
            std::cin >> command;

            // Make the input lowercase
            std::transform(command.begin(), command.end(), command.begin(),
                           [](unsigned char c) { return std::tolower(c); });

            if (command == "register") {
                std::string user;
                std::cout << "Username: ";
                std::cin >> user;

                std::string pass, confirm;
                if (HIDE_PASS) {
                    pass = std::string(getpass("Password: "));
                    confirm = std::string(getpass("Confirm password: "));

                    if (pass != confirm) {
                        std::cerr << "Passwords are not the same!\n";
                        std::cout << "\n";
                        continue;
                    }
                } else {
                    std::cout << "Password: ";
                    std::cin >> pass;
                }

                registration(user, pass);
            } else if (command == "login") {
                std::string user;
                std::cout << "Username: ";
                std::cin >> user;

                std::string pass;
                if (HIDE_PASS) {
                    pass = std::string(getpass("Password: "));
                } else {
                    std::cout << "Password: ";
                    std::cin >> pass;
                }

                login(user, pass);
            } else if (command == "enter_library") {
                enter_library();
            } else if (command == "get_books") {
                get_books();
            } else if (command == "get_book") {
                uint id = read_number("Book id: ");
                get_book(id);
            } else if (command == "add_book") {
                std::string title, author, genre, publisher;
                uint page_count;

                std::cout << "Title: ";
                std::cin >> title;
                std::cout << "Author: ";
                std::cin >> author;
                std::cout << "Genre: ";
                std::cin >> genre;
                std::cout << "Publisher: ";
                std::cin >> publisher;
                page_count = read_number("Number of pages: ");

                add_book(title, author, genre, publisher, page_count);
            } else if (command == "delete_book") {
                uint id = read_number("Book id: ");
                delete_book(id);
            } else if (command == "logout") {
                logout();
            } else if (command == "exit") {
                return;
            } else {
                std::cout << "Invalid input!\n";
            }
            std::cout << "\n";
        }
        FOREVER;
    }

    ~Client() { disconnect_from_server(); }
};
}  // namespace RestCpp