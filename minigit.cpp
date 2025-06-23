#include "minigit.h"
#include "utils.h" // For Utils::readFile, Utils::writeFile, Utils::createDirectory, Utils::sha1
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>  // For current timestamp, although time_t is sufficient
#include <iomanip> // For std::put_time, std::get_time, std::hex, std::setw, std::setfill
#include <filesystem>
#include <algorithm> // For std::set_union, std::max
#include <set>       // For find_lca's ancestor history
#include <queue>     // For std::queue in find_lca

namespace fs = std::filesystem;

// Constructor
MiniGit::MiniGit()
{
    repo_path = fs::current_path();
    objects_path = repo_path / ".minigit" / "objects";
    refs_path = repo_path / ".minigit" / "refs";
    head_path = repo_path / ".minigit" / "HEAD";
    index_path = repo_path / ".minigit" / "index"; // Staging area
}

MiniGit::~MiniGit()
{
    // Destructor (nothing specific needed for this example)
}

void MiniGit::init()
{
    fs::create_directories(objects_path);        // Stores blobs and commit objects
    fs::create_directories(refs_path / "heads"); // Stores branch pointers

    Utils::writeFile(head_path.string(), "ref: refs/heads/main");
    Utils::writeFile(index_path.string(), ""); // Initialize empty index file

    std::cout << "Initialized empty MiniGit repository in " << (repo_path / ".minigit").string() << std::endl;
}

std::map<std::string, std::string> MiniGit::read_index()
{
    std::map<std::string, std::string> index_map;
    std::string content = Utils::readFile(index_path.string());
    std::stringstream ss(content);
    std::string line;
    while (std::getline(ss, line))
    {
        size_t first_space = line.find(' ');
        if (first_space != std::string::npos)
        {
            std::string filepath = line.substr(0, first_space);
            std::string blob_hash = line.substr(first_space + 1);
            index_map[filepath] = blob_hash;
        }
    }
    return index_map;
}

void MiniGit::write_index(const std::map<std::string, std::string> &index_map)
{
    std::stringstream ss;
    for (const auto &pair : index_map)
    {
        ss << pair.first << " " << pair.second << "\n";
    }
    Utils::writeFile(index_path.string(), ss.str());
}

std::string MiniGit::create_blob(const std::string &filepath)
{
    std::string content = Utils::readFile(filepath);
    if (content.empty() && !fs::exists(filepath))
    {
        std::cerr << "Error: File not found or empty: " << filepath << std::endl;
        return "";
    }

    std::string blob_hash = Utils::sha1(content);

    fs::path blob_path = objects_path / blob_hash;
    Utils::writeFile(blob_path.string(), content);

    std::cout << "Blob created for " << filepath << " with hash " << blob_hash << std::endl;
    return blob_hash;
}

void MiniGit::add(const std::string &filepath)
{
    if (!fs::exists(filepath) || fs::is_directory(filepath))
    {
        std::cerr << "Error: Cannot add '" << filepath << "'. File does not exist or is a directory." << std::endl;
        return;
    }

    std::string blob_hash = create_blob(filepath);
    if (blob_hash.empty())
    {
        std::cerr << "Failed to create blob for " << filepath << std::endl;
        return;
    }

    std::map<std::string, std::string> index_map = read_index();
    index_map[filepath] = blob_hash;
    write_index(index_map);

    std::cout << "Added " << filepath << " to staging area." << std::endl;
}

std::string MiniGit::get_head_commit_hash()
{
    std::string head_content = Utils::readFile(head_path.string());
    if (head_content.empty())
    {
        return "";
    }

    if (head_content.rfind("ref: ", 0) == 0)
    {
        std::string ref_path = head_content.substr(5);
        if (!fs::exists(repo_path / ".minigit" / ref_path))
        {
            return "";
        }
        return Utils::readFile((repo_path / ".minigit" / ref_path).string());
    }
    else
    {
        return head_content;
    }
}

void MiniGit::update_head(const std::string &commit_hash, bool is_branch, const std::string &branch_name)
{
    if (is_branch)
    {
        fs::path branch_file_path = refs_path / "heads" / branch_name;
        Utils::writeFile(branch_file_path.string(), commit_hash);
        Utils::writeFile(head_path.string(), "ref: " + (fs::path("refs") / "heads" / branch_name).string());
    }
    else
    {
        Utils::writeFile(head_path.string(), commit_hash);
    }
}

