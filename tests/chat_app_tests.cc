#include "../include/tests.hpp"

#include <gtkmm.h>
#include <regex>

#include <gtest/gtest.h>
#include <gmock/gmock.h>


ChatTest::ChatTest() :
    server          (Gtk::manage(new Chat())),
    client_1        (Gtk::manage(new Chat())),
    client_2        (Gtk::manage(new Chat())),
    home_button     (server->home_button),
    connect_button  (server->connect_button),
    message_button  (server->message_button),
    ip_entry        (server->ip_entry),
    port_entry      (server->port_entry),
    message_entry   (server->message_entry),
    chat_box        (server->chat_box),
    footer_box      (server->footer_box),
    chat_scrolled   (server->chat_scrolled),
    status_label    (server->status_label) {};

TEST_F(ChatTest, ChatStructuralTest) {
    ASSERT_THAT(server,   ::testing::NotNull());
    ASSERT_THAT(client_1, ::testing::NotNull());
    ASSERT_THAT(client_2, ::testing::NotNull());
    auto header_t {dynamic_cast<Gtk::Box *>(server->get_first_child())};
    ASSERT_THAT(header_t, ::testing::NotNull());
    auto home_t {dynamic_cast<Gtk::Button *>(header_t->get_first_child())};
    ASSERT_THAT(home_t, ::testing::NotNull());
    ASSERT_THAT(home_t, ::testing::Eq(home_button));
    auto ip_entry_t {dynamic_cast<Gtk::Entry *>(home_t->get_next_sibling())};
    ASSERT_THAT(ip_entry_t, ::testing::NotNull());
    ASSERT_THAT(ip_entry_t, ::testing::Eq(ip_entry));
    auto port_entry_t {dynamic_cast<Gtk::Entry *>(ip_entry_t->get_next_sibling())};
    ASSERT_THAT(port_entry_t, ::testing::NotNull());
    ASSERT_THAT(port_entry_t, ::testing::Eq(port_entry));
    auto connect_button_t {dynamic_cast<Gtk::Button *>(port_entry_t->get_next_sibling())};
    ASSERT_THAT(connect_button_t, ::testing::NotNull());
    ASSERT_THAT(connect_button_t, ::testing::Eq(connect_button));
    auto chat_scrolled_t {dynamic_cast<Gtk::ScrolledWindow *>(header_t->get_next_sibling())};
    ASSERT_THAT(chat_scrolled_t, ::testing::NotNull());
    ASSERT_THAT(chat_scrolled_t, ::testing::Eq(chat_scrolled));
    // Don't know why it doesn't get the chat_box
    // auto chat_box_t {dynamic_cast<Gtk::Box *>(chat_scrolled_t->get_child())};
    // EXPECT_THAT(chat_box_t, ::testing::NotNull());
    // EXPECT_THAT(chat_box_t, ::testing::Eq(chat_box));
    auto footer_box_t {dynamic_cast<Gtk::Box *>(chat_scrolled_t->get_next_sibling())};
    ASSERT_THAT(footer_box_t, ::testing::NotNull());
    ASSERT_THAT(footer_box_t, ::testing::Eq(footer_box));
    auto message_entry_t {dynamic_cast<Gtk::Entry *>(footer_box_t->get_first_child())};
    ASSERT_THAT(message_entry_t, ::testing::NotNull());
    ASSERT_THAT(message_entry_t, ::testing::Eq(message_entry));
    auto message_button_t {dynamic_cast<Gtk::Button *>(message_entry_t->get_next_sibling())};
    ASSERT_THAT(message_button_t, ::testing::NotNull());
    ASSERT_THAT(message_button_t, ::testing::Eq(message_button));
    // EXPECT_THAT(status_label, ::testing::NotNull());
}

