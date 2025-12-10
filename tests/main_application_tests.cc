#include "include/tests.hpp"


MainApplicationError mainApplicationTests()
{
    auto mainApp = MainApplication::create();
    if(!mainApp)
    {
        std::cerr << "Main Application couldn't be created." << std::endl;
        return MainApplicationError::application_missing;
    }
    auto window{mainApp->create_window()};
    if(!window | (window->get_name() != "main_window"))
    {
        std::cerr << "Main Window couldn't be created." << std::endl;
        return MainApplicationError::window_missing;
    }
    auto box{window->get_child()};
    if(!box | (box->get_name() != "main_box"))
    {
        std::cerr << "Main Box couldn't be created." << std::endl;
        return MainApplicationError::box_missing;
    }
    auto *notebook{dynamic_cast<Gtk::Notebook*>(box->get_first_child())};
    if(!notebook | (notebook->get_name() != "main_notebook"))
    {
        std::cerr << "Main Notebook couldn't be created." << std::endl;
        return MainApplicationError::notebook_missing;
    }
    auto browser{notebook->get_nth_page(0)};
    if(!browser | (browser->get_name() != "main_browser"))
    {
        std::cerr << "Browser Box couldn't be created." << std::endl;
        return MainApplicationError::browser_missing;
    }
    auto foot{notebook->get_next_sibling()};
    if(!foot | (foot->get_name() != "label_foot"))
    {
        std::cerr << "Main Foot Label couldn't be created." << std::endl;
        return MainApplicationError::foot_missing;
    }
    return MainApplicationError::no_error;
}