void MiniGit::commit(const std::string &hash, const std::string &parent1_hash, const std::string &parent2_hash, const std::map<std::string, std::string> &snapshot_map)
{
    Commit new_commit_obj;
    new_commit_obj.hash = hash;
    new_commit_obj.parent_hash = parent1_hash;
    new_commit_obj.second_parent_hash = parent2_hash;
    new_commit_obj.message = "Internal commit (merge/checkout)";
    new_commit_obj.author = "MiniGit";
    new_commit_obj.timestamp = std::time(nullptr);
    new_commit_obj.snapshot = snapshot_map;

    std::string commit_data = serialize_commit_data(new_commit_obj);
    Utils::writeFile((objects_path / new_commit_obj.hash).string(), commit_data);

    std::string head_content = Utils::readFile(head_path.string());
    if (head_content.rfind("ref: ", 0) == 0)
    {
        std::string current_branch_ref_path = head_content.substr(5);
        std::string branch_name = fs::path(current_branch_ref_path).filename().string();
        update_head(new_commit_obj.hash, true, branch_name);
    }
    else
    {
        update_head(new_commit_obj.hash, false);
    }
}

void MiniGit::commit(const std::string &message)
{
    std::map<std::string, std::string> current_snapshot = read_index();

    if (current_snapshot.empty())
    {
        std::cout << "Nothing to commit, working tree clean. (Staging area is empty)" << std::endl;
        return;
    }

    std::string parent_hash = get_head_commit_hash();

    Commit new_commit_obj;
    new_commit_obj.parent_hash = parent_hash;
    new_commit_obj.second_parent_hash = "";
    new_commit_obj.message = message;
    new_commit_obj.author = "default_user";
    new_commit_obj.timestamp = std::time(nullptr);
    new_commit_obj.snapshot = current_snapshot;

    new_commit_obj.hash = Utils::sha1(serialize_commit_data(new_commit_obj));

    fs::path commit_obj_path = objects_path / new_commit_obj.hash;
    Utils::writeFile(commit_obj_path.string(), serialize_commit_data(new_commit_obj));

    std::string head_content = Utils::readFile(head_path.string());
    if (head_content.rfind("ref: ", 0) == 0)
    {
        std::string current_branch_ref_path = head_content.substr(5);
        std::string branch_name = fs::path(current_branch_ref_path).filename().string();
        update_head(new_commit_obj.hash, true, branch_name);
        std::cout << "[" << branch_name << " " << new_commit_obj.hash.substr(0, 7) << "] " << message << std::endl;
    }
    else
    {
        update_head(new_commit_obj.hash, false);
        std::cout << "[detached HEAD " << new_commit_obj.hash.substr(0, 7) << "] " << message << std::endl;
    }

    write_index({});
    std::cout << "Committed successfully." << std::endl;
}

void MiniGit::log()
{
    std::string current_commit_hash = get_head_commit_hash();
    if (current_commit_hash.empty())
    {
        std::cout << "No commits yet." << std::endl;
        return;
    }

    std::cout << "Commit history:" << std::endl;
    while (!current_commit_hash.empty())
    {
        Commit c_obj = get_commit(current_commit_hash);
        if (c_obj.hash.empty())
        {
            break;
        }

        std::cout << "\ncommit " << c_obj.hash << std::endl;
        if (!c_obj.second_parent_hash.empty())
        {
            std::cout << "Merge: " << c_obj.parent_hash.substr(0, 7) << " " << c_obj.second_parent_hash.substr(0, 7) << std::endl;
        }
        std::cout << "Author: " << c_obj.author << std::endl;
        std::cout << "Date: " << std::asctime(std::localtime(&c_obj.timestamp));
        std::cout << "\n    " << c_obj.message << std::endl;

        current_commit_hash = c_obj.parent_hash;
    }
}

