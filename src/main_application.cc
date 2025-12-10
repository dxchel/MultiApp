#include "../include/main_application.hpp"
#include "../include/browser_app.hpp"


MainApplication::MainApplication() :
    Gtk::Application("org.xchel.multiapp", Gio::Application::Flags::HANDLES_OPEN) {}

Glib::RefPtr<MainApplication> MainApplication::create()
{
    return Glib::make_refptr_for_instance<MainApplication>(new MainApplication());
}

void MainApplication::on_activate()
{
    // The application has been started, so let's show a window.
    create_window()->present();
}

Gtk::Window* MainApplication::create_window()
{
    mainWindow = Gtk::manage(new Gtk::ApplicationWindow());
    mainWindow->set_default_size(800, 600);
    mainWindow->maximize();
    mainWindow->set_title("GUI MultiApp");
    mainWindow->set_default_icon_name("applications-engineering");

    auto mainBox = Gtk::manage(new Gtk::Box());
    mainBox->set_orientation(Gtk::Orientation::VERTICAL);
    mainWindow->set_child(*mainBox);

    auto mainLabel = Gtk::manage(new Gtk::Label());
    mainLabel->set_text("Page Status");
    mainLabel->set_justify(Gtk::Justification::CENTER);
    mainLabel->set_hexpand(true);
    mainBox->insert_child_at_start(*mainLabel);

    auto mainNotebook = Gtk::manage(new Gtk::Notebook());
    mainNotebook->set_vexpand(true);
    mainNotebook->set_scrollable(true);

    auto label = Gtk::manage(new Gtk::Label());
    label->set_text("Page Status");
    label->set_justify(Gtk::Justification::CENTER);
    label->set_hexpand(true);
    mainNotebook->append_page(*label);
    mainNotebook->set_current_page(0);
    mainBox->insert_child_at_start(*mainNotebook);

    add_window(*mainWindow);
    return mainWindow;
}