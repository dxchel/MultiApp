#ifndef _CHAT_APP_
#define _CHAT_APP_

#include <atomic>
#include <gtkmm.h>

#include <asio.hpp>
#include <asio/co_spawn.hpp>
#include <asio/detached.hpp>
#include <asio/use_awaitable.hpp>

#include <memory>
#include <string>

using asio::ip::tcp;
using asio::awaitable;
using asio::use_awaitable;

static constexpr const char *DEFAULT_PORT {"9000"};
static constexpr const char *LOCALHOST {"127.0.0.1"};


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

    std::unique_ptr<asio::steady_timer> send_timer{};
    std::string send_buf{};
    std::deque<tcp::socket> sockets{};
    asio::io_context ioc;
    std::thread ioc_thread;
    std::atomic<bool> posted{true};

    /**
     * @brief Get Main Application status label.
     *
     * Gets Main Application status label after realize due to
     * the box needing to be contained.
     */
    void on_realize() override;

    inline void conection_check();
    inline void message_buffer();
    awaitable<void> sender(tcp::socket& socket);
    awaitable<void> receiver(tcp::socket& socket);
    // Call this from anywhere on the Asio thread to safely update GTK
    void post_to_gtk(std::function<void()>);


public:
    /**
     * @brief Creates Chat object with all needed Widgets and signals.
     *
     * Creates Chat object using res/gtk/chat_app.ui file,
     * Checks for file issues, gets Widgets and connects needed signals.
     */
    Chat();

    ~Chat() noexcept;
};

#endif