void MiniGit::branch(const std::string &branch_name)
{
    if (branch_name.empty())
    {
        std::cerr << "Error: Branch name cannot be empty." << std::endl;
        return;
    }

    fs::path branch_file_path = refs_path / "heads" / branch_name;
    if (fs::exists(branch_file_path))
    {
        std::cout << "Branch '" << branch_name << "' already exists." << std::endl;
        return;
    }

    std::string current_commit_hash = get_head_commit_hash();
    if (current_commit_hash.empty())
    {
        std::cerr << "Error: Cannot create branch. No commits yet." << std::endl;
        return;
    }

    Utils::writeFile(branch_file_path.string(), current_commit_hash);
    std::cout << "Branch '" << branch_name << "' created at " << current_commit_hash.substr(0, 7) << std::endl;
}

void MiniGit::checkout(const std::string &branch_name_or_commit_hash)
{
    std::string target_commit_hash;
    std::string resolved_ref_name;

    fs::path branch_path = refs_path / "heads" / branch_name_or_commit_hash;
    if (fs::exists(branch_path))
    {
        target_commit_hash = Utils::readFile(branch_path.string());
        resolved_ref_name = "ref: " + (fs::path("refs") / "heads" / branch_name_or_commit_hash).string();
        std::cout << "Switching to branch '" << branch_name_or_commit_hash << "'" << std::endl;
    }
    else
    {
        fs::path commit_obj_path = objects_path / branch_name_or_commit_hash;
        if (!fs::exists(commit_obj_path))
        {
            std::cerr << "Error: Reference '" << branch_name_or_commit_hash << "' not found. Not a branch or a valid commit hash." << std::endl;
            return;
        }
        target_commit_hash = branch_name_or_commit_hash;
        resolved_ref_name = target_commit_hash;
        std::cout << "Note: switching to 'detached HEAD' state." << std::endl;
    }

    Commit target_commit = get_commit(target_commit_hash);
    if (target_commit.hash.empty())
    {
        std::cerr << "Error: Could not retrieve commit object for " << target_commit_hash << std::endl;
        return;
    }

    // --- Start of FIX for checkout deleting source files (included in this complete version) ---
    std::set<std::string> protected_files = {
        ".minigit",                                                     // The repository itself
        "main.cpp", "minigit.cpp", "minigit.h", "utils.cpp", "utils.h", // Source files
        "Makefile",                                                     // Build script
        "minigit"                                                       // The executable itself
    };

    // First, iterate through the working directory and remove files/directories
    // that are NOT protected and are NOT part of the new commit's snapshot.
    // This handles deletions from the new commit.
    for (const auto &entry : fs::directory_iterator(repo_path))
    {
        std::string filename = entry.path().filename().string();
        // If the entry is a protected file/directory, skip it.
        if (protected_files.count(filename))
        {
            continue;
        }

        // If it's a file that is not in the target snapshot (meaning it's removed by checkout)
        // or if it's a directory (we assume only tracked files are left as files, others removed)
        if (!target_commit.snapshot.count(filename) || fs::is_directory(entry.path()))
        {
            fs::remove_all(entry.path()); // Remove if not in snapshot or if it's a directory
        }
    }
    // --- End of FIX ---

    // 4. Write files from target commit's snapshot to working directory
    // This will overwrite existing files or create new ones from the snapshot.
    for (const auto &pair : target_commit.snapshot)
    {
        const std::string &filepath = pair.first;
        const std::string &blob_hash = pair.second;
        std::string content = get_file_content_from_blob_hash(blob_hash);
        if (!content.empty())
        {
            Utils::writeFile(repo_path / filepath, content);
        }
        else
        {
            // Ensure file is created empty if blob content is empty (e.g., for empty files)
            Utils::writeFile(repo_path / filepath, "");
            std::cerr << "Warning: Could not fully restore file " << filepath << " (blob " << blob_hash << ") or it was empty." << std::endl;
        }
    }

    // 5. Update HEAD and index
    Utils::writeFile(head_path.string(), resolved_ref_name);
    write_index(target_commit.snapshot);

    std::cout << "HEAD is now at " << target_commit_hash.substr(0, 7) << std::endl;
}

Commit MiniGit::get_commit(const std::string &commit_hash)
{
    fs::path commit_obj_path = objects_path / commit_hash;
    std::string commit_data = Utils::readFile(commit_obj_path.string());
    if (commit_data.empty())
    {
        return Commit();
    }
    return parse_commit_data(commit_hash, commit_data);
}

