#include "include/main_application.hpp"
#include "include/browser_app.hpp"

#include <iostream>


MainApplication::MainApplication() :
    Gtk::Application("org.xchel.multiapp", Gio::Application::Flags::HANDLES_OPEN),
    mainWindow {} {}

Glib::RefPtr<MainApplication> MainApplication::create()
{
    return Glib::make_refptr_for_instance<MainApplication>(new MainApplication());
}

void MainApplication::on_activate()
{
    // The application has been started, so let's show a window.
    create_window()->present();
}

Gtk::ApplicationWindow* MainApplication::create_window()
{
    // Load the GtkBuilder file and instantiate its widgets, check for errors
    auto refBuilder {Gtk::Builder::create()};
    try
    {
        refBuilder->add_from_file("res/gtk/main_app.ui");
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
    mainWindow = Gtk::manage(refBuilder->get_widget<Gtk::ApplicationWindow>("main_window"));
    auto mainBrowserBox {Gtk::manage(refBuilder->get_widget<Gtk::Box>("main_browser_box"))};
    auto browserBox {Gtk::manage(new Browser())};
    mainBrowserBox->insert_child_at_start(*browserBox);

    add_window(*mainWindow);
    return mainWindow;
}