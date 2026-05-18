#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <stack>
#include <fstream>
#include <ctime>
#include <algorithm>
#include <iomanip>
#include <sstream>
using namespace std;

// =========================
// SONG STRUCT
// =========================
struct Song {
    string title;
    string artist;
    string genre;
    string youtubeLink;
    int duration;
    Song(string t = "", string a = "", string g = "", int d = 0, string yt = "")
        : title(t), artist(a), genre(g), duration(d), youtubeLink(yt) {}

    string getFormattedDuration() const {
        int m = duration / 60;
        int s = duration % 60;
        return to_string(m) + ":" + (s < 10 ? "0" : "") + to_string(s);
    }

    void display() const {
        cout << "Title: " << title << "\n";
        cout << "Artist: " << artist << "\n";
        cout << "Genre: " << genre << "\n";
        cout << "Duration: " << getFormattedDuration() << "\n";
        cout << "YouTube: " << youtubeLink << "\n";
        cout << "----------------------\n";
    }
};

// =========================
// BINARY SEARCH TREE NODE
// =========================
struct TreeNode {
    Song* song;          // pointer to avoid duplication
    TreeNode* left;
    TreeNode* right;
    TreeNode(Song* s) : song(s), left(nullptr), right(nullptr) {}
};

// =========================
// PLAYLIST MANAGER
// =========================
class PlaylistManager {
private:
    string name;
    vector<Song*> songs;          // stores pointers to Song objects (owns them)
    vector<string> logs;
    queue<Song*> playQueue;       // stores pointers (no copy)
    stack<Song*> undoStack;       // stores pointers (no copy)
    TreeNode* bstRoot;            // BST for fast search by title

    // ---------- BST: Insert ----------
    TreeNode* insertBST(TreeNode* root, Song* s) {
        if (!root) return new TreeNode(s);
        if (s->title < root->song->title)
            root->left = insertBST(root->left, s);
        else if (s->title > root->song->title)
            root->right = insertBST(root->right, s);
        // if equal title, do nothing (no duplicates allowed)
        return root;
    }

    // ---------- BST: Find minimum node ----------
    TreeNode* findMin(TreeNode* root) {
        while (root && root->left) root = root->left;
        return root;
    }

    // ---------- BST: Delete by title ----------
    TreeNode* deleteBST(TreeNode* root, const string& title) {
        if (!root) return nullptr;
        if (title < root->song->title)
            root->left = deleteBST(root->left, title);
        else if (title > root->song->title)
            root->right = deleteBST(root->right, title);
        else {
            // Found the node to delete
            if (!root->left) {
                TreeNode* rightChild = root->right;
                delete root;
                return rightChild;
            } else if (!root->right) {
                TreeNode* leftChild = root->left;
                delete root;
                return leftChild;
            } else {
                // Node with two children: get inorder successor (smallest in right subtree)
                TreeNode* temp = findMin(root->right);
                root->song = temp->song;               // copy pointer (not full object)
                root->right = deleteBST(root->right, temp->song->title);
            }
        }
        return root;
    }

    // ---------- BST: Search ----------
    TreeNode* searchBST(TreeNode* root, const string& title) const {
        if (!root || root->song->title == title)
            return root;
        if (title < root->song->title)
            return searchBST(root->left, title);
        else
            return searchBST(root->right, title);
    }

    // ---------- Delete entire BST (to avoid memory leak) ----------
    void deleteBST(TreeNode* root) {
        if (!root) return;
        deleteBST(root->left);
        deleteBST(root->right);
        delete root;
    }

    // ---------- YouTube link validation ----------
    bool validYoutube(const string& url) {
        return url.find("youtube.com") != string::npos ||
               url.find("youtu.be") != string::npos;
    }

