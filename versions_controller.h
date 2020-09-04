#ifndef CPP_OBJECT_VERSION_CONTROLLER_VERSIONS_CONTROLLER_H
#define CPP_OBJECT_VERSION_CONTROLLER_VERSIONS_CONTROLLER_H

#include <type_traits>
#include <vector>
#include <list>
#include <shared_mutex>
#include <mutex>
#include <assert.h>

struct patch_interface {

};

template <typename T>
concept PatchInterface = std::is_base_of_v<patch_interface, T>;

template <PatchInterface PI>
struct version {
public:
    explicit version(size_t patch_number) : patch_number(patch_number) {};

    version(const version<PI> &ref) : patch_number(ref.patch_number), patches(ref.clone_all_patches()) {};

    version(version<PI> &&ref) noexcept : patch_number(ref.patch_number), patches(ref.clone_all_patches()) {};

    /**
     * Create a new patch to this version.
     * @param patch - The desired patch.
     */
    void create_new_patch(PI patch) {
        std::lock_guard g(guarder);
        patches.emplace_back(patch);
        patch_number++;
    };

    /**
     * @param last_patch_number - Desired patch number to start clone from.
     *
     * @return A copy vector of this version's patches, started from { last_patch_number - (patch_number - patches.size()) }.
     */
    std::vector<PI> clone_new_patches(size_t last_patch_number) const {
        std::lock_guard g(guarder);
        if (last_patch_number - (patch_number - patches.size()) < 0 || last_patch_number > patch_number) {
            throw std::runtime_error("Requested patch is illegal.");
        }
        return std::vector(patches.begin() + last_patch_number - (patch_number - patches.size()), patches.end());
    }

    /**
     * @return All of this version's patches.
     */
    std::vector<PI> clone_all_patches() const {
        std::lock_guard g(guarder);
        return patches;
    }

private:
    mutable std::mutex guarder;
    std::vector<PI> patches;
    size_t patch_number;
};

template <PatchInterface PI>
struct versions_controller {
public:
    explicit versions_controller(size_t max_versions_count = 0) : max_versions_count(max_versions_count), last_version_status(CLOSE), patch_number(0) {}

    /**
     * @return The last version (copy).
     */
    version<PI> get_last_version() { std::shared_lock g(guarder); return find_version(versions.size() - 1); }

    /**
     * @param number - Target version number.
     * @return A target version copy.
     */
    version<PI> get_version(size_t number) { std::shared_lock g(guarder); return find_version(number); }

    /**.
     * If the current version is open - create a new patch in the current version.
     * Otherwise create a new version with the desired patch.
     * @param patch - The desired patch.
     */
    void create_new_patch(PI patch) {
        std::unique_lock g(guarder);
        if (last_version_status == CLOSE) {
            versions.emplace_back(patch_number); // Create a new version
            last_version_status = OPEN;
        }
        find_version(versions.size() - 1).create_new_patch(patch);
        patch_number++;
    }

    /**
     * Close the current version for new patches
     */
    void close_version() {
        std::shared_lock g(guarder);
        last_version_status = CLOSE;
    }

    /**
     * Remove all versions from this controller.
     */
    void clear_versions() { std::unique_lock g(guarder); versions.clear(); }

    size_t get_current_patch_number() { std::shared_lock g(guarder); return patch_number; }

private:
    std::shared_mutex guarder;
    std::list<version<PI>> versions;
    size_t max_versions_count;
    size_t patch_number;

    enum version_status : bool {
        CLOSE = 0,
        OPEN = 1
    } last_version_status;

    version<PI>& find_version(size_t number) {
        auto ver_it = versions.begin();
        std::advance(ver_it, number);
        return *ver_it;
    }
};

template <PatchInterface PI>
struct version_collection_interface {
public:
    virtual void apply_version(const version<PI> &ver) = 0;

    version<PI> get_current_version() { return history.get_last_version(); }

    version<PI> get_and_close_current_version() {
        auto current_version = history.get_last_version();
        history.close_version();
        return current_version;
    }

    void close_version() { history.close_version(); }

    void apply_and_close_version(const version<PI> &ver) { apply_version(ver); close_version(); }

protected:
    versions_controller<PI> history;
};

#endif //CPP_OBJECT_VERSION_CONTROLLER_VERSIONS_CONTROLLER_H