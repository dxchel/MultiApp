#ifndef _MAIN_WINDOW_
#define _MAIN_WINDOW_

#include <gtkmm.h>

enum class MainApplicationError {no_error, application_missing, window_missing, box_missing, notebook_missing, browser_missing, foot_missing};

class MainApplication : public Gtk::Application
{
    Gtk::Window *mainWindow{};
    friend MainApplicationError mainApplicationTests();

    Gtk::Window* create_window();

protected:
    MainApplication();

    // Override default signal handlers:
    void on_activate() override;

public:
    static Glib::RefPtr<MainApplication> create();
};

#endif