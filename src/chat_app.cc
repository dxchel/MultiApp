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
        (
            [this](){ ip_entry->set_text("localhost"); port_entry->set_text("1234");}
        );
    if(ip_entry) [[likely]]
        ip_entry->signal_activate().connect([](){ std::cout << "ip entered!" << std::endl;});
    if(port_entry) [[likely]]
        port_entry->signal_activate().connect([](){ std::cout << "ip entered!" << std::endl;});
    if(connect_button) [[likely]]
        connect_button->signal_clicked().connect([this]()
        {
            std::cout << connect_button->get_label() << "ing" << std::endl;
            connect_button->set_label(connect_button->get_label().compare("Connect") ? "Connect" : "Disconnect");
            footer_box->set_visible(!footer_box->get_visible());
            ip_entry->set_sensitive(!ip_entry->get_sensitive());
            port_entry->set_sensitive(!port_entry->get_sensitive());
        });
    if(message_button) [[likely]]
        message_button->signal_clicked().connect([](){ std::cout << "Messaging" << std::endl;});

    // Insert elements into Browser Box
    insert_child_at_start(*header);
    append(*chat_scrolled);
    append(*footer_box);
}

void Chat::on_realize() {
    Gtk::Box::on_realize();
    status_label = dynamic_cast<Gtk::Label *>(get_parent()->get_parent()->get_parent()->get_parent()->get_last_child());
    if (!status_label) std::cout << "Status label not found" << std::endl;
    if (status_label)
        status_label->set_text("Welcome to the LAN Chat!");
}

