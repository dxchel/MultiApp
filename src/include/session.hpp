#pragma once

#include <vector>
#include <list>

#include <thread>
#include <asio.hpp>

using asio::ip::tcp;
using asio::awaitable;
using asio::use_awaitable;

constexpr const char *DEFAULT_PORT {"1234"};
constexpr const char *LOCALHOST    {"127.0.0.1"};



/**
 * @brief Connection struct containing important information for a connection.
 * 
 * Contains the socket, timer, nickname, fingerprint and send buffer for a connection.
 * Also contains a function to add messages to the send buffer using a mutex in case needed.
 */
struct Connection {
    inline static unsigned current_id{};

    tcp::socket        socket;
    asio::steady_timer send_timer;
    std::mutex         send_mutex;
    std::string        nickname{};
    std::string        fingerprint{};
    std::string        send_buffer{};

    /* Constructor for Connection, initializes the socket and timer, and sets the default nickname. */
    explicit Connection(asio::io_context&);

    /**
     * @brief Adds a string to the buffer.
     *
     * Adds a string to the buffer, appending the string with a '\n' at the end,
     * canceling the timer for the message to be sent.
     *
     * @param[in] message: Message to be buffered.
     */
    void add_to_send_buffer(std::string&);
};


/**
 * @brief Session class containing basic structure for Client/Server.
 *
 * Uses a vector to store received messages and runs connection in its own thread.
 * Contains a sender and receiver asio functions and has a poster and disconnecter function so it can interact with GUI.
 */
class Session {
public:
    std::vector<std::string> receive_queue;
    std::mutex               receive_mutex;

    /* Default constructor for Session. */
    Session() = default;

    /* Virtual destructor for Session. Stops ioc and joins thread. */
    virtual ~Session() noexcept;

    /* Non copyable, non movable */
    Session(const Session&) = delete;
    Session& operator=(const Session&) = delete;
    Session(Session&&) = delete;
    Session& operator=(Session&&) = delete;

    /**
     * @brief Adds a function for the session to run when a message was processed.
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

    /**
     * @brief Processes a message depending on its type.
     *
     * @param[in] message: Message to be processed.
     * @param[in] origin: Connection that sent the message.
     */
    virtual void process_message(std::string&, std::shared_ptr<Connection> = nullptr) = 0;

protected:
    std::string host;
    unsigned    port;

    asio::io_context          ioc;
    std::thread               ioc_thread;
    std::function<void(void)> poster;
    std::function<void(void)> disconnecter;

    /**
     * @brief Sender function to run so the session sends data.
     *
     * @param[in] connection: Connection struct to send data to.
     */
    awaitable<void> sender(std::shared_ptr<Connection>);

    /**
     * @brief Receiver function to run so the session awaits for data.
     *
     * @param[in] connection: Connection struct to receive data from.
     */
    virtual awaitable<void> receiver(std::shared_ptr<Connection>);

    /**
     * @brief Broadcasting method for Server and Client.
     *
     * @param[in] message: Message to be broadcasted.
     * @param[in] origin: Connection that sent the message.
     */
    virtual void broadcast(const std::string&, std::shared_ptr<Connection> = nullptr) = 0;
};


/**
 * @brief Client class that connects to a server and sends messages.
 *
 * Connects to a server on a given host and port, sends messages to the server,
 * and receives messages from the server.
 */
class Client : public Session {
public:
    /* Constructor for Client, initializes the connection and connects to the server. */
    explicit Client(const std::string &host, unsigned port);

    /**
     * @brief Processes a message for Client.
     * 
     * Client posts the message and sends it to the server in case there's no origin.
     *
     * @param[in] message: Message to be processed.
     * @param[in] origin: Connection that sent the message.
     */
    void process_message(std::string&, std::shared_ptr<Connection> = nullptr) override;

private:
    std::shared_ptr<Connection> connection;

    /**
     * @brief Receiver function to run so the connection awaits for data.
     * 
     * It also removes closed connections.
     *
     * @param[in] connection: Connection struct to receive data from.
     */
    awaitable<void> receiver(std::shared_ptr<Connection>) override;

    /**
     * @brief Broadcasting method for Client.
     * 
     * Broadcasts a message to the server, sending it to the server with the origin connection as nullptr.
     *
     * @param[in] message: Message to be broadcasted.
     * @param[in] origin: nullptr.
     */
    void broadcast(const std::string&, std::shared_ptr<Connection> = nullptr) override;
};


/**
 * @brief Server class that accepts clients and broadcasts messages between them.
 *
 * Listens on a given port, relays every incoming line to all other connected
 * clients, and echoes it back to the sender tagged with "(you)".
 */
class Server : public Session {
public:
    /* Constructor for Server, initializes the acceptor and starts the accept loop. */
    explicit Server(unsigned port);

    /* Destructor for Server. Closes the acceptor. */
    ~Server() noexcept;

    /**
     * @brief Processes a message for the Server.
     * 
     * Always posts and broadcasts the message to all clients.
     *
     * @param[in] message: Message to be processed.
     * @param[in] origin: Connection that sent the message.
     */
    void process_message(std::string&, std::shared_ptr<Connection> = nullptr) override;

private:
    tcp::acceptor    acceptor;

    std::list<std::shared_ptr<Connection>> connections{};

    /**
     * @brief Receiver function to run so the session awaits for data.
     * Removes closed connections.
     * 
     * @param[in] socket: Socket connection to receive data from.
     */
    awaitable<void> receiver(std::shared_ptr<Connection>) override;

    /* Accept loop function to run so the session accepts new clients. */
    awaitable<void> accept_loop();

    /**
     * @brief Broadcasting method for Server.
     * 
     * Broadcasts a message to all clients.
     *
     * @param[in] message: Message to be broadcasted.
     * @param[in] origin: Connection that sent the message.
     */
    void broadcast(const std::string&, std::shared_ptr<Connection> = nullptr) override;
};

