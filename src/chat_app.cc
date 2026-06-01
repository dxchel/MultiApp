#include "include/chat_app.hpp"

#include <algorithm>
#include <asio.hpp>
#include <iostream>

#include <gtkmm.h>
#include <thread>



Connection::Connection(asio::io_context& ioc) :
    socket   (ioc),
    send_timer    (ioc),
    nickname ("User" + std::to_string(++current_id))
    { send_timer.expires_at(asio::steady_timer::time_point::min()); }

void Connection::add_to_send_buffer(std::string message) {
    send_buffer.append(message + "\n");
    send_timer.cancel();
    return;
}


Session::~Session() noexcept {
    if ( !ioc.stopped() ) ioc.stop();
    if ( ioc_thread.joinable() ) ioc_thread.join();
    std::cout << "[Server] stopped\n";
}

awaitable<void> Session::sender(std::shared_ptr<Connection> connection) {
    try {
        while (true) {
            while (true) {
                if (!connection->send_buffer.empty()) break;
                asio::error_code ec;
                co_await connection->send_timer.async_wait(asio::redirect_error(use_awaitable, ec));
                if (ec && ec != asio::error::operation_aborted) co_return;
            }
            std::string data;
            std::swap(data, connection->send_buffer);
            connection->send_timer.expires_at(asio::steady_timer::time_point::min());
            co_await asio::async_write(connection->socket, asio::buffer(data), use_awaitable);
        }
    } catch (const std::exception& e) {
        std::cout << "[Server] sender error: " << e.what() << "\n";
    }
}

awaitable<void> Session::receiver(std::shared_ptr<Connection> connection) {
    try {
        asio::streambuf buf;
        while (true) {
            std::size_t n = co_await asio::async_read_until(connection->socket, buf, '\n', use_awaitable);

            std::string line(
                asio::buffers_begin(buf.data()),
                asio::buffers_begin(buf.data()) + static_cast<std::ptrdiff_t>(n)
            );
            buf.consume(n);

            process_message(line, connection);
        }
    } catch (const std::exception& e) {
        std::cout << "[Server] " << connection->nickname << " disconnected: " << e.what() << "\n";
    }
}

void Session::set_poster(std::function<void(void)> message_poster)
    { poster = std::move(message_poster); }

void Session::set_disconnecter(std::function<void(void)> new_disconnecter)
    { disconnecter = std::move(new_disconnecter); }

Client::Client(const std::string &host, unsigned port) :
    Session::Session(),
    connection (std::make_shared<Connection>(ioc)) {
    connection->nickname = "Server";
    this->host = host;
    this->port = port;

    if ( port > 65535 ) {
        std::cerr << "Port out of bounds (0-65535)\n";
        throw std::out_of_range("Port out of bounds (0-65535)");
    }
    connection->send_timer.expires_at(asio::steady_timer::time_point::min());
    tcp::resolver resolver(ioc);

    std::cout << "Connecting to " << host << ":" << port << "...\n";

    asio::error_code ec;
    auto endpoints = resolver.resolve(host, std::to_string(port), ec);
    if (ec) {
        std::cerr << "[error] resolve(): " << ec.message()
                  << "\nIs the server running?\n";
        throw std::runtime_error("Failed to resolve host");
    }

    asio::connect(connection->socket, endpoints, ec);
    if (ec) {
        std::cerr << "[error] connect(): " << ec.message()
                  << "\nIs the server running?\n";
        throw std::runtime_error("Failed to connect to server");
    }

    // Spawn receiver and sender
    asio::co_spawn(ioc, receiver(connection), asio::detached);
    asio::co_spawn(ioc, sender(connection), asio::detached);

    // Run the event loop in separate thread until ioc.stop() is called
    ioc_thread = std::thread([this](){ ioc.run(); });
}


Client::~Client() noexcept {
    if ( connection->socket.is_open() )
        connection->socket.close();
}

awaitable<void> Client::receiver(std::shared_ptr<Connection> connection) {
    co_await Session::receiver(connection);
    if ( connection->socket.is_open() )
        connection->socket.close();
    if (disconnecter) disconnecter();
}

void Client::broadcast(const std::string& message, std::shared_ptr<Connection> origin) {
    (void) origin;
    connection->send_buffer += message + "\n";
    connection->send_timer.cancel();
}

void Client::process_message(std::string &message, std::shared_ptr<Connection> origin) {
    while (!message.empty() && (message[message.size()-1] == '\n' || message[message.size()-1] == '\r'))
        message.erase(message.size()-1);

    if ( origin ) {
        std::lock_guard<std::mutex> lock(receive_mutex);
        receive_queue.push_back(message);
        if (poster) poster();
    }
    else {
        broadcast(message, origin);
    }
}


