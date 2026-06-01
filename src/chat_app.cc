#include "include/chat_app.hpp"
#include "include/session.hpp"

#include <gtkmm.h>



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
