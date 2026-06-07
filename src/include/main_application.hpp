#pragma once

#include <gtkmm.h>


/**
 * @brief MainApplication class containing other apps in project.
 *
 * Gtk::Application that initializes a window containing needed
 * Widgets for containing core apps in the project and running functions.
 */
class MainApplication : public Gtk::Application {
    friend class MainApplicationTest;
    friend class MainApplicationTest_MainApplicationStructureTest_Test;

public:
    Gtk::Label *status_label{};

    /**
     * @brief Creates RefPtr using class constructor.
     *
     * @return New MainApplication object RefPtr.
     */
    static Glib::RefPtr<MainApplication> create();

private:
    Gtk::ApplicationWindow *main_window{};
    Gtk::Box               *selected_app{};

    std::vector<std::pair<std::string, std::function<Gtk::Box*()>>> apps{};

    /**
     * @brief Creates ApplicationWindow to show.
     *
     * Creates and populates the main ApplicationWindow using
     * res/gtk/main_app.ui file as base and adding Apps into it.
     *
     * @return New ApplicationWindow to use in the program.
     */
    Gtk::ApplicationWindow* create_window();

    /**
     * @brief Creates MainApplication object with application id and handles.
     */
    MainApplication();

    /**
     * @brief Presents main window.
     */
    void on_activate() override;
};
