#ifndef CPP_OBJECT_VERSION_CONTROLLER_SIMPLE_EXAMPLE_H
#define CPP_OBJECT_VERSION_CONTROLLER_SIMPLE_EXAMPLE_H

#include <iostream>
#include <string>
#include <sstream>
#include "versions_controller.h"

/**
 * A collection patch
 */
struct my_simple_patch : patch_interface {
    class new_element {
    public:
        explicit new_element(int val) : val(val) {}
        [[nodiscard]] int get() const { return val; }

    private:
        int val;
    };

    class update_element {
    public:
        explicit update_element(size_t relative_element_idx, int val) : relative_element_idx(relative_element_idx), val(val) {}
        [[nodiscard]] size_t get_relative_element_idx() const { return relative_element_idx; }
        [[nodiscard]] int get_val() const { return val; }

    private:
        size_t relative_element_idx;
        int val;
    };

    explicit my_simple_patch(new_element patch_new_element) : val(patch_new_element.get()), relative_element_idx(-1) {};
    explicit my_simple_patch(update_element patch_update_element) : val(patch_update_element.get_val()), relative_element_idx(patch_update_element.get_relative_element_idx()) {};
    my_simple_patch(const my_simple_patch&) = default;
    my_simple_patch(my_simple_patch&&) = default;
    my_simple_patch& operator=(const my_simple_patch&) = default;
    my_simple_patch& operator=(my_simple_patch&&) = default;

    [[nodiscard]] int get_relative_element_idx() const { return relative_element_idx; }
    [[nodiscard]] int get_val() const { return val; }

private:
    int relative_element_idx;
    int val;
};

class my_simple_object {
public:
    explicit my_simple_object(int val) : val(val) {}

    void set_val(int new_val) { val = new_val; }
    [[nodiscard]] int get_val() const { return val; }

private:
    int val;
};

/**
 * A collection of my_simple_object
 */
class my_simple_collection : public version_collection_interface<my_simple_patch> {
public:
    void add_new_element(int val) {
        vec.emplace_back(val);
        history.create_new_patch(my_simple_patch(my_simple_patch::new_element(val)));
    }

    void update_element(size_t index, int val) {
        vec.at(index).set_val(val);
        history.create_new_patch(my_simple_patch(my_simple_patch::update_element(index, val)));
    }

    void apply_version(const version<my_simple_patch> &ver) override {
        auto patches = ver.clone_new_patches(history.get_current_patch_number());
        for (auto &patch : patches) {
            if (auto index = patch.get_relative_element_idx(); index == -1) {
                add_new_element(patch.get_val());
            } else {
                update_element(index, patch.get_val());
            }
        }
    }

    std::string to_string() {
        std::stringstream s;
        for (auto &elem : vec) {
            s << elem.get_val() << " ";
        }
        return s.str();
    }

private:
    std::vector<my_simple_object> vec;
};

#endif //CPP_OBJECT_VERSION_CONTROLLER_SIMPLE_EXAMPLE_H