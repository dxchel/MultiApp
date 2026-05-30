#include "include/main_application.hpp"
#include "include/browser_app.hpp"
#include "include/fractal_app.hpp"
#include "include/chat_app.hpp"

#include <iostream>

#include <gtkmm.h>

MainApplication::MainApplication() :
    Gtk::Application("org.xchel.multiapp", Gio::Application::Flags::HANDLES_OPEN | Gio::Application::Flags::NON_UNIQUE) {}

Glib::RefPtr<MainApplication> MainApplication::create() {
    return Glib::make_refptr_for_instance<MainApplication>(new MainApplication());
}

void MainApplication::on_activate() {
    create_window()->present();
}

Gtk::ApplicationWindow* MainApplication::create_window() {
    // Load the GtkBuilder file and instantiate its widgets, check for errors
    auto ref_builder {Gtk::Builder::create()};
    try {
        ref_builder->add_from_file("res/gtk/main_app.ui");
    } catch(const Glib::FileError& ex) {
        std::cerr << "FileError: " << ex.what() << std::endl;
        throw ex;
    } catch(const Glib::MarkupError& ex) {
        std::cerr << "MarkupError: " << ex.what() << std::endl;
        throw ex;
    } catch(const Gtk::BuilderError& ex) {
        std::cerr << "BuilderError: " << ex.what() << std::endl;
        throw ex;
    }

    status_label = Gtk::manage(ref_builder->get_widget<Gtk::Label>("main_status_label"));

    main_window  = Gtk::manage(ref_builder->get_widget<Gtk::ApplicationWindow>("main_window"));

    auto main_notebook {Gtk::manage(ref_builder->get_widget<Gtk::Notebook>("main_notebook"))};
    auto main_box      {Gtk::manage(ref_builder->get_widget<Gtk::Box>("Chat"))};

    selected_app = Gtk::manage(new Chat());
    main_box->append(*selected_app);

    main_notebook->signal_switch_page().connect([this](Gtk::Widget* page, guint page_number) {
        auto app {dynamic_cast<Gtk::Box *>(page)};
        if (selected_app) {
            selected_app->unparent();
            selected_app = nullptr;
        }

        switch(page_number) {
        case 0:
            selected_app = Gtk::manage(new Chat());
            app->append(*selected_app);
            break;
        case 1:
            selected_app = Gtk::manage(new FractalBox());
            app->append(*selected_app);
            break;
        case 2:
            selected_app = Gtk::manage(new Browser());
            app->append(*selected_app);
            break;
        }
    });

    add_window(*main_window);
    return main_window;
}