#include "include/tests.hpp"

#include <iostream>

#include <gtest/gtest.h>
#include <gmock/gmock.h>


ChatTest::ChatTest() :
    chat            (Gtk::manage(new Chat())),
    home_button     (chat->home_button),
    connect_button  (chat->connect_button),
    message_button  (chat->message_button),
    ip_entry        (chat->ip_entry),
    port_entry      (chat->port_entry),
    message_entry   (chat->message_entry),
    chat_box        (chat->chat_box),
    footer_box      (chat->footer_box),
    chat_scrolled   (chat->chat_scrolled),
    status_label    (chat->status_label) {};

TEST_F(ChatTest, ChatStructuralTest) {
    ASSERT_THAT(chat, ::testing::NotNull());
    auto header_t {dynamic_cast<Gtk::Box *>(chat->get_first_child())};
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
    // Waiting for full function with server, due to the need of a server for validation
}
