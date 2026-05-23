#include "include/chat_app.hpp"

#include <algorithm>
#include <asio.hpp>
#include <iostream>

#include <gtkmm.h>
#include <thread>

Session::Session(const std::string &host, unsigned port) : host(host), port(port)
{
    if ( port > 65535 )
    {
        std::cerr << "Port out of bounds (0-65535)" << std::endl;
        throw std::out_of_range("Port out of bounds (0-65535)");
    }
}

Session::~Session() noexcept
{
    if ( !ioc.stopped() ) ioc.stop();
    if ( ioc_thread.joinable() ) ioc_thread.join();
    sockets.clear();
}

void Session::connect()
{
    send_timer = std::make_unique<asio::steady_timer>(ioc);
    send_timer->expires_at(asio::steady_timer::time_point::min());
    tcp::resolver resolver(ioc);

    sockets.emplace_back(ioc);

    std::cout << "Connecting to " << host << ":" << port << "...\n";

    asio::error_code ec;
    auto endpoints = resolver.resolve(host, std::to_string(port), ec);
    if (ec) {
        std::cerr << "[error] resolve(): " << ec.message()
                  << std::endl << "Is the server running?" << std::endl;
        throw std::runtime_error("Failed to resolve host");
    }

    asio::connect(sockets[0], endpoints, ec);
    if (ec) {
        std::cerr << "[error] connect(): " << ec.message()
                  << std::endl << "Is the server running?" << std::endl;
        throw std::runtime_error("Failed to connect to server");
    }

    // Spawn receiver and sender
    asio::co_spawn(ioc, receiver(sockets[0]), asio::detached);
    asio::co_spawn(ioc, sender(sockets[0]), asio::detached);

    // Run the event loop in separate thread until ioc.stop() is called
    ioc_thread = std::thread([this](){ ioc.run(); });
}

awaitable<void> Session::sender(tcp::socket& socket) {
    try {
        while (true) {
            // Wait until there's something to send
            while (send_buf.empty()) {
                asio::error_code ec;
                co_await send_timer->async_wait(
                    asio::redirect_error(use_awaitable, ec));
                if (ec && ec != asio::error::operation_aborted) co_return;
            }

            std::string data;
            std::swap(data, send_buf);

            co_await asio::async_write(socket, asio::buffer(data), use_awaitable);

            // Reset timer so we wait again next iteration
            send_timer->expires_at(asio::steady_timer::time_point::min());
        }
    } catch (const std::exception&) {
        std::cout << std::endl << "[disconnected sender from server due to exception]" << std::endl;
    }

    if ( socket.is_open() ) socket.close();
    if ( !ioc.stopped() ) ioc.stop();
}

awaitable<void> Session::receiver(tcp::socket& socket) {
    try {
        asio::streambuf buf;

        while (true) {
            // Receive until a '\n'
            std::size_t n = co_await asio::async_read_until(
                socket, buf, '\n', use_awaitable);

            Glib::ustring line(
                asio::buffers_begin(buf.data()),
                asio::buffers_begin(buf.data()) + static_cast<std::ptrdiff_t>(n));
            buf.consume(n);

            // Strip \r\n and make Glib valid
            while (!line.empty() && (line[line.size() - 1] == '\n' || line[line.size() - 1] == '\r'))
                line.erase(line.size() - 1);
            if ( !line.size() ) continue;
            line.make_valid();

            {
                std::lock_guard<std::mutex> lock(queue_mutex);
                message_queue.push_back(std::string(line.c_str()));
            }
            poster();
        }
    } catch (const std::exception&) {
        std::cout << std::endl << "[disconnected receiver from server due to exception]" << std::endl;
    }

    if ( socket.is_open() ) socket.close();
    if ( !ioc.stopped() ) ioc.stop();
    if ( disconnecter ) disconnecter();
}

void Session::add_to_buffer(std::string message)
{
    send_buf.append(message + "\n");
    send_timer->cancel();
    return;
}

void Session::set_poster(std::function<void(void)> message_poster)
    { poster = std::move(message_poster); }

