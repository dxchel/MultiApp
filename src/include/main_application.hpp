#ifndef _MAIN_WINDOW_
#define _MAIN_WINDOW_

#include <gtkmm.h>


/**
 * @brief MainApplication class containing other apps in project.
 * 
 * Gtk::Application that initializes a window containing needed
 * Widgets for containing core apps in the project and running functions.
 */
class MainApplication : public Gtk::Application
{
    friend class MainApplicationTester;

    Gtk::ApplicationWindow *mainWindow;

    Gtk::ApplicationWindow* create_window();

protected:
    /**
     * @brief Creates MainApplication object with application id and handles.
     */
    MainApplication();

    /**
     * @brief Presents main window.
     */
    void on_activate() override;

public:
    /**
     * @brief Creates RefPtr using class constructor.
     * 
     * @return New MainApplication object RefPtr.
     */
    static Glib::RefPtr<MainApplication> create();
};

#endif