#include "include/tests.hpp"


MainApplicationError mainApplicationTests()
{
    auto mainApp = MainApplication::create();
    if(!mainApp)
    {
        std::cerr << "Main Application Error." << std::endl;
        return MainApplicationError::application_error;
    }
    auto window{mainApp->create_window()};
    if(!window || (window->get_name() != "main_window"))
    {
        std::cerr << "Main Window Error." << std::endl;
        return MainApplicationError::window_error;
    }
    auto box{window->get_child()};
    if(!box || (box->get_name() != "main_box"))
    {
        std::cerr << "Main Box Error." << std::endl;
        return MainApplicationError::box_error;
    }
    auto *notebook{dynamic_cast<Gtk::Notebook*>(box->get_first_child())};
    if(!notebook || (notebook->get_name() != "main_notebook"))
    {
        std::cerr << "Main Notebook Error." << std::endl;
        return MainApplicationError::notebook_error;
    }
    auto browser{notebook->get_nth_page(0)};
    if(!browser || (browser->get_name() != "main_browser"))
    {
        std::cerr << "Browser Box Error." << std::endl;
        return MainApplicationError::browser_error;
    }
    auto foot{notebook->get_next_sibling()};
    if(!foot || (foot->get_name() != "label_foot"))
    {
        std::cerr << "Main Foot Label Error." << std::endl;
        return MainApplicationError::foot_error;
    }
    return MainApplicationError::no_error;
}
