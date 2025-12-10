#ifndef _MAIN_WINDOW_
#define _MAIN_WINDOW_

#include <gtkmm.h>

class MainApplication : public Gtk::Application
{
    Gtk::Window *mainWindow{};

    Gtk::Window* create_window();

protected:
    MainApplication();

    // Override default signal handlers:
    void on_activate() override;

public:
    static Glib::RefPtr<MainApplication> create();
};

#endif