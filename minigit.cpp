#include "minigit.h"
<<<<<<< HEAD
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
=======
#include "utils.h"
#include <iostream>
#include <fstream>
#include <filesystem> // Required for directory iteration
#include <ctime>
#include <sstream>
#include <vector> // Required for storing matched hashes

MiniGit::MiniGit() {}

bool MiniGit::is_initialized()
{
    return std::filesystem::exists(MINIGIT_DIR);
>>>>>>> 4f8bf363fc285507403972efdc9ff5c491a62063
}

void MiniGit::init()
{
<<<<<<< HEAD
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
=======
    if (is_initialized())
    {
        std::cout << "MiniGit repository already initialized." << std::endl;
        return;
    }
    Utils::createDirectory(MINIGIT_DIR);
    Utils::createDirectory(OBJECTS_DIR);
    Utils::createDirectory(REFS_DIR);
    Utils::writeFile(HEAD_FILE, "ref: refs/main");
    std::cout << "Initialized empty MiniGit repository in " << std::filesystem::absolute(MINIGIT_DIR) << std::endl;
}

void MiniGit::add(const std::string &filename)
{
    if (!is_initialized())
    {
        std::cerr << "Error: Not a MiniGit repository." << std::endl;
        return;
    }

    std::map<std::string, std::string> staging_area = get_staging_area();

    // Get files from the current HEAD commit to check if a non-existent file was previously tracked
    std::string current_head_commit_hash = get_head_commit_hash();
    std::map<std::string, std::string> files_in_head_commit;
    if (!current_head_commit_hash.empty())
    {
        files_in_head_commit = get_commit_files(current_head_commit_hash);
    }

    if (!std::filesystem::exists(filename))
    {
        // Case: File does not exist in the working directory.
        // If it was tracked in the current HEAD commit OR was explicitly in the staging area,
        // then this 'add' call means to stage its deletion by removing it from the staging area.
        if (files_in_head_commit.count(filename) || staging_area.count(filename))
        {
            if (staging_area.count(filename))
            {
                staging_area.erase(filename); // Remove it from staging area
            }
            // If it's not in staging_area but was in HEAD, we still want to stage its deletion.
            // By calling set_staging_area, we ensure it won't be in the next commit's snapshot.
            set_staging_area(staging_area);
            std::cout << "Staged deletion of '" << filename << "'." << std::endl;
        }
        else
        {
            // File does not exist, and was never tracked or staged.
            std::cerr << "Error: File not found: " << filename << ". Was not previously tracked." << std::endl;
        }
        return;
    }

    // Case: File exists in the working directory. Stage its content.
    std::string file_content = Utils::readFile(filename);
    std::string blob_hash = Utils::sha1(file_content);

    // Only update staging area if content or hash has changed, or it's a new file
    if (staging_area.find(filename) == staging_area.end() || staging_area[filename] != blob_hash)
    {
        staging_area[filename] = blob_hash;
        set_staging_area(staging_area);
        Utils::writeFile(OBJECTS_DIR + "/" + blob_hash, file_content); // Store blob
        std::cout << "Staged " << filename << std::endl;
    }
    else
    {
        std::cout << "File '" << filename << "' is already up-to-date in staging area." << std::endl;
>>>>>>> 4f8bf363fc285507403972efdc9ff5c491a62063
    }
}

