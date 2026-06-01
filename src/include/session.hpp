#pragma once

#include <iostream>
#include <thread>
#include <deque>
#include <list>

#include <asio.hpp>

using asio::ip::tcp;
using asio::awaitable;
using asio::use_awaitable;

constexpr const char *DEFAULT_PORT {"1234"};
constexpr const char *LOCALHOST    {"127.0.0.1"};



/* Connection struct containing socket, timer, buffer and nickname for each client. */
struct Connection {
    inline static unsigned current_id{};

    tcp::socket        socket;
    asio::steady_timer send_timer;
    std::mutex         send_mutex;
    std::string        nickname{};
    std::string        fingerprint{};
    std::string        send_buffer{};

    explicit Connection(asio::io_context&);

    /**
     * @brief Adds a string to the buffer.
     *
     * Adds a string to the buffer, appending the string with a '\n' at the end,
     * canceling the timer for the message to be sent.
     *
     * @param[in] message: Message to be buffered.
     */
    void add_to_send_buffer(std::string);
};


/**
 * @brief Session class containing everything needed to open an async client session.
 *
 * Uses a string buffer for sending, appends received messages
 * to a deque and runs asio in a different thread for the app to run other things.
 * 
 * It has a poster and disconnecter function so it can interact with GUI.
 */
class Session {
public:
    std::deque<std::string> receive_queue;
    std::mutex              receive_mutex;

    /* Deleted default constructor and destructor as it's not supposed to be used without Client or Server. */
    Session() = default;
    virtual ~Session() noexcept;


    /**
     * @brief Sender function to run so the session sends data.
     *
     * @param[in] socket: Socket connection to send data to.
     */
    virtual awaitable<void> sender(std::shared_ptr<Connection>);

    /**
     * @brief Receiver function to run so the session awaits for data.
     *
     * @param[in] socket: Socket connection to receive data from.
     */
    virtual awaitable<void> receiver(std::shared_ptr<Connection>);

    /**
     * @brief Adds a function for the session to run on received message.
     *
     * @param[in] function: Function to run on received message.
     */
    void set_poster(std::function<void(void)>);

    /**
     * @brief Adds a function for the session to run on error disconnection.
     *
     * @param[in] function: Function to run on error disconnection.
     */
    void set_disconnecter(std::function<void(void)>);

    /* Broadcasting method for Server and Client. */
    virtual void broadcast(const std::string&, std::shared_ptr<Connection> = nullptr) = 0;

    virtual void process_message(std::string&, std::shared_ptr<Connection> = nullptr) = 0;

protected:
    std::string host;
    unsigned    port;

    asio::io_context          ioc;
    std::thread               ioc_thread;
    std::function<void(void)> poster;
    std::function<void(void)> disconnecter;

};


class Client : public Session {
    std::shared_ptr<Connection> connection;

    /**
     * @brief Receiver function to run so the session awaits for data.
     *
     * @param[in] socket: Socket connection to receive data from.
     */
    awaitable<void> receiver(std::shared_ptr<Connection>) override;

    /**
     * @brief Broadcasting method for Client, send message to Server.
     *
     * @param[in] message: Message to be broadcast.
     * @param[in] origin: Not used as it's a Client transaction.
     */
    void broadcast(const std::string&, std::shared_ptr<Connection> = nullptr) override;

    virtual void process_message(std::string&, std::shared_ptr<Connection> = nullptr) override;

public:
    explicit Client(const std::string &host, unsigned port);
    ~Client() noexcept;
};


/**
 * @brief Server class that accepts clients and broadcasts messages between them.
 *
 * Listens on a given port, relays every incoming line to all other connected
 * clients, and echoes it back to the sender tagged with "(you)".
 * Runs its own ioc on a dedicated thread — same pattern as Session.
 */
class Server : public Session {
    tcp::acceptor    acceptor;

    std::list<std::shared_ptr<Connection>> connections{};

    /**
     * @brief Receiver function to run so the session awaits for data.
     *
     * @param[in] socket: Socket connection to receive data from.
     */
    awaitable<void> receiver(std::shared_ptr<Connection>) override;

    /* Accept loop function to run so the session accepts new clients. */
    awaitable<void> accept_loop();

    /**
     * @brief Broadcasting method for Server, send to all connected clients accordingly.
     *
     * @param[in] message: Message to be broadcast.
     * @param[in] origin: Connection that sent the message.
     */
    void broadcast(const std::string&, std::shared_ptr<Connection> = nullptr) override;
    void process_message(std::string&, std::shared_ptr<Connection> = nullptr) override;

public:
    explicit Server(unsigned port);
    ~Server() noexcept;
};

