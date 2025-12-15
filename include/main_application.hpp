#ifndef _MAIN_WINDOW_
#define _MAIN_WINDOW_

#include <gtkmm.h>


enum class MainApplicationError
{
    no_error,
    application_error,
    window_error,
    box_error,
    notebook_error,
    browser_error,
    foot_error
};

class MainApplication : public Gtk::Application
{
    friend class MainApplicationTester;

    Gtk::Window *mainWindow;

    Gtk::Window* create_window();

protected:
    MainApplication();

    // Override default signal handlers:
    void on_activate() override;

public:
    static Glib::RefPtr<MainApplication> create();
};

#endif