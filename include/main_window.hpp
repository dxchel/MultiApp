#ifndef _MAIN_WINDOW_
#define _MAIN_WINDOW_

#include <gtkmm.h>
#include <webkit/webkit.h>
#include <iostream>

namespace
{
Gtk::Window* mainDialog{};
Glib::RefPtr<Gtk::Application> app{};

void on_button_clicked();
void on_app_activate();
}

#endif