void MiniGit::commit(const std::string &message)
{
<<<<<<< HEAD
    std::map<std::string, std::string> current_snapshot = read_index();

    if (current_snapshot.empty())
    {
        std::cout << "Nothing to commit, working tree clean. (Staging area is empty)" << std::endl;
=======
    if (!is_initialized())
    {
        std::cerr << "Error: Not a MiniGit repository." << std::endl;
        return;
    }

    std::map<std::string, std::string> index = get_staging_area();
    if (index.empty())
    {
        std::cout << "Nothing to commit, working tree clean." << std::endl;
>>>>>>> 4f8bf363fc285507403972efdc9ff5c491a62063
        return;
    }

    std::string parent_hash = get_head_commit_hash();
<<<<<<< HEAD

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
=======
    std::stringstream commit_content_ss; // Using a stringstream to build commit content

    commit_content_ss << "parent: " << parent_hash << "\n";
    commit_content_ss << "timestamp: " << time(nullptr) << "\n";
    commit_content_ss << "message: " << message << "\n\n"; // Empty line separates metadata from files

    for (const auto &pair : index)
    {
        commit_content_ss << pair.first << " " << pair.second << "\n";
    }

    std::string commit_data = commit_content_ss.str();
    std::string new_commit_hash = Utils::sha1(commit_data); // <<< new_commit_hash is calculated here

    Utils::writeFile(OBJECTS_DIR + "/" + new_commit_hash, commit_data); // Store the commit object

    // Update the current branch pointer
    std::string head_content = Utils::readFile(HEAD_FILE);
    if (head_content.rfind("ref: ", 0) == 0)
    {                                                                                          // If HEAD points to a branch (e.g., "ref: refs/main" or "ref: refs/dev")
        std::string relative_branch_path = head_content.substr(std::string("ref: ").length()); // e.g., "refs/main"
        std::string full_branch_file_path = MINIGIT_DIR + "/" + relative_branch_path;          // Correct: .minigit/refs/main

        Utils::writeFile(full_branch_file_path, new_commit_hash);
        std::cout << "Committed to " << relative_branch_path << " [" << new_commit_hash.substr(0, 7) << "] " << message << std::endl;
    }
    else
    { // Detached HEAD (HEAD_FILE contains a direct commit hash)
        // When in detached HEAD, committing normally updates HEAD to the new commit
        Utils::writeFile(HEAD_FILE, new_commit_hash);
        std::cout << "Committed [detached HEAD] [" << new_commit_hash.substr(0, 7) << "] " << message << std::endl;
    }

    // Clear staging area (index)
    set_staging_area({});
>>>>>>> 4f8bf363fc285507403972efdc9ff5c491a62063
}

void MiniGit::log()
{
<<<<<<< HEAD
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
=======
    if (!is_initialized())
    {
        std::cerr << "Error: Not a MiniGit repository." << std::endl;
        return;
    }
    std::string current_commit = get_head_commit_hash();
    while (!current_commit.empty())
    {
        std::string commit_data = Utils::readFile(OBJECTS_DIR + "/" + current_commit);
        std::stringstream ss(commit_data);
        std::string line;
        std::string message;

        // Simple parsing to find the message line
        while (std::getline(ss, line))
        {
            if (line.rfind("message: ", 0) == 0)
            {
                message = line.substr(9);
                break;
            }
        }

        std::cout << "commit " << current_commit << "\n";
        std::cout << "Message: " << message << "\n\n";

        current_commit = get_commit_parent(current_commit);
>>>>>>> 4f8bf363fc285507403972efdc9ff5c491a62063
    }
}

void MiniGit::branch(const std::string &branch_name)
{
<<<<<<< HEAD
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

=======
    if (!is_initialized())
    {
        std::cerr << "Error: Not a MiniGit repository." << std::endl;
        return;
    }

    // You cannot create a branch if there are no commits yet
>>>>>>> 4f8bf363fc285507403972efdc9ff5c491a62063
    std::string current_commit_hash = get_head_commit_hash();
    if (current_commit_hash.empty())
    {
        std::cerr << "Error: Cannot create branch. No commits yet." << std::endl;
        return;
    }

<<<<<<< HEAD
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
=======
    // Construct the full path for the new branch file within .minigit/refs/
    std::string branch_file_path = REFS_DIR + "/" + branch_name;

    // Check if a branch with this name already exists
    if (std::filesystem::exists(branch_file_path))
    {
        std::cerr << "Error: A branch named '" << branch_name << "' already exists." << std::endl;
        return;
    }

    // Write the current commit hash into the new branch file
    Utils::writeFile(branch_file_path, current_commit_hash);

    std::cout << "Branch '" << branch_name << "' created, pointing to " << current_commit_hash.substr(0, 7) << std::endl;
}

void MiniGit::checkout(const std::string &name)
{
    if (!is_initialized())
    {
        std::cerr << "Error: Not a MiniGit repository." << std::endl;
        return;
    }

    std::string commit_hash_to_checkout;
    std::string new_head_content; // What will be written to .minigit/HEAD

    // 1. Determine if 'name' is a branch or a commit hash
    std::string branch_ref_path = REFS_DIR + "/" + name; // This points to .minigit/refs/<branchname>

    if (std::filesystem::exists(branch_ref_path))
    {
        // It's a branch name
        commit_hash_to_checkout = Utils::readFile(branch_ref_path);
        // CRITICAL FIX HERE: HEAD should point to `refs/<branch_name>`, NOT `.minigit/refs/<branch_name>`
        new_head_content = "ref: refs/" + name; // Correct format for HEAD when pointing to a branch
    }
    else if (name.length() == 40 && std::filesystem::exists(OBJECTS_DIR + "/" + name))
    {
        // Assume it's a full 40-character commit hash (detached HEAD)
        commit_hash_to_checkout = name;
        new_head_content = name;
    }
    else
    {
        // It's not a branch and not a full 40-char hash, so try to resolve as an abbreviated hash
        std::vector<std::string> matching_hashes;
        for (const auto &entry : std::filesystem::directory_iterator(OBJECTS_DIR))
        {
            if (entry.is_regular_file())
            {
                std::string object_hash = entry.path().filename().string();
                if (object_hash.length() >= name.length() && object_hash.rfind(name, 0) == 0)
                {
                    matching_hashes.push_back(object_hash);
                }
            }
        }

        if (matching_hashes.size() == 1)
        {
            // Exactly one match found, use its full hash
            commit_hash_to_checkout = matching_hashes[0];
            new_head_content = matching_hashes[0];
        }
        else if (matching_hashes.size() > 1)
        {
            // Ambiguous abbreviation
            std::cerr << "Error: Ambiguous abbreviation '" << name << "'. It matches multiple commits:" << std::endl;
            for (const auto &hash : matching_hashes)
            {
                std::cerr << "  " << hash << std::endl;
            }
            return;
        }
        else
        {
            // No matches found for abbreviation
            std::cerr << "Error: pathspec '" << name << "' did not match any file(s), branch, or commit known to minigit." << std::endl;
            return;
        }
    }

    if (commit_hash_to_checkout.empty())
    {
        std::cerr << "Error: Could not find commit for '" << name << "'" << std::endl;
        return;
    }

    // Get the files from the commit we are checking out to
    auto files_in_target_commit = get_commit_files(commit_hash_to_checkout);

    // Get the files that are currently tracked by MiniGit (from current HEAD commit)
    // This is crucial to only delete files MiniGit *knows* it should manage.
    std::string current_head_commit_hash = get_head_commit_hash();
    std::map<std::string, std::string> files_in_current_head;
    if (!current_head_commit_hash.empty())
    { // Only if there's a valid HEAD commit
        files_in_current_head = get_commit_files(current_head_commit_hash);
    }

    // 2. Clean the working directory: Delete files that were tracked by the CURRENT HEAD
    //    but are NOT present in the TARGET commit.
    for (const auto &current_file_pair : files_in_current_head)
    {
        std::string filename = current_file_pair.first;
        // If this file from current HEAD is NOT in the target commit
        if (files_in_target_commit.find(filename) == files_in_target_commit.end())
        {
            if (std::filesystem::exists(filename)) // Only try to remove if it physically exists
            {
                std::filesystem::remove(filename);
                std::cout << "Deleted: " << filename << std::endl;
            }
        }
    }

    // 3. Populate the working directory with files from the target commit
    for (const auto &file_pair : files_in_target_commit)
    {
        std::string filename = file_pair.first;
        std::string blob_hash = file_pair.second;
        std::string file_content = Utils::readFile(OBJECTS_DIR + "/" + blob_hash);
        Utils::writeFile(filename, file_content);
        std::cout << "Updated: " << filename << std::endl;
    }

    // 4. Clear the staging area (index)
    set_staging_area({});

    // 5. Update the HEAD file
    Utils::writeFile(HEAD_FILE, new_head_content);

    // Provide final success message
    if (new_head_content.rfind("ref:", 0) == 0)
    { // Starts with "ref:" means it's a branch
        std::cout << "Switched to branch '" << name << "'" << std::endl;
    }
    else
    { // Direct commit hash
        // Use the full commit_hash_to_checkout here, as 'name' could be an abbreviation
        std::cout << "Switched to commit '" << commit_hash_to_checkout.substr(0, 7) << "' (detached HEAD)" << std::endl;
    }
>>>>>>> 4f8bf363fc285507403972efdc9ff5c491a62063
}

void MiniGit::merge(const std::string &branch_name)
{
<<<<<<< HEAD
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
=======
    if (!is_initialized())
    {
        std::cerr << "Error: Not a MiniGit repository." << std::endl;
        return;
    }

    std::string current_branch_commit = get_head_commit_hash();
    // Note: This also needs to read from the full path REFS_DIR + "/" + branch_name
    std::string target_branch_commit = Utils::readFile(REFS_DIR + "/" + branch_name);

    if (current_branch_commit == target_branch_commit)
>>>>>>> 4f8bf363fc285507403972efdc9ff5c491a62063
    {
        std::cout << "Already up to date." << std::endl;
        return;
    }
<<<<<<< HEAD
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
=======

    std::string lca_hash = find_lca(current_branch_commit, target_branch_commit);

    if (lca_hash == target_branch_commit)
    {
        std::cout << "Already up to date." << std::endl;
        return;
    }
    if (lca_hash == current_branch_commit)
    {
        std::cout << "Fast-forwarding..." << std::endl;
        checkout(branch_name); // Simply checkout to the target branch
        return;
    }

    // 3-way merge
    auto lca_files = get_commit_files(lca_hash);
    auto current_files = get_commit_files(current_branch_commit);
    auto target_files = get_commit_files(target_branch_commit);

    bool conflict = false;
    for (const auto &target_pair : target_files)
    {
        std::string filename = target_pair.first;
        std::string target_hash = target_pair.second;

        std::string current_hash = current_files.count(filename) ? current_files[filename] : "";
        std::string lca_hash_file = lca_files.count(filename) ? lca_files[filename] : "";

        if (current_hash != lca_hash_file && target_hash != lca_hash_file && current_hash != target_hash)
        {
            std::cout << "CONFLICT: both modified " << filename << std::endl;
            conflict = true;
        }
        else if (target_hash != lca_hash_file && current_hash == lca_hash_file)
        {
            // Take the target version
            add(filename);
        }
    }

    if (conflict)
    {
        std::cerr << "Automatic merge failed; fix conflicts and then commit the result." << std::endl;
        return;
    }

    std::string message = "Merge branch '" + branch_name + "'";
    commit(message);
}

// --- Private Helper Implementations ---

std::map<std::string, std::string> MiniGit::get_staging_area()
{
    std::map<std::string, std::string> index;
    if (!std::filesystem::exists(INDEX_FILE))
        return index;

    std::ifstream file(INDEX_FILE);
    std::string line, filename, hash;
    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        ss >> filename >> hash;
        index[filename] = hash;
    }
    return index;
}

