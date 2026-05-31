#pragma once

#include <atomic>
#include <gtkmm.h>

#include <asio.hpp>

#include <string>
#include <regex>

using asio::ip::tcp;
using asio::awaitable;
using asio::use_awaitable;

constexpr const char *DEFAULT_PORT {"1234"};
constexpr const char *LOCALHOST    {"127.0.0.1"};


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
    std::deque<std::string> receiver_queue;
    std::mutex              receiver_mutex;
    std::string             sender_buf;
    std::mutex              sender_mutex;

    /* Deleted default constructor and destructor as it's not supposed to be used without Client or Server. */
    Session();
    ~Session() noexcept;


    /**
     * @brief Sender function to run so the session sends data.
     *
     * @param[in] socket: Socket connection to send data to.
     */
    awaitable<void> sender(tcp::socket& socket);

    /**
     * @brief Receiver function to run so the session awaits for data.
     *
     * @param[in] socket: Socket connection to receive data from.
     */
    awaitable<void> receiver(tcp::socket& socket);

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

    /**
     * @brief Adds a string to the buffer.
     *
     * Adds a string to the buffer, appending the string with a '\n' at the end,
     * canceling the timer for the message to be sent.
     *
     * @param[in] message: Message to be buffered.
     */
    void add_to_send_buffer(std::string);

    virtual void process_message(std::string);

protected:
    std::string host;
    unsigned    port;

    asio::io_context                    ioc;
    std::unique_ptr<asio::steady_timer> send_timer;
    std::thread                         ioc_thread;

    std::function<void(void)> poster;
    std::function<void(void)> disconnecter;

};


class Client : public Session {
    tcp::socket socket;

    /* Queue line for the sender, then wake their sender. */
    void send_message(const std::string&);

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
    /* Client struct containing socket, timer, buffer and nickname for each client. */
    struct Client {
        inline static unsigned current_id{};

        tcp::socket        socket;
        asio::steady_timer timer;
        std::string        nickname{};
        std::string        fingerprint{};
        std::string        buf{};

        explicit Client(asio::io_context&);
    };

    tcp::acceptor    acceptor;

    std::list<std::shared_ptr<Client>> clients{};

    /* Queue line for every client except origin, then wake their sender. */
    void broadcast(const std::string&, Client* = nullptr);

    /* Accept loop function to run so the session accepts new clients. */
    awaitable<void> accept_loop();

public:
    explicit Server(unsigned port);
    ~Server() noexcept;
};


/**
 * @brief Chat class containing important Widgets and functions for Chat functionality.
 *
 * Gtk::Box implementing class that contains important Gtk Widgets
 * for connecting and sending messages to another app.
 */
class Chat : public Gtk::Box {
    friend class ChatTest;
    friend class ChatTest_ChatFunctionalTest_Test;

    Gtk::Button         *home_button{};
    Gtk::Button         *connect_button{};
    Gtk::Button         *message_button{};
    Gtk::Entry          *ip_entry{};
    Gtk::Entry          *port_entry{};
    Gtk::Entry          *message_entry{};
    Gtk::Box            *chat_box{};
    Gtk::Box            *footer_box{};
    Gtk::ScrolledWindow *chat_scrolled{};
    Gtk::Label          *status_label{};

    std::unique_ptr<Glib::Dispatcher> dispatcher{};

    std::unique_ptr<Session> session{};
    std::unique_ptr<Server>  server{};  // non-null when we are the host

    /**
     * @brief Get Main Application status label.
     *
     * Gets Main Application status label after realize due to
     * the box needing to be contained.
     */
    void on_realize() override;

    /**
     * @brief Buffers the message from the Entry to the Session Buffer for sending.
     *
     * Does nothing if Entry is empty, else it adds the message and empties the Entry.
     */
    inline void message_buffer();

    /**
     * @brief Tries connecting as client, falls back to hosting if localhost and no server found.
     *
     * Checks the button to see if it needs to connect or disconnect, then tries
     * to connect as client and adds any needed information to the Session object.
     * If the host is localhost and the connection fails, starts a Server and
     * connects to it.
     */
    inline void session_connection();

public:
    /**
     * @brief Creates Chat object with all needed Widgets and signals.
     *
     * Creates Chat object using res/gtk/chat_app.ui file,
     * Checks for file issues, gets Widgets and connects needed signals.
     */
    Chat();
};
