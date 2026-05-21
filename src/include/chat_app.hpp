#ifndef _CHAT_APP_
#define _CHAT_APP_

#include <atomic>
#include <gtkmm.h>

#include <asio.hpp>

#include <memory>
#include <string>

using asio::ip::tcp;
using asio::awaitable;
using asio::use_awaitable;

static constexpr const char *DEFAULT_PORT {"9000"};
static constexpr const char *LOCALHOST {"127.0.0.1"};


class Session
{
    std::unique_ptr<asio::steady_timer> send_timer{};
    std::string send_buf{}, host{};
    std::deque<tcp::socket> sockets{};
    asio::io_context ioc{};
    std::thread ioc_thread{};
    unsigned port{};
    std::function<void(std::string&)> poster{};
    std::function<void(void)> disconnecter{};

public:
    std::atomic<bool> posted{true};
    Session(const std::string&, unsigned);
    ~Session() noexcept;


    awaitable<void> sender(tcp::socket& socket);
    awaitable<void> receiver(tcp::socket& socket);

    void set_poster(std::function<void(std::string&)>);
    void set_disconnecter(std::function<void(void)>);
    void connect();
    void add_to_buffer(std::string);
};


/**
 * @brief Chat class containing important Widgets and functions for Chat functionality.
 *
 * Gtk::Box implementing class that contains important Gtk Widgets
 * for connecting and sending messages to another app.
 */
class Chat : public Gtk::Box
{
    friend class ChatTest;
    friend class ChatTest_ChatFunctionalTest_Test;

    Gtk::Button *home_button{}, *connect_button{}, *message_button{};
    Gtk::Entry *ip_entry{}, *port_entry{}, *message_entry{};
    Gtk::Box *chat_box{}, *footer_box{};

    Gtk::Label *status_label{};

    std::unique_ptr<Session> session{};

    /**
     * @brief Get Main Application status label.
     *
     * Gets Main Application status label after realize due to
     * the box needing to be contained.
     */
    void on_realize() override;

    inline void message_buffer();

    inline void session_connection();

public:
    /**
     * @brief Creates Chat object with all needed Widgets and signals.
     *
     * Creates Chat object using res/gtk/chat_app.ui file,
     * Checks for file issues, gets Widgets and connects needed signals.
     */
    Chat();

    // Call this from anywhere on the Asio thread to safely update GTK
    //void post_to_gtk();

};

#endif