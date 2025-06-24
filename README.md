# MiniGit: A Custom Version Control System

## Project Overview

MiniGit is a lightweight, command-line based version control system (VCS) designed to simulate the core functionalities of Git. Developed as a practical application of fundamental Data Structures and Algorithms (DSA) principles, MiniGit allows users to manage code changes, track history, and collaborate on projects locally. It demonstrates key concepts such as content-addressable storage, commit graph traversal, and three-way merging.

## Key Features Implemented

MiniGit supports the following commands, providing a foundational set of version control capabilities:

* **`minigit init`**:
    Initializes a new MiniGit repository in the current directory. This command sets up the essential `.minigit/` directory structure, including `objects/` (for storing blobs and commits), `refs/heads/` (for managing branches), `HEAD` (to point to the current branch/commit), and `index` (the staging area).

* **`minigit add <filename>`**:
    Stages specified files for the next commit. When a file is added, its content is read, a SHA-1 hash is computed, and the content (blob) is stored immutably within the `.minigit/objects/` directory. The staging area (`.minigit/index`) is updated to record the file's path and its corresponding blob hash.

* **`minigit commit -m "<message>"`**:
    Creates a new commit object representing the current state of the staging area. A unique SHA-1 hash is generated for this commit, derived from its content (metadata and snapshot). The commit object, containing its message, author, timestamp, parent commit(s) hash, and a snapshot of staged files (paths mapped to blob hashes), is then stored in `.minigit/objects/`. The `HEAD` pointer is updated to point to this new commit, and the staging area is cleared.

* **`minigit log`**:
    Displays the commit history starting from the `HEAD` commit. It traverses backward through the commit graph using parent pointers, presenting a chronological list of commits. Each entry shows the commit hash, author, date, and commit message. For merge commits, it also displays the hashes of both parent branches.

* **`minigit branch <branch-name>`**:
    Creates a new branch reference (a named pointer) that points to the current `HEAD` commit. This allows for the creation of parallel lines of development within the repository. Branch references are stored as files within the `.minigit/refs/heads/` directory.

* **`minigit checkout <branch-name/commit-hash>`**:
    Switches the working directory to reflect the exact state of a specified branch or commit. This operation updates the `HEAD` pointer to refer to the target branch or commit and restores the files from that commit's snapshot into the working directory.

* **`minigit merge <branch-name>`**:
    Integrates changes from a specified branch into the current active branch. MiniGit performs a three-way merge: it identifies the Lowest Common Ancestor (LCA) of the two branches and compares the file contents from the current branch, the merge branch, and the LCA.
    * **Conflict Handling**: If conflicting changes are detected in the same lines of a file, MiniGit marks these conflicts directly within the file using standard Git conflict markers (`<<<<<<<`, `=======`, `>>>>>>>`), requiring manual resolution by the user.
    * **Merge Commit**: Upon successful resolution of any conflicts (or if no conflicts exist), a new "merge commit" is created. This special commit has two parent pointers: one to the tip of the current branch and one to the tip of the merged branch, preserving the history of both lines of development.

## Internal Data Structures & Design Decisions

MiniGit's architecture is heavily inspired by Git's object model, emphasizing immutability and content addressing through a file-based storage system.

* **Blobs (File Content)**:
    * **DSA Concept**: Hashing, File I/O.
    * **Design**: Raw file content is stored as "blob" objects. A SHA-1 hash of the content serves as its unique identifier. These blobs are stored in a two-level directory structure (`.minigit/objects/<first2_chars_of_hash>/<rest_of_hash>`), enabling efficient storage and lookup of immutable file versions.

* **Commit Nodes**:
    * **DSA Concept**: Directed Acyclic Graph (DAG) for history, Linked List (for linear history traversal).
    * **Design**: Each commit is represented by a `Commit` struct/class containing metadata (message, author, timestamp) and pointers (`parent_hash`, `second_parent_hash`) to its parent commit(s). Crucially, a commit also stores a `snapshot` (`std::map<std::string, std::string>`), which maps file paths to their corresponding blob hashes. Commit objects are serialized into text files and stored in `objects/` using their unique SHA-1 hash.

* **Branch References (`HEAD`, `refs/heads/`)**:
    * **DSA Concept**: HashMap (mapping branch names to commit hashes).
    * **Design**: The `.minigit/HEAD` file indicates the current state of the repository (either pointing to a branch reference, e.g., `ref: refs/heads/main`, or directly to a commit hash for a detached HEAD). Branch names are represented by files within `.minigit/refs/heads/`, and the content of these files is the SHA-1 hash of the commit the branch currently points to.

* **Staging Area (`index`)**:
    * **DSA Concept**: Hash Table / Set (for efficient tracking of staged files).
    * **Design**: The staging area is managed through the `.minigit/index` file. In memory, it's represented as a `std::map<std::string, std::string>` that maps file paths (relative to the repository root) to the SHA-1 hashes of their staged blob content.

* **Log History Traversal**:
    * **DSA Concept**: Graph Traversal.
    * **Design**: The `log` command iteratively deserializes `Commit` objects, starting from `HEAD`, and follows their `parent_hash` (first parent) to reconstruct and display the linear commit history. For merge commits, it shows both parents to illustrate the merge lineage.

* **Merge Logic**:
    * **DSA Concept**: Graph Traversal (BFS for `is_ancestor` and `find_lca`), Three-Way Merge Algorithm.
    * **Design**: The `merge` command leverages graph traversal techniques to determine if one commit is an ancestor of another (`is_ancestor`) and to find the Lowest Common Ancestor (LCA) between two diverging branches. The merge algorithm then compares file contents from the LCA, current branch tip, and merge branch tip to intelligently combine changes and highlight conflicts.

## Limitations and Future Improvements

MiniGit, while demonstrating core VCS concepts, has several limitations:

* **Team Collaboration Workflow**: Due to limitations in available computing resources, the project development was conducted collaboratively on a single machine. This meant that the requirement for individual team members to make distinct commits from separate PCs could not be fully demonstrated in the commit history.
* **No Remote Operations**: The current implementation is entirely local. It lacks functionality for `clone`, `push`, `pull`, or interacting with remote repositories.
* **Basic Diffing**: A dedicated `minigit diff` command to show line-by-line differences between file versions or commits is not implemented.
* **Limited Conflict Resolution**: While conflicts are marked, there are no built-in tools within the MiniGit CLI for automated or assisted conflict resolution; manual editing is required.
* **Hardcoded Author**: The commit author is currently a hardcoded default.
* **No `.gitignore` Support**: The system does not parse or respect `.gitignore` files to exclude specified files from tracking.
* **Advanced Commands Absent**: Features like `rebase`, `cherry-pick`, `tagging`, `stashing`, or `reverting` are not implemented.
* **Performance**: For very large files or repositories with extensive history, performance could be improved (e.g., through object packing).

**Future Improvements Could Include:**

* Implementing the `minigit diff` command for detailed content comparisons.
* Adding configurable author and user settings.
* Developing capabilities for network-based remote repository interaction (`clone`, `push`, `pull`).
* Implementing more sophisticated and user-friendly conflict resolution prompts.
* Adding support for `.gitignore` file parsing.
* Expanding the command set to include more advanced Git functionalities.
* Optimizing performance for large-scale operations.
* Adding comprehensive unit and integration tests.
* Tracking file permissions as part of the snapshot.