/*!
* \file
* \brief file main.cpp
*
* Unit tests main file
* At the moment this is using google tests
*
* Copyright of Timo Hannukkala, All rights reserved.
*
* \author Timo Hannukkala <timohannukkala@hotmail.com>
*/

#include "gtest/gtest.h"
#include "tst_glyph_filler.h"
#include "tst_glyph_drawer.h"
#include "tst_test_loader.h"
#include "tst_glyph_image_positions.h"
#include "tst_glyph_graph_generator.h"
#include "tst_parse_text.h"

TEST(ParseFont, Test) {
    EXPECT_EQ(tst_parse_text_generate_list_characters(), 0);
}

TEST(GlyphGraphGenerator, Test) {
    EXPECT_EQ(tst_glyph_graph_generator_min_value(), 0);
    EXPECT_EQ(tst_glyph_graph_generator_max_value(), 0);
}


TEST(GlyphImage, Test) {
    EXPECT_EQ(tst_glyph_image_positions_generate_glyph_positions(), 0);
}

TEST(GlyphDrawer, Test) {
    EXPECT_EQ(tst_fillInnerAreaInImageFiles(), 0);
}

TEST(TestLoader, Test) {
    EXPECT_EQ(tst_test_loader_rotate(), 0);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