Commit MiniGit::parse_commit_data(const std::string &commit_hash, const std::string &commit_data)
{
    Commit c_obj;
    c_obj.hash = commit_hash;
    std::stringstream ss(commit_data);
    std::string line;
    std::map<std::string, std::string> snapshot_map;
    bool in_snapshot_section = false;

    while (std::getline(ss, line))
    {
        if (line.rfind("parent: ", 0) == 0)
        {
            c_obj.parent_hash = line.substr(8);
        }
        else if (line.rfind("parent2: ", 0) == 0)
        {
            c_obj.second_parent_hash = line.substr(9);
        }
        else if (line.rfind("message: ", 0) == 0)
        {
            c_obj.message = line.substr(9);
        }
        else if (line.rfind("author: ", 0) == 0)
        {
            c_obj.author = line.substr(8);
        }
        else if (line.rfind("timestamp: ", 0) == 0)
        {
            c_obj.timestamp = static_cast<std::time_t>(std::stoll(line.substr(11)));
        }
        else if (line == "---snapshot---")
        {
            in_snapshot_section = true;
        }
        else if (in_snapshot_section)
        {
            size_t first_space = line.find(' ');
            if (first_space != std::string::npos)
            {
                std::string filepath = line.substr(0, first_space);
                std::string blob_hash = line.substr(first_space + 1);
                snapshot_map[filepath] = blob_hash;
            }
        }
    }
    c_obj.snapshot = snapshot_map;
    return c_obj;
}

std::string MiniGit::serialize_commit_data(const Commit &commit_obj)
{
    std::stringstream ss;
    ss << "parent: " << commit_obj.parent_hash << "\n";
    if (!commit_obj.second_parent_hash.empty())
    {
        ss << "parent2: " << commit_obj.second_parent_hash << "\n";
    }
    ss << "message: " << commit_obj.message << "\n";
    ss << "author: " << commit_obj.author << "\n";
    ss << "timestamp: " << commit_obj.timestamp << "\n";
    ss << "---snapshot---\n";
    for (const auto &pair : commit_obj.snapshot)
    {
        ss << pair.first << " " << pair.second << "\n";
    }
    return ss.str();
}

std::string MiniGit::get_file_content_from_blob_hash(const std::string &blob_hash)
{
    fs::path blob_path = objects_path / blob_hash;
    return Utils::readFile(blob_path.string());
}

bool MiniGit::is_ancestor(const std::string &ancestor_hash, const std::string &descendant_hash)
{
    if (ancestor_hash.empty())
        return true;
    if (descendant_hash.empty())
        return false;

    if (ancestor_hash == descendant_hash)
        return true;

    std::queue<std::string> q;
    std::set<std::string> visited;
    q.push(descendant_hash);
    visited.insert(descendant_hash);

    while (!q.empty())
    {
        std::string current = q.front();
        q.pop();

        Commit current_commit = get_commit(current);
        if (current_commit.hash.empty())
            continue;

        if (!current_commit.parent_hash.empty() && visited.find(current_commit.parent_hash) == visited.end())
        {
            if (current_commit.parent_hash == ancestor_hash)
                return true;
            q.push(current_commit.parent_hash);
            visited.insert(current_commit.parent_hash);
        }
        if (!current_commit.second_parent_hash.empty() && visited.find(current_commit.second_parent_hash) == visited.end())
        {
            if (current_commit.second_parent_hash == ancestor_hash)
                return true;
            q.push(current_commit.second_parent_hash);
            visited.insert(current_commit.second_parent_hash);
        }
    }
    return false;
}

