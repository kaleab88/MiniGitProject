#include "minigit.h"
#include "utils.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <ctime>
#include <sstream>

MiniGit::MiniGit() {}

bool MiniGit::is_initialized()
{
    return std::filesystem::exists(MINIGIT_DIR);
}

void MiniGit::init()
{
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
    if (!std::filesystem::exists(filename))
    {
        std::cerr << "Error: File not found: " << filename << std::endl;
        return;
    }

    std::string content = Utils::readFile(filename);
    std::string hash = Utils::sha1(content);

    Utils::writeFile(OBJECTS_DIR + "/" + hash, content);

    auto index = get_staging_area();
    index[filename] = hash;
    set_staging_area(index);

    std::cout << "Staged " << filename << std::endl;
}

void MiniGit::commit(const std::string &message)
{
    if (!is_initialized())
    {
        std::cerr << "Error: Not a MiniGit repository." << std::endl;
        return;
    }
    auto index = get_staging_area();
    if (index.empty())
    {
        std::cout << "Nothing to commit, working tree clean." << std::endl;
        return;
    }

    std::string parent_hash = get_head_commit_hash();
    std::stringstream commit_content;
    commit_content << "parent: " << parent_hash << "\n";
    commit_content << "timestamp: " << time(nullptr) << "\n";
    commit_content << "message: " << message << "\n\n";

    for (const auto &pair : index)
    {
        commit_content << pair.first << " " << pair.second << "\n";
    }

    std::string commit_hash = Utils::sha1(commit_content.str());
    Utils::writeFile(OBJECTS_DIR + "/" + commit_hash, commit_content.str());

    std::string head_ref_path_relative_to_minigit = get_head_ref_path(); // This returns "refs/main"
    // CORRECTED LINE: Prepend MINIGIT_DIR to get the correct path
    Utils::writeFile(MINIGIT_DIR + "/" + head_ref_path_relative_to_minigit, commit_hash);

    // Clear the index
    set_staging_area({});

    std::cout << "Committed to " << head_ref_path_relative_to_minigit << " [" << commit_hash.substr(0, 7) << "] " << message << std::endl;
}

void MiniGit::log()
{
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
    }
}
// Inside your minigit.cpp file, locate or add this function:
void MiniGit::branch(const std::string &branch_name)
{
    if (!is_initialized())
    {
        std::cerr << "Error: Not a MiniGit repository." << std::endl;
        return;
    }

    // You cannot create a branch if there are no commits yet
    std::string current_commit_hash = get_head_commit_hash();
    if (current_commit_hash.empty())
    {
        std::cerr << "Error: Cannot create branch. No commits yet." << std::endl;
        return;
    }

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

    std::string commit_hash;
    std::string new_head_content;

    // Note: This also needs to read from the full path REFS_DIR + "/" + name
    if (std::filesystem::exists(REFS_DIR + "/" + name))
    {
        // It's a branch
        commit_hash = Utils::readFile(REFS_DIR + "/" + name);
        new_head_content = "ref: " + REFS_DIR + "/" + name;
    }
    else
    {
        // Assume it's a commit hash
        // Note: objects path is already correctly prefixed, so no change needed here
        if (std::filesystem::exists(OBJECTS_DIR + "/" + name))
        {
            commit_hash = name;
            new_head_content = name; // Detached HEAD
        }
        else
        {
            std::cerr << "Error: pathspec '" << name << "' did not match any file(s) or branch known to minigit." << std::endl;
            return;
        }
    }

    // Update working directory
    auto files_in_commit = get_commit_files(commit_hash);
    for (const auto &file_pair : files_in_commit)
    {
        std::string file_content = Utils::readFile(OBJECTS_DIR + "/" + file_pair.second);
        Utils::writeFile(file_pair.first, file_content);
    }

    // Clear the staging area
    set_staging_area({});

    Utils::writeFile(HEAD_FILE, new_head_content);
    std::cout << "Switched to '" << name << "'" << std::endl;
}

void MiniGit::merge(const std::string &branch_name)
{
    if (!is_initialized())
    {
        std::cerr << "Error: Not a MiniGit repository." << std::endl;
        return;
    }

    std::string current_branch_commit = get_head_commit_hash();
    // Note: This also needs to read from the full path REFS_DIR + "/" + branch_name
    std::string target_branch_commit = Utils::readFile(REFS_DIR + "/" + branch_name);

    if (current_branch_commit == target_branch_commit)
    {
        std::cout << "Already up to date." << std::endl;
        return;
    }

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
    while (std::getline(ss, line) && !line.empty())
    {
        // Skip metadata lines like "parent:", "timestamp:", "message:"
        if (line.rfind("parent:", 0) == 0 ||
            line.rfind("timestamp:", 0) == 0 ||
            line.rfind("message:", 0) == 0)
        {
            continue;
        }
        // If we reach here and the line is not empty, it should be a file entry
        if (line.find(" ") != std::string::npos)
        {
            std::stringstream line_ss(line);
            line_ss >> filename >> file_hash;
            files[filename] = file_hash;
        }
    }
    // Now process the actual file entries after the empty line
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
}