    // ---------- Quick Sort (on vector<Song*> by title) ----------
    int partition(int low, int high) {
        Song* pivot = songs[high];
        int i = low - 1;
        for (int j = low; j < high; j++) {
            if (songs[j]->title < pivot->title) {
                i++;
                swap(songs[i], songs[j]);
            }
        }
        swap(songs[i + 1], songs[high]);
        return i + 1;
    }

    void quickSort(int low, int high) {
        if (low < high) {
            int pi = partition(low, high);
            quickSort(low, pi - 1);
            quickSort(pi + 1, high);
        }
    }

public:
    PlaylistManager(string n = "Playlist") : name(n), bstRoot(nullptr) {}

    ~PlaylistManager() {
        // Delete all Song objects (owned by vector)
        for (Song* s : songs) delete s;
        deleteBST(bstRoot);
    }

    // ---------- Log activity ----------
    void logActivity(const string& action) {
        time_t now = time(0);
        tm t;
        localtime_s(&t, &now);
        stringstream ss;
        ss << put_time(&t, "%Y-%m-%d %H:%M:%S");
        logs.push_back(ss.str() + " - " + action);
    }

    // ---------- Add song ----------
    void addSong(const Song& s) {
        if (!validYoutube(s.youtubeLink)) {
            cout << "Invalid YouTube Link\n";
            return;
        }
        Song* newSong = new Song(s);   // allocate on heap
        songs.push_back(newSong);
        playQueue.push(newSong);
        bstRoot = insertBST(bstRoot, newSong);
        logActivity("Added: " + newSong->title);
        cout << "Song Added\n";
    }

    // ---------- Remove last song ----------
    void removeLastSong() {
        if (songs.empty()) {
            cout << "Empty Playlist\n";
            return;
        }
        Song* last = songs.back();
        songs.pop_back();
        undoStack.push(last);                // push pointer (no copy)
        // Remove from BST
        bstRoot = deleteBST(bstRoot, last->title);
        logActivity("Removed: " + last->title);
        cout << "Removed Last Song\n";
        // Note: last is NOT deleted yet – it's stored in undoStack
    }

    // ---------- Undo removal ----------
    void undoRemove() {
        if (undoStack.empty()) {
            cout << "Nothing To Undo\n";
            return;
        }
        Song* restored = undoStack.top();
        undoStack.pop();
        songs.push_back(restored);
        // Re‑insert into BST
        bstRoot = insertBST(bstRoot, restored);
        logActivity("Undo: " + restored->title);
        cout << "Undo Done\n";
    }

    // ---------- Remove song by title (extra feature, optional) ----------
    bool removeSongByTitle(const string& title) {
        // Find position in vector
        int pos = -1;
        for (size_t i = 0; i < songs.size(); i++) {
            if (songs[i]->title == title) {
                pos = i;
                break;
            }
        }
        if (pos == -1) return false;
        Song* toRemove = songs[pos];
        songs.erase(songs.begin() + pos);
        undoStack.push(toRemove);
        bstRoot = deleteBST(bstRoot, title);
        logActivity("Removed by title: " + title);
        cout << "Removed: " << title << "\n";
        return true;
    }

    // ---------- Display all (insertion order) ----------
    void displayAll() const {
        if (songs.empty()) {
            cout << "Empty Playlist\n";
            return;
        }
        int i = 1;
        for (Song* s : songs) {
            cout << i++ << ".\n";
            s->display();
        }
    }

    // ---------- Play next from queue ----------
    void playNext() {
        if (playQueue.empty()) {
            cout << "Queue Empty\n";
            return;
        }
        Song* next = playQueue.front();
        playQueue.pop();
        cout << "Now Playing: " << next->title << "\n";
    }

    // ---------- Search using BST ----------
    void searchSong(const string& title) {
        TreeNode* result = searchBST(bstRoot, title);
        if (result) {
            cout << "Song Found:\n";
            result->song->display();
        } else {
            cout << "Song Not Found\n";
        }
    }

