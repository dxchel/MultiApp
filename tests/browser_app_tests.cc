#include "include/tests.hpp"


BrowserAppTester::BrowserAppTester() : browser (Gtk::manage(new Browser())) {};

BrowserAppError BrowserAppTester::browserAppStructureTests()
{
    auto browser {dynamic_cast<Gtk::Box *>(Gtk::manage(new Browser()))};
    if(!browser)
    {
        std::cerr << "Browser Box Error." << std::endl;
        return BrowserAppError::browser_error;
    }
    auto header {dynamic_cast<Gtk::Box *>(browser->get_first_child())};
    if(!header)
    {
        std::cerr << "Header Bar Error."  << std::endl;
        return BrowserAppError::header_error;
    }
    auto back {dynamic_cast<Gtk::Button *>(header->get_first_child())};
    if(!back)
    {
        std::cerr << "Back Button Error in header." << std::endl;
        return BrowserAppError::back_error;
    }
    auto forward {dynamic_cast<Gtk::Button *>(back->get_next_sibling())};
    if(!forward)
    {
        std::cerr << "Forward Button Error in header." << std::endl;
        return BrowserAppError::forward_error;
    }
    auto home {dynamic_cast<Gtk::Button *>(forward->get_next_sibling())};
    if(!home)
    {
        std::cerr << "Home Button Error in header." << std::endl;
        return BrowserAppError::home_error;
    }
    auto reload {dynamic_cast<Gtk::Button *>(home->get_next_sibling())};
    if(!reload)
    {
        std::cerr << "Reload Button Error in header." << std::endl;
        return BrowserAppError::reload_error;
    }
    auto entry {dynamic_cast<Gtk::Entry *>(reload->get_next_sibling())};
    if(!entry)
    {
        std::cerr << "Entry Button Error in header." << std::endl;
        return BrowserAppError::entry_error;
    }
    auto enter {dynamic_cast<Gtk::Button *>(entry->get_next_sibling())};
    if(!enter)
    {
        std::cerr << "Enter Button Error in header." << std::endl;
        return BrowserAppError::enter_error;
    }
    auto menu {dynamic_cast<Gtk::MenuButton *>(enter->get_next_sibling())};
    if(!menu)
    {
        std::cerr << "Menu Button Error in header." << std::endl;
        return BrowserAppError::menu_error;
    }
    return BrowserAppError::no_error;
}

BrowserAppError BrowserAppTester::browserAppFunctionalTests()
{
    // Browser Functionality tests placeholder
    return BrowserAppError::no_error;
}