std::string MiniGit::find_lca(const std::string &commit1_hash, const std::string &commit2_hash)
{
    if (commit1_hash.empty() || commit2_hash.empty())
    {
        return "";
    }
    if (commit1_hash == commit2_hash)
        return commit1_hash;

    std::queue<std::string> q1;
    std::set<std::string> ancestors1;
    q1.push(commit1_hash);
    ancestors1.insert(commit1_hash);

    while (!q1.empty())
    {
        std::string current_hash = q1.front();
        q1.pop();

        Commit current_commit = get_commit(current_hash);
        if (current_commit.hash.empty())
            continue;

        if (!current_commit.parent_hash.empty() && ancestors1.find(current_commit.parent_hash) == ancestors1.end())
        {
            q1.push(current_commit.parent_hash);
            ancestors1.insert(current_commit.parent_hash);
        }
        if (!current_commit.second_parent_hash.empty() && ancestors1.find(current_commit.second_parent_hash) == ancestors1.end())
        {
            q1.push(current_commit.second_parent_hash);
            ancestors1.insert(current_commit.second_parent_hash);
        }
    }

    std::queue<std::string> q2;
    std::set<std::string> visited2;
    q2.push(commit2_hash);
    visited2.insert(commit2_hash);

    std::vector<std::string> lca_candidates;

    while (!q2.empty())
    {
        std::string current_hash = q2.front();
        q2.pop();

        if (ancestors1.count(current_hash))
        {
            lca_candidates.push_back(current_hash);
        }

        Commit current_commit = get_commit(current_hash);
        if (current_commit.hash.empty())
            continue;

        if (!current_commit.parent_hash.empty() && visited2.find(current_commit.parent_hash) == visited2.end())
        {
            q2.push(current_commit.parent_hash);
            visited2.insert(current_commit.parent_hash);
        }
        if (!current_commit.second_parent_hash.empty() && visited2.find(current_commit.second_parent_hash) == visited2.end())
        {
            q2.push(current_commit.second_parent_hash);
            visited2.insert(current_commit.second_parent_hash);
        }
    }

    if (lca_candidates.empty())
    {
        return "";
    }

    std::string best_lca = "";
    time_t latest_timestamp = 0;

    for (const std::string &lca_candidate_hash : lca_candidates)
    {
        Commit lca_candidate_commit = get_commit(lca_candidate_hash);
        if (lca_candidate_commit.hash.empty())
            continue;

        if (lca_candidate_commit.timestamp > latest_timestamp)
        {
            latest_timestamp = lca_candidate_commit.timestamp;
            best_lca = lca_candidate_hash;
        }
    }

    return best_lca;
}

