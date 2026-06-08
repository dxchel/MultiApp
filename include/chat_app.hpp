#pragma once

#include "session.hpp"

#include <gtkmm.h>



/**
 * @brief Chat class containing important Widgets and functions for Chat functionality.
 *
 * Gtk::Box implementing class that contains important Gtk Widgets
 * for connecting and sending messages to another app.
 */
class Chat : public Gtk::Box {
    friend class ChatTest;
    friend class ChatTest_ChatFunctionalTest_Test;

public:
    /**
     * @brief Creates Chat object with all needed Widgets and signals.
     *
     * Creates Chat object using resources/gtk/chat_app.ui file,
     * Checks for file issues, gets Widgets and connects needed signals.
     */
    Chat();

private:
    Gtk::Button         *home_button{};
    Gtk::Button         *connect_button{};
    Gtk::Button         *message_button{};
    Gtk::Entry          *ip_entry{};
    Gtk::Entry          *port_entry{};
    Gtk::Entry          *message_entry{};
    Gtk::Box            *chat_box{};
    Gtk::Box            *footer_box{};
    Gtk::ScrolledWindow *chat_scrolled{};
    Gtk::Label          *status_label{};

    std::unique_ptr<Glib::Dispatcher> dispatcher{};

    std::unique_ptr<Session> session{};

    /**
     * @brief Get Main Application status label.
     *
     * Gets Main Application status label after realize due to
     * the box needing to be contained.
     */
    void on_realize() override;

    /**
     * @brief Buffers the message from the Entry to the Session Buffer for sending.
     *
     * Does nothing if Entry is empty, else it adds the message and empties the Entry.
     */
    void message_buffer();

    /**
     * @brief Tries connecting as client, falls back to hosting if localhost and no server found.
     *
     * Checks the button to see if it needs to connect or disconnect, then tries
     * to connect as client and adds any needed information to the Session object.
     * If the host is localhost and the connection fails, starts a Server and
     * connects to it.
     */
    void session_connection();
};
