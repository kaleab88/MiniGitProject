#ifndef MINIGIT_H
#define MINIGIT_H

#include <string>
#include <vector>
#include <map>
#include <set>

class MiniGit
{
public:
    MiniGit();
    void init();
    void add(const std::string &filename);
    void commit(const std::string &message);
    void log();
    void branch(const std::string &branch_name);
    void checkout(const std::string &name);
    void merge(const std::string &branch_name);

private:
    const std::string MINIGIT_DIR = ".minigit";
    const std::string OBJECTS_DIR = ".minigit/objects";
    const std::string REFS_DIR = ".minigit/refs";
    const std::string HEAD_FILE = ".minigit/HEAD";
    const std::string INDEX_FILE = ".minigit/index";

    // Helper functions
    bool is_initialized();
    std::string get_head_ref_path();
    std::string get_head_commit_hash();
    std::map<std::string, std::string> get_staging_area();
    void set_staging_area(const std::map<std::string, std::string> &index);
    std::string get_commit_parent(const std::string &commit_hash);
    std::map<std::string, std::string> get_commit_files(const std::string &commit_hash);
    std::string find_lca(const std::string &commit1, const std::string &commit2);
};

#endif // MINIGIT_H