std::map<std::string, std::string> MiniGit::apply_merge_changes(
    const std::map<std::string, std::string> &current_snapshot,
    const std::map<std::string, std::string> &other_snapshot,
    const std::map<std::string, std::string> &lca_snapshot,
    bool &conflicts_occurred)
{
    conflicts_occurred = false;
    std::map<std::string, std::string> merged_snapshot = current_snapshot;

    std::set<std::string> all_filepaths;
    for (const auto &pair : current_snapshot)
        all_filepaths.insert(pair.first);
    for (const auto &pair : other_snapshot)
        all_filepaths.insert(pair.first);
    for (const auto &pair : lca_snapshot)
        all_filepaths.insert(pair.first);

    for (const std::string &filepath : all_filepaths)
    {
        std::string current_blob = current_snapshot.count(filepath) ? current_snapshot.at(filepath) : "";
        std::string other_blob = other_snapshot.count(filepath) ? other_snapshot.at(filepath) : "";
        std::string lca_blob = lca_snapshot.count(filepath) ? lca_snapshot.at(filepath) : "";

        if (lca_blob.empty() && current_blob.empty() && !other_blob.empty())
        { // File added in other branch
            merged_snapshot[filepath] = other_blob;
            std::cout << "Added file: " << filepath << std::endl;
        }
        else if (!lca_blob.empty() && current_blob.empty() && !other_blob.empty())
        { // File deleted in current, modified in other (conflict)
            std::cout << "CONFLICT (delete/modify): " << filepath << " deleted in current, modified in other." << std::endl;
            std::string current_content = "";
            std::string other_content = get_file_content_from_blob_hash(other_blob);
            std::string lca_content = get_file_content_from_blob_hash(lca_blob);
            write_file_with_conflict_markers(filepath, current_content, other_content, lca_content);
            conflicts_occurred = true;
            merged_snapshot[filepath] = ""; // Mark as conflicted
        }
        else if (!lca_blob.empty() && !current_blob.empty() && other_blob.empty())
        { // File deleted in other, modified in current (conflict)
            std::cout << "CONFLICT (modify/delete): " << filepath << " modified in current, deleted in other." << std::endl;
            std::string current_content = get_file_content_from_blob_hash(current_blob);
            std::string other_content = "";
            std::string lca_content = get_file_content_from_blob_hash(lca_blob);
            write_file_with_conflict_markers(filepath, current_content, other_content, lca_content);
            conflicts_occurred = true;
            merged_snapshot[filepath] = ""; // Mark as conflicted
        }
        else if (!lca_blob.empty() && current_blob.empty() && other_blob.empty())
        { // File deleted in both (no conflict)
            merged_snapshot.erase(filepath);
            std::cout << "Deleted file: " << filepath << std::endl;
        }
        else if (!current_blob.empty() && !lca_blob.empty() && current_blob != lca_blob && other_blob == lca_blob)
        { // File modified in current only
            std::cout << "Modified file (current): " << filepath << std::endl;
        }
        else if (!other_blob.empty() && !lca_blob.empty() && other_blob != lca_blob && current_blob == lca_blob)
        { // File modified in other only
            merged_snapshot[filepath] = other_blob;
            std::cout << "Modified file (other): " << filepath << std::endl;
        }
        else if (!current_blob.empty() && current_blob != lca_blob && other_blob != lca_blob && current_blob == other_blob)
        { // File modified in both, no conflict (same changes)
            merged_snapshot[filepath] = current_blob;
            std::cout << "Modified file (both same): " << filepath << std::endl;
        }
        else if (!current_blob.empty() && !other_blob.empty() && current_blob != other_blob && current_blob != lca_blob && other_blob != lca_blob)
        { // File modified in both, different changes (conflict!)
            std::cout << "CONFLICT (content): both modified " << filepath << std::endl;
            std::string current_content = get_file_content_from_blob_hash(current_blob);
            std::string other_content = get_file_content_from_blob_hash(other_blob);
            std::string lca_content = get_file_content_from_blob_hash(lca_blob);

            write_file_with_conflict_markers(filepath, current_content, other_content, lca_content);
            conflicts_occurred = true;
            merged_snapshot[filepath] = ""; // Mark as conflicted
        }
    }
    return merged_snapshot;
}

void MiniGit::write_file_with_conflict_markers(const std::string &filepath, const std::string &current_content, const std::string &other_content, const std::string &lca_content)
{
    std::string conflict_content = "";
    conflict_content += "<<<<<<< HEAD\n";
    conflict_content += current_content;
    if (!lca_content.empty())
    {
        conflict_content += "||||||| base\n";
        conflict_content += lca_content;
    }
    conflict_content += "=======\n";
    conflict_content += other_content;
    conflict_content += ">>>>>>> MERGE_BRANCH\n";

    Utils::writeFile(repo_path / filepath, conflict_content);
}

