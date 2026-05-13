#ifndef _CHAT_APP_
#define _CHAT_APP_

#include <gtkmm.h>


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

    /**
     * @brief Get Main Application status label.
     *
     * Gets Main Application status label after realize due to
     * the box needing to be contained.
     */
    void on_realize() override;

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