void MiniGit::set_staging_area(const std::map<std::string, std::string> &index)
{
    std::ofstream file(INDEX_FILE);
    for (const auto &pair : index)
    {
        file << pair.first << " " << pair.second << "\n";
    }
}

std::string MiniGit::get_head_ref_path()
{
    std::string head_content = Utils::readFile(HEAD_FILE);
    // e.g., "ref: refs/main"
    // Make sure to handle the case where HEAD_FILE might be empty or not formatted as "ref: ..."
    if (head_content.rfind("ref: ", 0) == 0)
    {
        return head_content.substr(5);
    }
    // If it's a detached HEAD, HEAD_FILE directly contains the commit hash
    return head_content;
}

std::string MiniGit::get_head_commit_hash()
{
    if (!std::filesystem::exists(HEAD_FILE))
        return "";

    std::string head_content = Utils::readFile(HEAD_FILE);
    if (head_content.rfind("ref: ", 0) == 0)
    {
        // It's a symbolic ref (e.g., ref: refs/main)
        std::string ref_path = head_content.substr(5);
        // CORRECTED: Prepend MINIGIT_DIR to get the actual path to the ref file
        return Utils::readFile(MINIGIT_DIR + "/" + ref_path);
    }
    else
    {
        // It's a detached HEAD (contains the commit hash directly)
        return head_content;
    }
}

