#ifndef _CHAT_APP_
#define _CHAT_APP_

#include "gtkmm/textview.h"
#include <cstdint>
#include <gtkmm.h>

#include <iostream>
#include <string>
#include <map>
#include <cstring>
#include <stdexcept>
#include <csignal>
#include <cerrno>
#include <thread>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    using ssize_t = int;
    using socklen = int;
    using socket_t = SOCKET;
    inline void close_socket(SOCKET s) { closesocket(s); }
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
    inline void close_socket(int s) { ::close(s); }
    inline std::string last_error() { return std::strerror(errno); }
#endif

static constexpr const char *DEFAULT_PORT {"9000"};
static constexpr const char *LOCALHOST {"127.0.0.1"};


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


class Socket
{
    socket_t socket{INVALID_SOCKET};

    // Non-copyable
    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;

    // Moveable
    Socket(Socket&&) noexcept;

public:
    Socket();

    operator socket_t() const;
    Socket& operator=(Socket&&) noexcept;

    bool valid() const;
    bool connect(const std::string&, uint16_t);
    bool send_message(const std::string&) const;
    void close();

    ~Socket();
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

    std::atomic<bool> running{true};
    std::deque<Socket> sockets{};

    /**
     * @brief Get Main Application status label.
     *
     * Gets Main Application status label after realize due to
     * the box needing to be contained.
     */
    void on_realize() override;

    inline void connect();
    inline void receive_line(socket_t, std::string&);
    inline void broadcast_message(const std::string&);
    void receiver_thread(socket_t);

#ifdef _WIN32
    inline void close_sockets();
#else
    inline void close_sockets();

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
};

#endif