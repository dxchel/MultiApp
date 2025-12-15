#include "include/tests.hpp"

#include <iostream>


MainApplicationTester::MainApplicationTester() : mainApp (MainApplication::create()) {};

MainApplicationError MainApplicationTester::mainApplicationStructureTests() const
{
    if(!mainApp)
    {
        std::cerr << "Main Application Error." << std::endl;
        return MainApplicationError::application_error;
    }
    auto window {dynamic_cast<Gtk::Window *>(mainApp->create_window())};
    if(!window)
    {
        std::cerr << "Main Window Error." << GTK_IS_WINDOW(window)<< std::endl;
        return MainApplicationError::window_error;
    }
    auto box {dynamic_cast<Gtk::Box *>(window->get_child())};
    if(!box)
    {
        std::cerr << "Main Box Error." << GTK_IS_BOX(box) << std::endl;
        return MainApplicationError::box_error;
    }
    auto notebook {dynamic_cast<Gtk::Notebook *>(box->get_first_child())};
    if(!notebook)
    {
        std::cerr << "Main Notebook Error." << std::endl;
        return MainApplicationError::notebook_error;
    }
    auto browser {dynamic_cast<Gtk::Box *>(notebook->get_first_child())};
    if(!browser)
    {
        std::cerr << "Browser Box Error." << std::endl;
        return MainApplicationError::browser_error;
    }
    auto foot {dynamic_cast<Gtk::Label *>(notebook->get_next_sibling())};
    if(!foot)
    {
        std::cerr << "Main Foot Label Error." << std::endl;
        return MainApplicationError::foot_error;
    }
    return MainApplicationError::no_error;
}
