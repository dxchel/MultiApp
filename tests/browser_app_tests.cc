#include "include/tests.hpp"


BrowserAppError browserAppTests()
{
    auto browser = Gtk::manage(new Browser());
    if(!browser)
    {
        std::cerr << "Browser Box couldn't be created." << std::endl;
        return BrowserAppError::browser_missing;
    }
    return BrowserAppError::no_error;
}
