#include "include/tests.hpp"


BrowserAppError browserAppTests()
{
    auto browser = Gtk::manage(new Browser());
    if(!browser || browser->get_name() != "main_browser")
    {
        std::cerr << "Browser Box Error." << std::endl;
        return BrowserAppError::browser_error;
    }
    auto header = browser->get_first_child();
    if(!header || header->get_name() != "header_bar")
    {
        std::cerr << "Header Bar Error." << std::endl;
        return BrowserAppError::header_error;
    }
    auto back = header->get_first_child();
    if(!back || back->get_name() != "back_button")
    {
        std::cerr << "Back Button Error in header." << std::endl;
        return BrowserAppError::back_error;
    }
    auto forward = back->get_next_sibling();
    if(!forward || forward->get_name() != "forward_button")
    {
        std::cerr << "Forward Button Error in header." << std::endl;
        return BrowserAppError::forward_error;
    }
    auto home = forward->get_next_sibling();
    if(!home || home->get_name() != "home_button")
    {
        std::cerr << "Home Button Error in header." << std::endl;
        return BrowserAppError::home_error;
    }
    auto reload = home->get_next_sibling();
    if(!reload || reload->get_name() != "reload_button")
    {
        std::cerr << "Reload Button Error in header." << std::endl;
        return BrowserAppError::reload_error;
    }
    auto entry = reload->get_next_sibling();
    if(!entry || entry->get_name() != "header_entry")
    {
        std::cerr << "Entry Button Error in header." << std::endl;
        return BrowserAppError::entry_error;
    }
    auto enter = entry->get_next_sibling();
    if(!enter || enter->get_name() != "enter_button")
    {
        std::cerr << "Enter Button Error in header." << std::endl;
        return BrowserAppError::enter_error;
    }
    auto menu = enter->get_next_sibling();
    if(!menu || menu->get_name() != "menu_button")
    {
        std::cerr << "Menu Button Error in header." << std::endl;
        return BrowserAppError::menu_error;
    }
    return BrowserAppError::no_error;
}
