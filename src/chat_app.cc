#include "include/chat_app.hpp"
#include "gtkmm/scrolledwindow.h"

#include <iostream>
#include <regex>

#include <gtkmm.h>
#include <webkit/webkit.h>


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

    if(message_button) [[likely]]
        message_button->signal_clicked().connect([this](){ send_message(); });
    if(message_entry) [[likely]]
        message_entry->signal_activate().connect([this](){ send_message(); });

    // Insert elements into Browser Box
    insert_child_at_start(*header);
    append(*chat_scrolled);
    append(*footer_box);
}

Chat::~Chat() { if (socket != INVALID_SOCKET) close_socket(); };


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
        if ( socket != INVALID_SOCKET ) close_socket();
        connect_button->set_label("Connect");
        footer_box->set_visible(false);
        ip_entry->set_sensitive(true);
        port_entry->set_sensitive(true);
        return;
    }

    int p {atoi(port_entry->get_text().c_str())};
    if (p > 65535)
    {
        std::cerr << "[error] Invalid port: " << p << "\n";
        return;
    }

    auto port {static_cast<uint16_t>(p)};
    std::string host {ip_entry->get_text().lowercase().compare("localhost") ? ip_entry->get_text() : LOCALHOST};

#ifdef _WIN32
    WinsockInit wsa_init;
#endif

    socket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (socket == INVALID_SOCKET)
    {
        std::cerr << "[error] socket(): " << last_error() << "\n";
        return;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port   = htons(port);

    int rc {::inet_pton(AF_INET, host.c_str(), &addr.sin_addr)};
    if (!rc)
    {
        std::cerr << "[error] Invalid address: " << host << "\n";
        close_socket();
        return;
    }
    if (rc < 0) {
        std::cerr << "[error] inet_pton(): " << last_error() << "\n";
        close_socket();
        return;
    }

    std::cout << "Connecting to " << host << ":" << port << "...\n";
    if (::connect(socket, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == SOCKET_ERROR) {
        std::cerr << "[error] connect(): " << last_error() << "\nIs the server running?\n";
        close_socket();
        return;
    }
    std::cout << "Connected! Type a message and press Enter or the enter button.\n\n";
    connect_button->set_label("Disconnect");
    ip_entry->set_sensitive(false);
    port_entry->set_sensitive(false);
    footer_box->set_visible(true);
    message_entry->grab_focus();
    return;
}


inline void Chat::send_message()
{
    if ( !message_entry->get_text_length() ) return;

    const std::string message { message_entry->get_text() + "\n" };
    const char *ptr {message.c_str()};
    std::size_t left {message.size()};
    std::cout << "Sending: " << message;
    while (left > 0) {
        ssize_t sent = ::send(socket, ptr, static_cast<int>(left), 0);
        if (sent <= 0) { std::cerr << "\n[error] send failed: " << last_error() << "\n"; close_socket(); return;}
        ptr  += sent;
        left -= static_cast<std::size_t>(sent);
    }
    message_entry->delete_text(0, -1);
}

inline std::string Chat::receive_line() const {
    std::string received{};
    char ch = 0;
    while (true) {
        ssize_t n = ::recv(socket, &ch, 1, 0);
        if (n <= 0) { received.clear(); break; }
        if (ch == '\n') break;
        if (ch != '\r') received += ch;
    }
    return received;
}

/*
static void receiver_thread(socket_t sock) {
    std::string line;
    while (g_running) {
        if (!recv_line(sock, line)) {
            if (g_running) {
                std::cout << "\n[disconnected from server]\n";
                g_running = false;
            }
        }
        // Move cursor to beginning of line, print server message, then reprint
        // the prompt so the user's half-typed text stays visible.
        std::cout << "\r" << line << "\n> " << std::flush;
    }
}
*/

#ifdef _WIN32
    inline void Chat::close_socket() { closesocket(socket); socket = INVALID_SOCKET; std::cout << "Disconnected, bye!" << std::endl; }
#else
    inline void Chat::close_socket() { ::close(socket); socket = INVALID_SOCKET; std::cout << "Disconnected, bye!" << std::endl; }
#endif

