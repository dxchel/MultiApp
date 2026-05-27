#ifndef _CHAT_APP_
#define _CHAT_APP_

#include <atomic>
#include <gtkmm.h>

#include <asio.hpp>

#include <string>

using asio::ip::tcp;
using asio::awaitable;
using asio::use_awaitable;

static constexpr const char *DEFAULT_PORT {"1234"};
static constexpr const char *LOCALHOST {"127.0.0.1"};


/**
 * @brief Session class containing everything needed to open an async client session.
 *
 * Uses a string buffer for sending, appends received messages
 * to a deque and runs asio in a different thread for the app to run other things.
 * 
 * It has a poster and disconnecter function so it can interact with GUI.
 */
class Session {
    std::string send_buf{}, host{};
    unsigned port{};

    std::deque<tcp::socket> sockets{};

    asio::io_context ioc{};
    std::unique_ptr<asio::steady_timer> send_timer{};
    std::thread ioc_thread{};

    std::function<void(void)> poster{};
    std::function<void(void)> disconnecter{};

public:
    std::deque<std::string> message_queue;
    std::mutex queue_mutex;

    /**
     * @brief Connects the session as client and starts the asio thread.
     * 
     * @throws out_of_range if the port is out of range.
     */
    Session(const std::string&, unsigned);

    /* Stops asio io context, joins asio thread and deletes sockets. */
    ~Session() noexcept;


    /**
     * @brief Receiver function to run so the session sends data.
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
     * @brief Connects the session as client and starts the asio thread.
     *
     * @throws RuntimeError on connection failure.
     */
    void connect();

    /**
     * @brief Adds a string to the buffer.
     *
     * Adds a string to the buffer, appending the string with a '\n' at the end,
     * canceling the timer for the message to be sent.
     *
     * @param[in] message: Message to be buffered.
     */
    void add_to_buffer(std::string);
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

    Gtk::Button *home_button{}, *connect_button{}, *message_button{};
    Gtk::Entry *ip_entry{}, *port_entry{}, *message_entry{};
    Gtk::Box *chat_box{}, *footer_box{};
    Gtk::ScrolledWindow *chat_scrolled{};
    Gtk::Label *status_label{};

    Glib::Dispatcher dispatcher{};

    std::unique_ptr<Session> session{};

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
     * @brief Tries connecting as client.
     *
     * Checks the button to see if it needs to connect or disconnect, then tries
     * to connect as client and adds any needed information to the Session object.
     */
    inline void session_connection();

    /**
     * @brief Poster function to run when the session receives data.
     * 
     * This function uses the session data queue and adds the message bubbles to the box
     */
    inline void poster();

public:
    /**
     * @brief Creates Chat object with all needed Widgets and signals.
     *
     * Creates Chat object using res/gtk/chat_app.ui file,
     * Checks for file issues, gets Widgets and connects needed signals.
     */
    Chat();
};

#endif
