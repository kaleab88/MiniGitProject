#include "minigit.h"
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
    }
}

void MiniGit::commit(const std::string &message)
{
    if (!is_initialized())
    {
        std::cerr << "Error: Not a MiniGit repository." << std::endl;
        return;
    }

    std::map<std::string, std::string> index = get_staging_area();
    if (index.empty())
    {
        std::cout << "Nothing to commit, working tree clean." << std::endl;
        return;
    }

    std::string parent_hash = get_head_commit_hash();
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
}