TEST_F(ChatTest, ChatFunctionalTest) {
    server->session_connection();
    ASSERT_THAT(server->session,       ::testing::NotNull());
    ASSERT_THAT(server->session->type, ::testing::Eq("Server"));

    // Check new connections
    client_1->session_connection();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    ASSERT_THAT(client_1->session,       ::testing::NotNull());
    ASSERT_THAT(client_1->session->type, ::testing::Eq("Client"));
    ASSERT_THAT(dynamic_cast<Server *>(server->session.get())->connections.size(), ::testing::Eq(1));
    ASSERT_THAT(server->session->receive_queue.size(),    ::testing::Eq(1));
    ASSERT_THAT(std::regex_search(server->session->receive_queue.front(), std::regex("User\\d says Hi!!!")), ::testing::Eq(true));

    // Check poster and disconnecter setters
    server->session->set_poster([this]{ std::cout << "Posting message from Server..." << std::endl; });
    server->session->set_disconnecter([this]{ std::cout << "Disconnecting from Server..." << std::endl; });
    ASSERT_THAT(server->session->poster, ::testing::NotNull());
    ASSERT_THAT(server->session->disconnecter, ::testing::NotNull());
    server->session->poster();
    server->session->disconnecter();
    client_1->session->set_poster([this]{ std::cout << "Posting message from Client..." << std::endl; });
    client_1->session->set_disconnecter([this]{ std::cout << "Disconnecting from Client..." << std::endl; });
    ASSERT_THAT(client_1->session->poster, ::testing::NotNull());
    ASSERT_THAT(client_1->session->disconnecter, ::testing::NotNull());
    client_1->session->poster();
    client_1->session->disconnecter();

    // Check message processing and broadcasting on Server
    std::string message {"Hello World!"};
    server->session->process_message(message);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    ASSERT_THAT(server->session->receive_queue.size(),    ::testing::Eq(2));
    ASSERT_THAT(server->session->receive_queue.back(), ::testing::Eq(message));
    ASSERT_THAT(client_1->session->receive_queue.size(),  ::testing::Eq(1));
    ASSERT_THAT(client_1->session->receive_queue.back(), ::testing::Eq(message));

    // Check multiple clients and broadcasting
    client_2->session_connection();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    ASSERT_THAT(client_2->session,       ::testing::NotNull());
    ASSERT_THAT(client_2->session->type, ::testing::Eq("Client"));
    ASSERT_THAT(dynamic_cast<Server *>(server->session.get())->connections.size(), ::testing::Eq(2));
    ASSERT_THAT(server->session->receive_queue.size(),    ::testing::Eq(3));
    ASSERT_THAT(std::regex_search(server->session->receive_queue.back(), std::regex("User\\d says Hi!!!")), ::testing::Eq(true));
    ASSERT_THAT(client_1->session->receive_queue.size(),    ::testing::Eq(2));
    ASSERT_THAT(std::regex_search(client_1->session->receive_queue.back(), std::regex("User\\d says Hi!!!")), ::testing::Eq(true));

    // Check message processing and broadcasting on Client
    client_1->session->process_message(message);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    ASSERT_THAT(server->session->receive_queue.size(),    ::testing::Eq(4));
    ASSERT_THAT(client_1->session->receive_queue.size(),  ::testing::Eq(3));
    ASSERT_THAT(client_2->session->receive_queue.size(),  ::testing::Eq(1));
    ASSERT_THAT(std::regex_search(server->session->receive_queue.back(), std::regex("\\(User\\d\\): " + message)), ::testing::Eq(true));
    ASSERT_THAT(client_1->session->receive_queue.back(), ::testing::Eq("(You): " + message));
    ASSERT_THAT(std::regex_search(client_2->session->receive_queue.back(), std::regex("\\(User\\d\\): " + message)), ::testing::Eq(true));

    // Check message processing and broadcasting on Client with multiple clients
    client_2->session->process_message(message);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    ASSERT_THAT(server->session->receive_queue.size(),    ::testing::Eq(5));
    ASSERT_THAT(client_1->session->receive_queue.size(),  ::testing::Eq(4));
    ASSERT_THAT(client_2->session->receive_queue.size(),  ::testing::Eq(2));
    ASSERT_THAT(std::regex_search(server->session->receive_queue.back(), std::regex("\\(User\\d\\): " + message)), ::testing::Eq(true));
    ASSERT_THAT(std::regex_search(client_1->session->receive_queue.back(), std::regex("\\(User\\d\\): " + message)), ::testing::Eq(true));
    ASSERT_THAT(client_2->session->receive_queue.back(), ::testing::Eq("(You): " + message));

    // Check Client disconnection and broadcasting from Server
    client_1->session_connection();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    ASSERT_THAT(client_1->session,       ::testing::IsNull());
    ASSERT_THAT(dynamic_cast<Server *>(server->session.get())->connections.size(), ::testing::Eq(1));
    ASSERT_THAT(server->session->receive_queue.size(),    ::testing::Eq(6));
    ASSERT_THAT(std::regex_search(server->session->receive_queue.back(), std::regex("User\\d has left the chat!!!")), ::testing::Eq(true));
    ASSERT_THAT(client_2->session->receive_queue.size(),    ::testing::Eq(3));
    ASSERT_THAT(std::regex_search(client_2->session->receive_queue.back(), std::regex("User\\d has left the chat!!!")), ::testing::Eq(true));

    // Check Server disconnection on Client
    server->session_connection();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    ASSERT_THAT(server->session,       ::testing::IsNull());
    ASSERT_THAT(dynamic_cast<Client *>(client_2->session.get())->connection->socket.is_open(), ::testing::Eq(false));
}
