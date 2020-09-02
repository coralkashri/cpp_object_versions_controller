#include <iostream>
#include "versions_controller.h"

struct patch : patch_interface {
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

    explicit patch(new_element patch_new_element) : val(patch_new_element.get()), relative_element_idx(-1) {};
    explicit patch(update_element patch_update_element) : val(patch_update_element.get_val()), relative_element_idx(patch_update_element.get_relative_element_idx()) {};
    patch(const patch&) = default;
    patch(patch&&) = default;
    patch& operator=(const patch&) = default;
    patch& operator=(patch&&) = default;

    [[nodiscard]] int get_relative_element_idx() const { return relative_element_idx; }
    [[nodiscard]] int get_val() const { return val; }

private:
    int relative_element_idx;
    int val;
};

int main() {
    std::cout << "Hello, World!" << std::endl;
    return 0;
}