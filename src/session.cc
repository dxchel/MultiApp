#include "include/session.hpp"

#include <iostream>
#include <vector>
#include <list>

#include <thread>
#include <asio.hpp>



Connection::Connection(asio::io_context& ioc) :
    socket     (ioc),
    send_timer (ioc),
    nickname   ("User" + std::to_string(++current_id))
    { send_timer.expires_at(asio::steady_timer::time_point::min()); }

void Connection::add_to_send_buffer(std::string& message) {
    send_buffer.append(message + "\n");
    send_timer.cancel();
}


Session::~Session() noexcept {
    if ( !ioc.stopped() ) ioc.stop();
    if ( ioc_thread.joinable() ) ioc_thread.join();
    std::cout << "[" + type + "] stopped\n";
}

awaitable<void> Session::sender(std::shared_ptr<Connection> connection) {
    try {
        while (true) {
            while (true) {
                if (!connection->send_buffer.empty()) break;
                asio::error_code ec;
                co_await connection->send_timer.async_wait(asio::redirect_error(use_awaitable, ec));
                if (ec && ec != asio::error::operation_aborted) co_return;
            }
            std::string data;
            std::swap(data, connection->send_buffer);
            connection->send_timer.expires_at(asio::steady_timer::time_point::min());
            co_await asio::async_write(connection->socket, asio::buffer(data), use_awaitable);
        }
    } catch (const std::exception& e) {
        std::cout << "[" + type + "] sender error: " << e.what() << "\n";
    }
}

awaitable<void> Session::receiver(std::shared_ptr<Connection> connection) {
    try {
        asio::streambuf buf;
        while (true) {
            std::size_t n = co_await asio::async_read_until(connection->socket, buf, '\n', use_awaitable);

            std::string line(
                asio::buffers_begin(buf.data()),
                asio::buffers_begin(buf.data()) + static_cast<std::ptrdiff_t>(n)
            );
            buf.consume(n);

            process_message(line, connection);
        }
    } catch (const std::exception& e) {
        std::cout << "[" + type + "] " << connection->nickname << " disconnected: " << e.what() << "\n";
    }
}

void Session::set_poster(std::function<void(void)> message_poster)
    { poster = std::move(message_poster); }

void Session::set_disconnecter(std::function<void(void)> new_disconnecter)
    { disconnecter = std::move(new_disconnecter); }

Client::Client(const std::string &host, unsigned port) :
    Session::Session(),
    connection (std::make_shared<Connection>(ioc)) {
    type = "Client";
    connection->nickname = "Server";
    this->host = host;
    this->port = port;

    if ( port > 65535 ) {
        std::cerr << "Port out of bounds (0-65535)\n";
        throw std::out_of_range("Port out of bounds (0-65535)");
    }
    connection->send_timer.expires_at(asio::steady_timer::time_point::min());
    tcp::resolver resolver(ioc);

    std::cout << "[Client] Connecting to " << host << ":" << port << "...\n";

    asio::error_code ec;
    auto endpoints = resolver.resolve(host, std::to_string(port), ec);
    if (ec) {
        std::cerr << "[Client] resolve(): " << ec.message()
                  << "\nIs the server running?\n";
        throw std::runtime_error("Failed to resolve host");
    }

    asio::connect(connection->socket, endpoints, ec);
    if (ec) {
        std::cerr << "[Client] connect(): " << ec.message()
                  << "\nIs the server running?\n";
        throw std::runtime_error("Failed to connect to server");
    }

    // Spawn receiver and sender
    asio::co_spawn(ioc, receiver(connection), asio::detached);
    asio::co_spawn(ioc, sender(connection), asio::detached);

    // Run the event loop in separate thread until ioc.stop() is called
    ioc_thread = std::thread([this]{ ioc.run(); });
    std::cout << "[Client] connected to " << host << ":" << port << "\n";
}


awaitable<void> Client::receiver(std::shared_ptr<Connection> connection) {
    co_await Session::receiver(connection);
    if ( connection->socket.is_open() )
        connection->socket.close();
    if (disconnecter) disconnecter();
}

void Client::broadcast(const std::string& message, std::shared_ptr<Connection> origin) {
    (void) origin;
    connection->send_buffer.append(message + "\n");
    connection->send_timer.cancel();
}

void Client::process_message(std::string &message, std::shared_ptr<Connection> origin) {
    while (!message.empty() && (message[message.size()-1] == '\n' || message[message.size()-1] == '\r'))
        message.erase(message.size()-1);

    if ( origin ) {
        std::lock_guard<std::mutex> lock(receive_mutex);
        receive_queue.push_back(message);
        if (poster) poster();
    }
    else {
        broadcast(message, origin);
    }
}


Server::Server(unsigned port) : Session::Session(),
    acceptor (tcp::acceptor(ioc, tcp::endpoint(tcp::v4(), static_cast<asio::ip::port_type>(port)))) {
    type = "Server";
    asio::co_spawn(ioc, accept_loop(), asio::detached);
    ioc_thread = std::thread([this]{ ioc.run(); });
    std::cout << "[" + type + "] listening on port " << port << "\n";
}

Server::~Server() noexcept {
    acceptor.close();
}

awaitable<void> Server::receiver(std::shared_ptr<Connection> connection) {
    co_await Session::receiver(connection);
    if ( connection->socket.is_open() )
        connection->socket.close();
    connections.remove(connection);
    std::string farewell{ connection->nickname + " has left the chat!!!" };
    process_message(farewell);
}

awaitable<void> Server::accept_loop() {
    while (true) {
        auto connection = std::make_shared<Connection>(ioc);
        asio::error_code ec;
        co_await acceptor.async_accept(connection->socket, asio::redirect_error(use_awaitable, ec));
        if (ec) {
            std::cout << "[" + type + "] accept loop ending: " << ec.message() << "\n";
            co_return;
        }

        try {
            auto ep = connection->socket.remote_endpoint();
            connection->fingerprint = ep.address().to_string() + ":" + std::to_string(ep.port());
        } catch (...) { connection->fingerprint = "unknown"; }
        std::cout << "[" + type + "] new client: " << connection->nickname << " (" << connection->fingerprint << ")\n";

        std::string message {connection->nickname + " says Hi!!!\n"};
        process_message(message);

        asio::co_spawn(ioc, receiver(connection), asio::detached);
        asio::co_spawn(ioc, sender(connection),   asio::detached);

        connections.push_back(connection);
    }
    if (disconnecter) disconnecter();
}

void Server::broadcast(const std::string& message, std::shared_ptr<Connection> origin) {
    for (auto& connection : connections) {
        std::lock_guard<std::mutex> send_lock(connection->send_mutex);
        if ( !origin )
            connection->send_buffer += message + "\n";
        else if ( connection->fingerprint == origin->fingerprint )
            connection->send_buffer += "(You): " + message + "\n";
        else
            connection->send_buffer += "(" + origin->nickname + "): " + message + "\n";
        connection->send_timer.cancel();
    }
}

void Server::process_message(std::string &message, std::shared_ptr<Connection> connection) {
    (void) connection;
    while (!message.empty() && (message[message.size()-1] == '\n' || message[message.size()-1] == '\r'))
        message.erase(message.size()-1);

    broadcast(message, connection);

    if ( connection )
        message = "(" + connection->nickname + "): " + message;

    std::lock_guard<std::mutex> lock(receive_mutex);
    receive_queue.push_back(message);
    if (poster) poster();
}
