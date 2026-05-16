#include "include/chat_app.hpp"
#include "gtkmm/enums.h"

#include <algorithm>
#include <asio/io_context.hpp>
#include <cstddef>
#include <iostream>

#include <gtkmm.h>
#include <string>
#include <thread>


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
        ip_entry->signal_activate().connect([this](){ conection_check(); });
    if(port_entry) [[likely]]
        port_entry->signal_activate().connect([this](){ conection_check(); });
    if(connect_button) [[likely]]
        connect_button->signal_clicked().connect([this](){ conection_check(); });

    if(message_entry) [[likely]]
        message_entry->signal_activate().connect( [this] () { message_buffer(); });
    if(message_button) [[likely]]
        message_button->signal_clicked().connect( [this] () { message_buffer(); });

    // Insert elements into Browser Box
    insert_child_at_start(*header);
    append(*chat_scrolled);
    append(*footer_box);
}

Chat::~Chat() noexcept
{
    if ( !ioc.stopped() ) ioc.stop();
    if ( ioc_thread.joinable() ) ioc_thread.join();
    for ( auto &socket : sockets )
        if ( socket.is_open() )
            socket.close();
    std::cout << "Goodbye from Chat!" << std::endl;
}

void Chat::on_realize()
{
    Gtk::Box::on_realize();
    status_label = dynamic_cast<Gtk::Label *>(get_parent()->get_parent()->get_parent()->get_parent()->get_last_child());
    if (!status_label) std::cout << "Status label not found" << std::endl;
    if (status_label)
        status_label->set_text("Welcome to the LAN Chat!");
}

awaitable<void> Chat::sender(tcp::socket& socket) {
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

awaitable<void> Chat::receiver(tcp::socket& socket) {
    try {
        asio::streambuf buf;

        while (true) {
            std::size_t n = co_await asio::async_read_until(
                socket, buf, '\n', use_awaitable);

            while ( !posted )
            {
                asio::error_code ec;
                co_await send_timer->async_wait(
                    asio::redirect_error(use_awaitable, ec));
                if (ec && ec != asio::error::operation_aborted) co_return;
            }

            Glib::ustring line(
                asio::buffers_begin(buf.data()),
                asio::buffers_begin(buf.data()) + static_cast<std::ptrdiff_t>(n));
            buf.consume(n);

            // Strip \r\n
            while (!line.empty() && (line[line.size() - 1] == '\n' || line[line.size() - 1] == '\r'))
                line.erase(line.size() - 1);
            line.make_valid();

            std::cout << line << std::endl << std::flush;

            // @TODO FIX first bubbles take the whole space
            post_to_gtk([this, line = std::move(line)]()
            {
                auto bubble {Gtk::manage(new Gtk::TextView())};
                bubble->set_hexpand(true);
                bubble->set_vexpand(false);
                bubble->set_editable(false);
                bubble->set_cursor_visible(false);
                bubble->set_justification(Gtk::Justification::LEFT);
                bubble->set_wrap_mode(Gtk::WrapMode::WORD_CHAR);
                bubble->get_buffer()->set_text(line);
                chat_box->append(*bubble);
                posted = true;
            });
        }
    } catch (const std::exception&) {
        std::cout << std::endl << "[disconnected receiver from server due to exception]" << std::endl;
    }

    if ( socket.is_open() ) socket.close();
    if ( !ioc.stopped() ) ioc.stop();
}

inline void Chat::conection_check()
{
    // Disconnect if connected
    // @TODO FIX on disconnect it needs two clicks to use again
    if (!connect_button->get_label().compare("Disconnect"))
    {
        if ( !ioc.stopped() ) ioc.stop();
        if ( ioc_thread.joinable() ) ioc_thread.join();
        sockets.clear();
        std::cout << "\n[disconnected from server due to user request]\n";

        connect_button->set_label("Connect");
        footer_box->set_visible(false);
        ip_entry->set_sensitive(true);
        port_entry->set_sensitive(true);
        return;
    }

    // Else connect as client
    auto port { static_cast<unsigned>(std::atoi(port_entry->get_text().c_str())) };
    if ( port > 65535 )
    {
        std::cerr << "Port out of bounds (0-65535)" << std::endl;
        status_label->set_label("Something went wrong when trying to connect, check cerr");
        return;
    }
    auto host { ip_entry->get_text().lowercase().compare("localhost") ? ip_entry->get_text().c_str() : LOCALHOST };

    try {
        if ( !send_timer ) send_timer = std::make_unique<asio::steady_timer>(ioc);
        send_timer->expires_at(asio::steady_timer::time_point::min());
        tcp::resolver resolver(ioc);

        sockets.emplace_back(ioc);

        std::cout << "Connecting to " << host << ":" << port << "...\n";

        // Resolve and connect synchronously before starting the event loop —
        // simplifies startup error handling significantly
        asio::error_code ec;
        auto endpoints = resolver.resolve(host, std::to_string(port), ec);
        if (ec) {
            std::cerr << "[error] resolve(): " << ec.message()
                      << std::endl << "Is the server running?" << std::endl;
            return;
        }

        asio::connect(sockets[0], endpoints, ec);
        if (ec) {
            std::cerr << "[error] connect(): " << ec.message()
                      << std::endl << "Is the server running?" << std::endl;
            return;
        }

        ioc.restart();
        asio::co_spawn(ioc, receiver(sockets[0]), asio::detached);
        asio::co_spawn(ioc, sender(sockets[0]), asio::detached);

        // Run the event loop until ioc.stop() is called
        ioc_thread = std::thread([this](){ ioc.run(); });

    } catch (const std::exception& e) {
        std::cerr << "[fatal] " << e.what() << std::endl;
        return;
    }

    connect_button->set_label("Disconnect");
    ip_entry->set_sensitive(false);
    port_entry->set_sensitive(false);
    footer_box->set_visible(true);
    message_entry->grab_focus();
    return;
}

void Chat::message_buffer ()
{
    if ( !message_entry->get_text_length() ) return;
    send_buf.append((message_entry->get_text() + "\n"));
    send_timer->cancel(); // wake the sender coroutine
    message_entry->delete_text(0, -1);
};

void Chat::post_to_gtk(std::function<void()> fn) {
    // Schedules fn to run on the GTK main loop thread
    Glib::signal_idle().connect_once(std::move(fn));
    posted = false;
}