Server::Server(unsigned port) : Session::Session(),
    acceptor (tcp::acceptor(ioc, tcp::endpoint(tcp::v4(), static_cast<asio::ip::port_type>(port)))) {
    host = "Server";
    asio::co_spawn(ioc, accept_loop(), asio::detached);
    ioc_thread = std::thread([this](){ ioc.run(); });
    std::cout << "[Server] listening on port " << port << "\n";
}

Server::~Server() noexcept {
    acceptor.close();
    for (auto& connection : connections) {
        if (connection->socket.is_open())
            connection->socket.close();
        connection->send_timer.cancel();
    }
}

awaitable<void> Server::receiver(std::shared_ptr<Connection> connection) {
    co_await Session::receiver(connection);
    if ( connection->socket.is_open() )
        connection->socket.close();
    connections.remove(connection);
    std::string farewell{connection->nickname + " has left the chat!!!"};
    process_message(farewell);
}

awaitable<void> Server::accept_loop() {
    while (true) {
        auto connection = std::make_shared<Connection>(ioc);
        asio::error_code ec;
        co_await acceptor.async_accept(connection->socket, asio::redirect_error(use_awaitable, ec));
        if (ec) {
            std::cout << "[server] accept loop ending: " << ec.message() << "\n";
            co_return;
        }

        try {
            auto ep = connection->socket.remote_endpoint();
            connection->fingerprint = ep.address().to_string() + ":" + std::to_string(ep.port());
        } catch (...) { connection->fingerprint = "unknown"; }
        std::cout << "[server] new client: " << connection->nickname << " (" << connection->fingerprint << ")\n";

        std::string message {connection->nickname + " says Hi!!!\n"};
        process_message(message);

        asio::co_spawn(ioc, receiver(connection), asio::detached);
        asio::co_spawn(ioc, sender(connection),   asio::detached);

        connections.push_back(connection);
    }
}

void Server::broadcast(const std::string& message, std::shared_ptr<Connection> origin) {
    for (auto& connection : connections) {
        std::lock_guard<std::mutex> send_lock(connection->send_mutex);
        if ( !origin )
            connection->send_buffer += message + "\n";
        else if ( connection->fingerprint == origin->fingerprint )
            connection->send_buffer += "(You): " + message + "\n";
        else
            connection->send_buffer += "(" + origin->nickname + "): " + message + "\n";
        connection->send_timer.cancel();
    }
}

void Server::process_message(std::string &message, std::shared_ptr<Connection> connection) {
    (void) connection;
    while (!message.empty() && (message[message.size()-1] == '\n' || message[message.size()-1] == '\r'))
        message.erase(message.size()-1);

    broadcast(message, connection);

    if ( connection )
        message = "(" + connection->nickname + "): " + message;

    std::lock_guard<std::mutex> lock(receive_mutex);
    receive_queue.push_back(message);
    if (poster) poster();
}


Chat::Chat() : Gtk::Box(Gtk::Orientation::VERTICAL) {
    // Load the GtkBuilder file and instantiate its widgets, check for errors
    auto ref_builder {Gtk::Builder::create()};
    try {
        ref_builder->add_from_file("res/gtk/chat_app.ui");
    } catch(const Glib::FileError& ex) {
        std::cerr << "FileError: " << ex.what() << "\n";
        throw ex;
    } catch(const Glib::MarkupError& ex) {
        std::cerr << "MarkupError: " << ex.what() << "\n";
        throw ex;
    } catch(const Gtk::BuilderError& ex) {
        std::cerr << "BuilderError: " << ex.what() << "\n";
        throw ex;
    }

    // Get the GtkBuilder-instantiated objects:
    auto header {Gtk::manage(ref_builder->get_widget<Gtk::Box>("header_box"))};

    home_button     = ref_builder->get_widget<Gtk::Button>("home_button");
    connect_button  = ref_builder->get_widget<Gtk::Button>("connect_button");
    message_button  = ref_builder->get_widget<Gtk::Button>("message_button");

    ip_entry        = ref_builder->get_widget<Gtk::Entry>("ip_entry");
    port_entry      = ref_builder->get_widget<Gtk::Entry>("port_entry");
    message_entry   = ref_builder->get_widget<Gtk::Entry>("message_entry");

    chat_scrolled   = Gtk::manage(ref_builder->get_widget<Gtk::ScrolledWindow>("chat_scrolled"));
    chat_box        = Gtk::manage(ref_builder->get_widget<Gtk::Box>("chat_box"));
    footer_box      = Gtk::manage(ref_builder->get_widget<Gtk::Box>("footer_box"));

    // Add Callbacks
    if(home_button) [[likely]]
        home_button->signal_clicked().connect
        ( [this](){ ip_entry->set_text(LOCALHOST); port_entry->set_text(DEFAULT_PORT); } );
    if(ip_entry) [[likely]]
        ip_entry->signal_activate().connect([this](){ session_connection(); });
    if(port_entry) [[likely]]
        port_entry->signal_activate().connect([this](){ session_connection(); });
    if(connect_button) [[likely]]
        connect_button->signal_clicked().connect([this](){ session_connection(); });

    if(message_entry) [[likely]]
        message_entry->signal_activate().connect( [this] () { message_buffer(); });
    if(message_button) [[likely]]
        message_button->signal_clicked().connect( [this] () { message_buffer(); });

    // Insert elements into Browser Box
    insert_child_at_start(*header);
    append(*chat_scrolled);
    append(*footer_box);

    auto css_provider = Gtk::CssProvider::create();
    css_provider->load_from_path("./res/gtk/chat_app.css");

    // Add to the default display
    Gtk::StyleContext::add_provider_for_display(
        Gdk::Display::get_default(),
        css_provider,
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );
}