std::string MiniGit::get_commit_parent(const std::string &commit_hash)
{
    if (commit_hash.empty())
        return "";
    std::string commit_data = Utils::readFile(OBJECTS_DIR + "/" + commit_hash);
    std::stringstream ss(commit_data);
    std::string line;
    std::getline(ss, line); // parent: <hash>

    // CORRECTED: Relax the condition to allow "parent: " (8 chars) for initial commit
    if (line.rfind("parent: ", 0) == 0)
    {
        // Return whatever is after "parent: ", which could be an empty string for initial commit
        return line.substr(8);
    }
    return "";
}

std::map<std::string, std::string> MiniGit::get_commit_files(const std::string &commit_hash)
{
    std::map<std::string, std::string> files;
    if (commit_hash.empty())
        return files;
    std::string commit_data = Utils::readFile(OBJECTS_DIR + "/" + commit_hash);
    std::stringstream ss(commit_data);
    std::string line, filename, file_hash;

    // Skip parent, timestamp, and message lines
    // Read lines until an empty line (separating metadata from file list) or EOF
    while (std::getline(ss, line))
    {
        if (line.empty())
        { // Found the empty line separator
            break;
        }
        // Skip metadata lines like "parent:", "timestamp:", "message:"
        if (line.rfind("parent:", 0) == 0 ||
            line.rfind("timestamp:", 0) == 0 ||
            line.rfind("message:", 0) == 0)
        {
            continue;
        }
        // If we reach here and the line is not empty, it should be a file entry
        // This means the loop for metadata parsing was not fully correct,
        // it should only consume actual metadata lines until the empty line.
        // For robustness, let's assume if it's not metadata and not empty, it's a file.
        // The original code had a slight issue with this parsing - let's fix it properly.
        // We'll rely on the break on empty line and then parse file entries.
    }

    // Now process the actual file entries after the empty line (or directly if no metadata)
    while (std::getline(ss, line))
    {
        if (line.find(" ") != std::string::npos)
        {
            std::stringstream line_ss(line);
            line_ss >> filename >> file_hash;
            files[filename] = file_hash;
        }
    }
    return files;
}

std::string MiniGit::find_lca(const std::string &commit1_hash, const std::string &commit2_hash)
{
    std::set<std::string> history1;
    std::string current = commit1_hash;
    while (!current.empty())
    {
        history1.insert(current);
        current = get_commit_parent(current);
    }

    current = commit2_hash;
    while (!current.empty())
    {
        if (history1.count(current))
        {
            return current;
        }
        current = get_commit_parent(current);
    }
    return ""; // Should not happen in a connected history
>>>>>>> 4f8bf363fc285507403972efdc9ff5c491a62063
}