void MiniGit::merge(const std::string &branch_name)
{
    if (branch_name.empty())
    {
        std::cerr << "Error: Merge branch name cannot be empty." << std::endl;
        return;
    }

    std::string current_branch_name = "";
    std::string head_content = Utils::readFile(head_path.string());
    if (head_content.rfind("ref: ", 0) == 0)
    {
        current_branch_name = fs::path(head_content.substr(5)).filename().string();
    }
    else
    {
        std::cerr << "Error: Cannot merge in a detached HEAD state. Please checkout a branch first." << std::endl;
        return;
    }

    if (current_branch_name == branch_name)
    {
        std::cout << "Cannot merge a branch with itself." << std::endl;
        return;
    }

    fs::path merge_branch_path = refs_path / "heads" / branch_name;
    if (!fs::exists(merge_branch_path))
    {
        std::cerr << "Error: Branch '" << branch_name << "' does not exist." << std::endl;
        return;
    }

    std::string current_commit_hash = get_head_commit_hash();
    std::string merge_commit_hash = Utils::readFile(merge_branch_path.string());

    if (current_commit_hash.empty() || merge_commit_hash.empty())
    {
        std::cerr << "Error: Both branches must have at least one commit to merge." << std::endl;
        return;
    }

    Commit current_commit = get_commit(current_commit_hash);
    Commit merge_commit = get_commit(merge_commit_hash);

    if (is_ancestor(merge_commit_hash, current_commit_hash))
    {
        std::cout << "Already up to date." << std::endl;
        return;
    }
    if (is_ancestor(current_commit_hash, merge_commit_hash))
    {
        std::cout << "Fast-forward merge detected." << std::endl;
        update_head(merge_commit_hash, true, current_branch_name);

        // Cleanup for fast-forward: Only remove files not in new snapshot or protected.
        std::set<std::string> protected_files = {
            ".minigit",
            "main.cpp", "minigit.cpp", "minigit.h", "utils.cpp", "utils.h",
            "Makefile",
            "minigit"};
        for (const auto &entry : fs::directory_iterator(repo_path))
        {
            std::string filename = entry.path().filename().string();
            if (protected_files.count(filename))
            {
                continue;
            }
            if (!merge_commit.snapshot.count(filename) || fs::is_directory(entry.path()))
            {
                fs::remove_all(entry.path());
            }
        }
        // Write files from merge_commit's snapshot
        for (const auto &pair : merge_commit.snapshot)
        {
            const std::string &filepath = pair.first;
            const std::string &blob_hash = pair.second;
            std::string content = get_file_content_from_blob_hash(blob_hash);
            if (!content.empty())
            {
                Utils::writeFile(repo_path / filepath, content);
            }
            else
            {
                Utils::writeFile(repo_path / filepath, ""); // Ensure file is created, even if empty
            }
        }
        write_index(merge_commit.snapshot);
        std::cout << "Fast-forward to " << merge_commit_hash.substr(0, 7) << std::endl;
        return;
    }

    std::cout << "Performing a three-way merge..." << std::endl;
    std::string lca_hash = find_lca(current_commit_hash, merge_commit_hash);
    if (lca_hash.empty())
    {
        std::cerr << "Error: Could not find a common ancestor between " << current_branch_name << " and " << branch_name << std::endl;
        return;
    }

    Commit lca_commit = get_commit(lca_hash);

    std::cout << "LCA: " << lca_hash.substr(0, 7) << std::endl;

    bool conflicts_occurred = false;
    std::map<std::string, std::string> merged_snapshot = apply_merge_changes(
        current_commit.snapshot, merge_commit.snapshot, lca_commit.snapshot, conflicts_occurred);

    if (conflicts_occurred)
    {
        std::cout << "Automatic merge failed; fix conflicts and then commit the result." << std::endl;
        write_index(merged_snapshot);
    }
    else
    {
        std::cout << "Merge completed successfully. Creating a merge commit." << std::endl;

        std::map<std::string, std::string> final_merge_snapshot;
        for (const auto &pair : merged_snapshot)
        {
            if (!pair.second.empty())
            {
                final_merge_snapshot[pair.first] = pair.second;
            }
            else
            {
                std::string current_content_of_file = Utils::readFile(pair.first);
                std::string new_blob_hash = Utils::sha1(current_content_of_file);
                final_merge_snapshot[pair.first] = new_blob_hash;
                Utils::writeFile((objects_path / new_blob_hash).string(), current_content_of_file);
            }
        }

        std::string merge_message = "Merge branch '" + branch_name + "' into " + current_branch_name;

        Commit new_merge_commit_obj;
        new_merge_commit_obj.parent_hash = current_commit_hash;
        new_merge_commit_obj.second_parent_hash = merge_commit_hash;
        new_merge_commit_obj.message = merge_message;
        new_merge_commit_obj.author = "MiniGit Merge";
        new_merge_commit_obj.timestamp = std::time(nullptr);
        new_merge_commit_obj.snapshot = final_merge_snapshot;

        std::string new_merge_commit_hash = Utils::sha1(serialize_commit_data(new_merge_commit_obj));
        new_merge_commit_obj.hash = new_merge_commit_hash;

        Utils::writeFile((objects_path / new_merge_commit_obj.hash).string(), serialize_commit_data(new_merge_commit_obj));

        update_head(new_merge_commit_obj.hash, true, current_branch_name);
        write_index(new_merge_commit_obj.snapshot);

        std::cout << "Merge commit created: " << new_merge_commit_obj.hash.substr(0, 7) << std::endl;
    }
}