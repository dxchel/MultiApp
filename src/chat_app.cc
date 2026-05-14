#include "include/chat_app.hpp"
#include "gtkmm/enums.h"
#include "gtkmm/object.h"
#include "gtkmm/scrolledwindow.h"
#include "gtkmm/textview.h"

#include <cstddef>
#include <cstdint>
#include <iostream>

#include <gtkmm.h>
#include <string>
#include <webkit/webkit.h>


Socket::Socket() {}

// Moveable
Socket::Socket(Socket&& target) noexcept : socket{target.socket} { target.socket = INVALID_SOCKET; }

Socket::operator socket_t() const { return socket; }
Socket& Socket::operator=(Socket&& target) noexcept
{
    if (this != &target)
    {
        close();
        socket = target.socket;
        target.socket = INVALID_SOCKET;
    }
    return *this;
}

bool Socket::valid() const { return socket != INVALID_SOCKET; }
void Socket::close()
{
    if (socket != INVALID_SOCKET) {
        close_socket(socket);
        std::cout << "Closed socket: " << socket << std::endl;
        socket = INVALID_SOCKET;
    }
}

bool Socket::connect(const std::string &ip, uint16_t port)
{
    std::string host {ip.compare("localhost") ? ip : LOCALHOST};

#ifdef _WIN32
    WinsockInit wsa_init;
#endif

    socket = ::socket(AF_INET, SOCK_STREAM, 0);

    if (socket == INVALID_SOCKET)
    {
        std::cerr << "[error] socket(): " + last_error() << std::endl;
        return false;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port   = htons(port);

    int rc {::inet_pton(AF_INET, host.c_str(), &addr.sin_addr)};
    if (!rc)
    {
        std::cerr << "[error] Invalid address: " + host << std::endl;
        return false;
    }
    if (rc < 0) {
        std::cerr << "[error] inet_pton(): " + last_error() << std::endl;
        return false;
    }

    std::cout << "Connecting to " << host << ":" << port << "..." << std::endl;
    if (::connect(socket, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == SOCKET_ERROR) {
        std::cerr << "[error] connect(): " << last_error() << "\nIs the server running?\n";
        return false;
    }
    return true;
}

bool Socket::send_message(const std::string& message) const
{
    const char *ptr {message.c_str()};
    std::size_t left {message.size()};
    std::cout << "Sending: " << message << " on " << socket << std::endl;
    while (left > 0) {
        ssize_t sent = ::send(socket, ptr, static_cast<int>(left), 0);
        if (sent <= 0)
        {
            std::cerr << std::endl << "[error] send failed: " << last_error() << std::endl;
            std::cerr << "Socket: " << socket << std::endl;
            return false;
        }
        ptr  += sent;
        left -= static_cast<std::size_t>(sent);
    }
    return true;
}


Socket::~Socket() { close(); }


Chat::Chat() : Gtk::Box(Gtk::Orientation::VERTICAL)
{
    // Load the GtkBuilder file and instantiate its widgets, check for errors
    auto ref_builder {Gtk::Builder::create()};
    try
    {
        ref_builder->add_from_file("res/gtk/chat_app.ui");
    }
    catch(const Glib::FileError& ex)
    {
        std::cerr << "FileError: " << ex.what() << std::endl;
        throw ex;
    }
    catch(const Glib::MarkupError& ex)
    {
        std::cerr << "MarkupError: " << ex.what() << std::endl;
        throw ex;
    }
    catch(const Gtk::BuilderError& ex)
    {
        std::cerr << "BuilderError: " << ex.what() << std::endl;
        throw ex;
    }

    // Get the GtkBuilder-instantiated nav and header:
    auto header {Gtk::manage(ref_builder->get_widget<Gtk::Box>("header_box"))};

    // Get the GtkBuilder-instantiated widgets, and connect a signal handler
    home_button = ref_builder->get_widget<Gtk::Button>("home_button");
    connect_button = ref_builder->get_widget<Gtk::Button>("connect_button");
    message_button = ref_builder->get_widget<Gtk::Button>("message_button");

    ip_entry = ref_builder->get_widget<Gtk::Entry>("ip_entry");
    port_entry = ref_builder->get_widget<Gtk::Entry>("port_entry");
    message_entry = ref_builder->get_widget<Gtk::Entry>("message_entry");

    auto chat_scrolled {Gtk::manage(ref_builder->get_widget<Gtk::ScrolledWindow>("chat_scrolled"))};
    chat_box = Gtk::manage(ref_builder->get_widget<Gtk::Box>("chat_box"));
    footer_box = Gtk::manage(ref_builder->get_widget<Gtk::Box>("footer_box"));

    // Add Callbacks
    if(home_button) [[likely]]
        home_button->signal_clicked().connect
        ( [this](){ ip_entry->set_text(LOCALHOST); port_entry->set_text(DEFAULT_PORT); } );
    if(ip_entry) [[likely]]
        ip_entry->signal_activate().connect([this](){ connect(); });
    if(port_entry) [[likely]]
        port_entry->signal_activate().connect([this](){ connect(); });
    if(connect_button) [[likely]]
        connect_button->signal_clicked().connect([this](){ connect(); });

    if(message_entry) [[likely]]
        message_entry->signal_activate().connect([this]()
        {
            if ( !message_entry->get_text_length() ) return;
            broadcast_message(message_entry->get_text() + "\n");
        });
    if(message_button) [[likely]]
        message_button->signal_clicked().connect([this]()
        {
            if ( !message_entry->get_text_length() ) return;
            broadcast_message(message_entry->get_text() + "\n");
        });

    // Insert elements into Browser Box
    insert_child_at_start(*header);
    append(*chat_scrolled);
    append(*footer_box);
}

void Chat::on_realize()
{
    Gtk::Box::on_realize();
    status_label = dynamic_cast<Gtk::Label *>(get_parent()->get_parent()->get_parent()->get_parent()->get_last_child());
    if (!status_label) std::cout << "Status label not found" << std::endl;
    if (status_label)
        status_label->set_text("Welcome to the LAN Chat!");
}

inline void Chat::connect()
{
    if (!connect_button->get_label().compare("Disconnect"))
    {
        close_sockets();

        connect_button->set_label("Connect");
        footer_box->set_visible(false);
        ip_entry->set_sensitive(true);
        port_entry->set_sensitive(true);
        return;
    }

    if ( static_cast<unsigned>(std::atoi(port_entry->get_text().c_str())) > 65535 )
    {
        std::cerr << "Port out of bounds (0-65535)" << std::endl;
        status_label->set_label("Something went wrong when trying to connect, check cerr");
        return;
    }

    sockets.emplace_back();
    if ( !sockets[0].connect(ip_entry->get_text().lowercase(), std::atoi(port_entry->get_text().c_str())))
    {
        status_label->set_label("Something went wrong when trying to connect, check cerr");
        return;
    }

    connect_button->set_label("Disconnect");
    ip_entry->set_sensitive(false);
    port_entry->set_sensitive(false);
    footer_box->set_visible(true);
    message_entry->grab_focus();
    return;
}

inline void Chat::broadcast_message(const std::string &message)
{
    for ( auto &socket : sockets ) socket.send_message(message);
    message_entry->delete_text(0, -1);
}
/*
inline void Chat::receive_line(socket_t sock, std::string &received)
{
    received.clear();
    char ch = 0;
    while (running) {
        ssize_t n = ::recv(sock, &ch, 1, 0);
        if (n <= 0) running = false;
        if (ch == '\n') break;
        if (ch != '\r') received += ch;
    }
    return;
}

void Chat::receiver_thread(socket_t sock)
{
    std::string message{};
    while (running) {
        receive_line(sock, message);
        if ( !message.empty() )
        {
            auto bubble = Gtk::manage(new Gtk::TextView());
            bubble->set_hexpand(true);
            bubble->set_editable(false);
            bubble->set_justification(Gtk::Justification::LEFT);
            bubble->get_buffer()->set_text(message);
            chat_box->append(*bubble);
            std::cout << message;
        }
    }
    ::close(sock);
}
*/
inline void Chat::close_sockets()
{
    running = false;
    sockets.clear();
    std::cout << "Disconnected, bye!" << std::endl;
}

Chat::~Chat() { running = false; for ( auto &socket : sockets ) socket.close(); }

