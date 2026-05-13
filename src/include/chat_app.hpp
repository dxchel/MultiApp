#ifndef _CHAT_APP_
#define _CHAT_APP_

#include <gtkmm.h>

#include <iostream>
#include <string>
#include <cstring>
#include <stdexcept>
#include <csignal>
#include <cerrno>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    using ssize_t = int;
    using socklen = int;
    using socket_t = SOCKET;
    inline std::string last_error() { return std::to_string(WSAGetLastError()); }
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #define INVALID_SOCKET (-1)
    #define SOCKET_ERROR (-1)
    using socklen = socklen_t;
    using socket_t = int;
    inline std::string last_error() { return std::strerror(errno); }
#endif

static constexpr const char *DEFAULT_PORT {"9000"};
static constexpr const int BACKLOG {10};
static constexpr const char *LOCALHOST {"127.0.0.1"};

static std::atomic<bool> g_running{true};


#ifdef _WIN32
struct WinsockInit {
    WinsockInit() {
        WSADATA wsa{};
        if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
            throw std::runtime_error("WSAStartup failed: " + last_error());
    }
    ~WinsockInit() { WSACleanup(); }
};
#endif


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

    socket_t socket{};

    /**
     * @brief Get Main Application status label.
     *
     * Gets Main Application status label after realize due to
     * the box needing to be contained.
     */
    void on_realize() override;

    inline void connect();
    inline std::string receive_line() const;

#ifdef _WIN32
    inline void close_socket();
#else
    inline void close_socket();

    ~Chat();
#endif

public:
    /**
     * @brief Creates Chat object with all needed Widgets and signals.
     *
     * Creates Chat object using res/gtk/chat_app.ui file,
     * Checks for file issues, gets Widgets and connects needed signals.
     */
    Chat();

    inline void send_message();
};


/// Send an entire buffer, retrying on partial sends.
inline bool send_all(socket_t fd, const std::string& msg) {
    const char*  ptr  = msg.c_str();
    std::size_t  left = msg.size();
    while (left > 0) {
        ssize_t sent = ::send(fd, ptr, static_cast<int>(left), 0);
        if (sent <= 0) return false;
        ptr  += sent;
        left -= static_cast<std::size_t>(sent);
    }
    return true;
}

#endif