void Chat::on_realize() {
    Gtk::Box::on_realize();
    status_label = dynamic_cast<Gtk::Label *>(get_parent()->get_parent()->get_parent()->get_parent()->get_last_child());
    if (status_label)
        status_label->set_text("Welcome to the LAN Chat!");
    else
        std::cout << "Status label not found\n";
}

inline void Chat::session_connection() {
    // Disconnect if connected
    if (!connect_button->get_label().compare("Disconnect")) {
        session = nullptr;

        connect_button->set_label("Connect");
        footer_box->set_visible(false);
        ip_entry->set_sensitive(true);
        port_entry->set_sensitive(true);
        home_button->set_sensitive(true);
        if (status_label)
            status_label->set_label("Disconnected!");
        return;
    }

    // Parse host / port
    auto port { static_cast<unsigned>(std::atoi(port_entry->get_text().c_str())) };
    auto host { ip_entry->get_text().lowercase().compare("localhost") ? ip_entry->get_text().c_str() : LOCALHOST };

    // Try to connect as client
    try {
        session = std::make_unique<Client>(host, port);
        status_label->set_label("Connected to server " + std::string(host) + ":" + std::to_string(port) + "!");
    } catch (const std::exception& e) {
        session = nullptr;
        // If not localhost, give up
        if ( !(host == std::string(LOCALHOST)) ) {
            std::cerr << "[fatal] while creating session: " << e.what() << "\n";
            if (status_label)
                status_label->set_label("Something went wrong starting client, check cerr");
            return;
        }
        try {
            session  = std::make_unique<Server>(port);
            if (status_label)
                status_label->set_label("Hosting on port " + std::to_string(port) + " — waiting for peers!");
        } catch (const std::exception& e2) {
            std::cerr << "[fatal] could not start server: " << e2.what() << "\n";
            if (status_label)
                status_label->set_label("Could not start server, check cerr");
            session = nullptr;
            return;
        }
    }

    connect_button->set_label("Disconnect");
    ip_entry->set_sensitive(false);
    port_entry->set_sensitive(false);
    home_button->set_sensitive(false);
    footer_box->set_visible(true);
    message_entry->grab_focus();

    session->set_disconnecter([this]() {
        Glib::signal_idle().connect_once([this]() { session_connection(); });
    });
    dispatcher = std::make_unique<Glib::Dispatcher>();
    dispatcher->connect([this]() {
        std::lock_guard<std::mutex> lock(session->receive_mutex);
        while (!session->receive_queue.empty()) {
            std::string message{std::move(session->receive_queue.front())};
            session->receive_queue.pop_front();

            std::cout << message << "\n";

            auto bubble {Gtk::manage(new Gtk::Label())};
            bubble->set_css_classes({ "bubble" });
            std::smatch matches{};
            if ( std::regex_search(message, matches, std::regex(R"(^\((\w+)\):)")) ) {
                if ( matches[1].str() == "You" ) {
                    bubble->set_xalign(1.0);
                    bubble->add_css_class("you");
                } else {
                    bubble->set_xalign(0.0);
                    bubble->add_css_class("others");
                }
            }
            Glib::ustring valid_message{std::move(message)};
            valid_message.make_valid();
            bubble->set_text(std::move(valid_message));
            bubble->set_hexpand(true);
            bubble->set_vexpand(false);
            bubble->set_wrap(true);
            bubble->set_wrap_mode(Pango::WrapMode::WORD_CHAR);
            chat_box->append(*bubble);

            Glib::signal_idle().connect_once([this]() {
                auto adj = chat_scrolled->get_vadjustment();
                adj->set_value(adj->get_upper() - adj->get_page_size());
            });
        }
    });
    session->set_poster([this]() { dispatcher->emit(); });
    return;
}

inline void Chat::message_buffer () {
    if ( !message_entry->get_text_length() ) return;
    std::string message{message_entry->get_text()};
    session->process_message(message);
    message_entry->delete_text(0, -1);
};