void Session::set_disconnecter(std::function<void(void)> new_disconnecter)
    { disconnecter = std::move(new_disconnecter); }


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

    // Get the GtkBuilder-instantiated objects:
    auto header {Gtk::manage(ref_builder->get_widget<Gtk::Box>("header_box"))};

    home_button = ref_builder->get_widget<Gtk::Button>("home_button");
    connect_button = ref_builder->get_widget<Gtk::Button>("connect_button");
    message_button = ref_builder->get_widget<Gtk::Button>("message_button");

    ip_entry = ref_builder->get_widget<Gtk::Entry>("ip_entry");
    port_entry = ref_builder->get_widget<Gtk::Entry>("port_entry");
    message_entry = ref_builder->get_widget<Gtk::Entry>("message_entry");

    chat_scrolled = Gtk::manage(ref_builder->get_widget<Gtk::ScrolledWindow>("chat_scrolled"));
    chat_box = Gtk::manage(ref_builder->get_widget<Gtk::Box>("chat_box"));
    footer_box = Gtk::manage(ref_builder->get_widget<Gtk::Box>("footer_box"));

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
}

// Get StatusLabel from parent
void Chat::on_realize()
{
    Gtk::Box::on_realize();
    status_label = dynamic_cast<Gtk::Label *>(get_parent()->get_parent()->get_parent()->get_parent()->get_last_child());
    if (!status_label) std::cout << "Status label not found" << std::endl;
    if (status_label)
        status_label->set_text("Welcome to the LAN Chat!");
}

inline void Chat::session_connection()
{
    // Disconnect if connected
    if (!connect_button->get_label().compare("Disconnect"))
    {
        session = nullptr;
        std::cout << "\n[disconnected from server due to user request]\n";

        connect_button->set_label("Connect");
        footer_box->set_visible(false);
        ip_entry->set_sensitive(true);
        port_entry->set_sensitive(true);
        status_label->set_label("Disonnected from server!");
        return;
    }

    // Else connect as client
    auto port { static_cast<unsigned>(std::atoi(port_entry->get_text().c_str())) };
    auto host { ip_entry->get_text().lowercase().compare("localhost") ? ip_entry->get_text().c_str() : LOCALHOST };

    try {
        session = std::make_unique<Session>(host, port);
        session->connect();
        session->set_disconnecter([this]() { session_connection(); });
        session->set_poster([this]() { poster(); });
    } catch (const std::exception& e) {
        std::cerr << "[fatal] while creating session " << e.what() << std::endl;
        status_label->set_label("Something went wrong when trying to connect, check cerr");
        session = nullptr;
        return;
    }

    connect_button->set_label("Disconnect");
    ip_entry->set_sensitive(false);
    port_entry->set_sensitive(false);
    footer_box->set_visible(true);
    message_entry->grab_focus();
    status_label->set_label("Connected to server " + std::string(host) + ":" + std::to_string(port) + "!");
    return;
}

inline void Chat::message_buffer ()
{
    if ( !message_entry->get_text_length() ) return;
    session->add_to_buffer(message_entry->get_text());
    message_entry->delete_text(0, -1);
};

inline void Chat::poster ()
{
    dispatcher.connect([this]() {
        std::lock_guard<std::mutex> lock(session->queue_mutex);
        while (!session->message_queue.empty()) {
            std::string message{std::move(session->message_queue.front())};
            session->message_queue.pop_front();

            std::cout << message << std::endl;
            auto bubble {Gtk::manage(new Gtk::Label())};
            if ( message.compare(0, 5, "(you)") )
            {
                bubble->set_xalign(0.0);
                bubble->set_margin_end(100);
            } else
            {
                bubble->set_xalign(1.0);
                bubble->set_margin_start(100);
            }
            bubble->set_text(std::move(message));
            bubble->set_hexpand(true);
            bubble->set_vexpand(false);
            bubble->set_wrap(true);
            bubble->set_wrap_mode(Pango::WrapMode::WORD_CHAR);
            bubble->set_margin(7);
            chat_box->append(*bubble);

            Glib::signal_idle().connect_once([this]()
            {
                auto adj = chat_scrolled->get_vadjustment();
                adj->set_value(adj->get_upper() - adj->get_page_size());
            });
        }
    });
    dispatcher.emit();
};
