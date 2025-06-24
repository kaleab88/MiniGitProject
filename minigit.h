#ifndef MINIGIT_H
#define MINIGIT_H

#include <string>
#include <vector>
#include <map>
#include <set>          // Added from the incoming version - necessary for set operations in minigit.cpp
#include <ctime>        // From HEAD - For std::time_t
#include <filesystem>   // From HEAD - For std::filesystem::path

// Structure to represent a commit object
// This is from your HEAD version and is crucial.
struct Commit {
    std::string hash;
    std::string parent_hash;
    std::string second_parent_hash; // For merge commits
    std::string message;
    std::string author;
    std::time_t timestamp;
    std::map<std::string, std::string> snapshot; // Maps filepath to blob_hash

    // Default constructor to initialize members
    Commit() : timestamp(0) {}
};

class MiniGit {
public:
    MiniGit();
    ~MiniGit(); // From HEAD

    void init();
    void add(const std::string& filepath); // Using 'filepath' from HEAD as it's more descriptive
    void commit(const std::string& message);
    void log();
    void branch(const std::string& branch_name);
    void checkout(const std::string& branch_name_or_commit_hash); // Using 'branch_name_or_commit_hash' from HEAD
    void merge(const std::string& branch_name);

private:
    // These are from HEAD and are consistent with minigit.cpp's usage.
    std::filesystem::path repo_path;
    std::filesystem::path objects_path;
    std::filesystem::path refs_path;
    std::filesystem::path head_path;
    std::filesystem::path index_path; // Staging area

    // All these helper function declarations are from HEAD and align with minigit.cpp
    std::map<std::string, std::string> read_index();
    void write_index(const std::map<std::string, std::string>& index_map);
    std::string create_blob(const std::string& filepath);
    std::string get_head_commit_hash();
    void update_head(const std::string& commit_hash, bool is_branch, const std::string& branch_name = "");

    // Commit related functions
    Commit get_commit(const std::string& commit_hash);
    Commit parse_commit_data(const std::string& commit_hash, const std::string& commit_data);
    std::string serialize_commit_data(const Commit& commit_obj);
    void commit(const std::string& hash, const std::string& parent1_hash, const std::string& parent2_hash, const std::map<std::string, std::string>& snapshot_map);

    // File content from blob hash
    std::string get_file_content_from_blob_hash(const std::string& blob_hash);

    // Merge related functions
    bool is_ancestor(const std::string& ancestor_hash, const std::string& descendant_hash);
    std::string find_lca(const std::string& commit1_hash, const std::string& commit2_hash);
    std::map<std::string, std::string> apply_merge_changes(
        const std::map<std::string, std::string>& current_snapshot,
        const std::map<std::string, std::string>& other_snapshot,
        const std::map<std::string, std::string>& lca_snapshot,
        bool& conflicts_occurred
    );
    void write_file_with_conflict_markers(const std::string& filepath, const std::string& current_content, const std::string& other_content, const std::string& lca_content);
};

#endif // MINIGIT_H