    // ---------- Sort playlist using Quick Sort (with pre‑sort check) ----------
    void sortPlaylist() {
        if (songs.size() <= 1) {
            cout << "Playlist already sorted (trivial)\n";
            return;
        }
        // Check if already sorted
        bool alreadySorted = true;
        for (size_t i = 1; i < songs.size(); i++) {
            if (songs[i-1]->title > songs[i]->title) {
                alreadySorted = false;
                break;
            }
        }
        if (alreadySorted) {
            cout << "Playlist is already sorted by title. Nothing to do.\n";
            return;
        }
        // Perform Quick Sort (on pointers)
        quickSort(0, songs.size() - 1);
        cout << "Playlist Sorted by Title\n";
        // BST remains correct (only order of vector changed, BST still points to same songs)
    }

    // ---------- Save to CSV ----------
    void saveCSV() {
        ofstream file(name + ".csv");
        file << "Title,Artist,Genre,Duration,Youtube\n";
        for (Song* s : songs) {
            file << s->title << ","
                 << s->artist << ","
                 << s->genre << ","
                 << s->duration << ","
                 << s->youtubeLink << "\n";
        }
        file << "\nLogs\n";
        for (string& l : logs) {
            file << l << "\n";
        }
        cout << "Saved Successfully\n";
    }
};

// =========================
// SIMPLE CHATBOT
// =========================
class ChatBot {
public:
    void start() {
        string input;
        cout << "ChatBot Started (bye to exit)\n";
        while (true) {
            cout << "> ";
            getline(cin, input);
            transform(input.begin(), input.end(), input.begin(), ::tolower);
            if (input == "bye") {
                cout << "Goodbye\n";
                break;
            } else if (input == "hello") {
                cout << "Hello User\n";
            } else if (input == "help") {
                cout << "Commands: add, remove, play, search\n";
            } else {
                cout << "I am listening...\n";
            }
        }
    }
};

// =========================
// GLOBAL PLAYLIST
// =========================
PlaylistManager playlist("FunPlaylist");

// =========================
// MENU (optional extra remove by title)
// =========================
void showMenu() {
    cout << "\n1. Add Song";
    cout << "\n2. Remove Last";
    cout << "\n3. Display";
    cout << "\n4. Play Next";
    cout << "\n5. Search (BST)";
    cout << "\n6. Undo";
    cout << "\n7. Sort (Quick Sort with check)";
    cout << "\n8. Save CSV";
    cout << "\n9. ChatBot";
    cout << "\n10. Remove by Title (Bonus)";
    cout << "\n0. Exit";
    cout << "\nChoice: ";
}

// =========================
// MAIN
// =========================
int main() {
    ChatBot bot;
    int choice;
    do {
        showMenu();
        cin >> choice;
        cin.ignore();
        if (choice == 1) {
            string t, a, g, yt;
            int d;
            cout << "Title: "; getline(cin, t);
            cout << "Artist: "; getline(cin, a);
            cout << "Genre: "; getline(cin, g);
            cout << "Duration: "; cin >> d; cin.ignore();
            cout << "YouTube: "; getline(cin, yt);
            playlist.addSong(Song(t, a, g, d, yt));
        }
        else if (choice == 2) playlist.removeLastSong();
        else if (choice == 3) playlist.displayAll();
        else if (choice == 4) playlist.playNext();
        else if (choice == 5) {
            string t;
            cout << "Enter Title: "; getline(cin, t);
            playlist.searchSong(t);
        }
        else if (choice == 6) playlist.undoRemove();
        else if (choice == 7) playlist.sortPlaylist();
        else if (choice == 8) playlist.saveCSV();
        else if (choice == 9) bot.start();
        else if (choice == 10) {
            string t;
            cout << "Enter title to remove: "; getline(cin, t);
            if (!playlist.removeSongByTitle(t))
                cout << "Song not found.\n";
        }
    } while (choice != 0);
    return 0;
}
