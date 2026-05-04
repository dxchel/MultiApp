#include "gtkmm/dropdown.h"
#include "include/tests.hpp"

#include <iostream>

#include <gtest/gtest.h>
#include <gmock/gmock.h>


FractalTest::FractalTest() : fractal (Gtk::manage(new FractalBox())),
    menu{fractal->menu}, consts_box{fractal->consts_box},
    reset_button{fractal->reset_button}, save_button{fractal->save_button},
    fractal_dropdown{fractal->fractal_dropdown}, fractal_area{fractal->fractal_area},
    status_label{fractal->status_label}, iter_scale{fractal->iter_scale},
    const_r_scale{fractal->const_r_scale}, const_i_scale{fractal->const_i_scale},
    r_min_scale{fractal->r_min_scale}, r_max_scale{fractal->r_max_scale},
    g_min_scale{fractal->g_min_scale}, g_max_scale{fractal->g_max_scale},
    b_min_scale{fractal->b_min_scale}, b_max_scale{fractal->b_max_scale},
    selection{fractal->selection} {};

TEST_F(FractalTest, FractalStructuralTest)
{
    ASSERT_THAT(fractal, ::testing::NotNull());
    auto menu_t {dynamic_cast<Gtk::Box *>(fractal->get_first_child())};
    ASSERT_THAT(menu_t, ::testing::Eq(menu));
    auto fractal_area_t {dynamic_cast<FractalArea *>(menu_t->get_next_sibling())};
    ASSERT_THAT(fractal_area_t, ::testing::Eq(fractal_area));
    auto level {dynamic_cast<Gtk::Box *>(menu->get_first_child())};
    auto reset_t {dynamic_cast<Gtk::Button *>(level->get_first_child())};
    ASSERT_THAT(reset_t, ::testing::Eq(reset_button));
    auto save_t {dynamic_cast<Gtk::Button *>(reset_button->get_next_sibling())};
    ASSERT_THAT(save_t, ::testing::Eq(save_button));
    auto dropdown_t {dynamic_cast<Gtk::DropDown *>(save_button->get_next_sibling())};
    ASSERT_THAT(dropdown_t, ::testing::Eq(fractal_dropdown));
    level = dynamic_cast<Gtk::Box *>(level->get_next_sibling());
    auto iter_t {dynamic_cast<Gtk::Scale *>(level->get_first_child()->get_next_sibling())};
    ASSERT_THAT(iter_t, ::testing::Eq(iter_scale));
    level = dynamic_cast<Gtk::Box *>(level->get_next_sibling());
    auto r_min_t {dynamic_cast<Gtk::Scale *>(level->get_first_child()->get_next_sibling()->get_first_child())};
    ASSERT_THAT(r_min_t, ::testing::Eq(r_min_scale));
    auto r_max_t {dynamic_cast<Gtk::Scale *>(r_min_scale->get_next_sibling())};
    ASSERT_THAT(r_max_t, ::testing::Eq(r_max_scale));
    level = dynamic_cast<Gtk::Box *>(level->get_next_sibling());
    auto g_min_t {dynamic_cast<Gtk::Scale *>(level->get_first_child()->get_next_sibling()->get_first_child())};
    ASSERT_THAT(g_min_t, ::testing::Eq(g_min_scale));
    auto g_max_t {dynamic_cast<Gtk::Scale *>(g_min_scale->get_next_sibling())};
    ASSERT_THAT(g_max_t, ::testing::Eq(g_max_scale));
    level = dynamic_cast<Gtk::Box *>(level->get_next_sibling());
    auto b_min_t {dynamic_cast<Gtk::Scale *>(level->get_first_child()->get_next_sibling()->get_first_child())};
    ASSERT_THAT(b_min_t, ::testing::Eq(b_min_scale));
    auto b_max_t {dynamic_cast<Gtk::Scale *>(b_min_scale->get_next_sibling())};
    ASSERT_THAT(b_max_t, ::testing::Eq(b_max_scale));
    level = dynamic_cast<Gtk::Box *>(level->get_next_sibling());
    auto const_r_t {dynamic_cast<Gtk::Scale *>(level->get_first_child()->get_next_sibling()->get_first_child())};
    ASSERT_THAT(const_r_t, ::testing::Eq(const_r_scale));
    auto const_i_t {dynamic_cast<Gtk::Scale *>(const_r_scale->get_next_sibling())};
    ASSERT_THAT(const_i_t, ::testing::Eq(const_i_scale));
}

TEST_F(FractalTest, FractalFunctionalTest)
{
}