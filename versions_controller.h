#ifndef CPP_OBJECT_VERSION_CONTROLLER_VERSIONS_CONTROLLER_H
#define CPP_OBJECT_VERSION_CONTROLLER_VERSIONS_CONTROLLER_H

#include <type_traits>
#include <vector>
#include <list>
#include <shared_mutex>
#include <mutex>

struct patch_interface {

};

template <typename T>
concept PatchInterface = std::is_base_of_v<patch_interface, T>;

template <PatchInterface PI>
struct version {
public:
    void create_new_patch(PI patch) {
        patches.emplace_back(patch);
    };

    std::vector<PI> clone_patches() const {
        return patches;
    }

private:
    std::vector<PI> patches;
};

template <PatchInterface PI>
struct versions_controller {
public:
    version<PI>& get_last_version() const { std::shared_lock g(guarder); return get_version(versions.size() - 1); }
    version<PI>& get_version(std::size_t number) const { std::shared_lock g(guarder); return std::advance(versions.begin(), number); }
    void create_new_version() { std::unique_lock g(guarder); versions.emplace_back(); }

private:
    std::shared_mutex guarder;
    std::list<version<PI>> versions;
};

#endif //CPP_OBJECT_VERSION_CONTROLLER_VERSIONS_CONTROLLER_H