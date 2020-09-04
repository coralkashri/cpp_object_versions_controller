#include <iostream>
#include "../src/simple_example.h"

void simple_example_tests() {
    my_simple_collection src, copy;
    src.add_new_element(5);
    src.add_new_element(6);
    src.add_new_element(7);
    copy.apply_and_close_version(src.get_and_close_current_version());
    assert(src.to_string() == copy.to_string());
    src.add_new_element(8);
    src.add_new_element(9);
    src.add_new_element(10);
    src.add_new_element(11);
    src.add_new_element(12);
    src.add_new_element(13);
    copy.apply_and_close_version(src.get_and_close_current_version());
    assert(src.to_string() == copy.to_string());
    src.update_element(3, 80);
    src.add_new_element(14);
    src.add_new_element(15);
    copy.apply_and_close_version(src.get_and_close_current_version());
    assert(src.to_string() == copy.to_string());
    copy.apply_and_close_version(src.get_and_close_current_version()); // Nothing happens here
    copy.apply_and_close_version(src.get_and_close_current_version()); // Nothing happens here
    assert(src.to_string() == copy.to_string());
}

void advanced_example_tests() {

}

int main() {
    simple_example_tests();
    advanced_example_tests();

    return EXIT_SUCCESS;
}