
#include <iostream>
#include <vector>
#include <string>
#include <cctype>
#include <fstream>
#include <ctime>
#include <algorithm>
#include <chrono>      // for benchmarking
using namespace std;
using namespace std::chrono;

// ==================== TEMPLATE DOUBLY LINKED LIST ====================
template<typename T>
struct Node {
    T data;
    Node* next;
    Node* prev;
    Node(T d) : data(d), next(nullptr), prev(nullptr) {}
};

template <typename T>
class DoublyLinkedList {
private:
    Node<T>* head;
    Node<T>* tail;
    int count;
public:
    DoublyLinkedList() : head(nullptr), tail(nullptr), count(0) {}
    ~DoublyLinkedList() {
        while (head) {
            Node<T>* temp = head;
            head = head->next;
            delete temp;
        }
    }
    void push_back(T value) {
        Node<T>* n = new Node<T>(value);
        if (!head) head = tail = n;
        else {
            tail->next = n;
            n->prev = tail;
            tail = n;
        }
        ++count;
    }
    Node<T>* begin() const { return head; }
    bool empty() const { return head == nullptr; }
    int size() const { return count; }
};

// ==================== SONG STRUCT ====================
struct Song {
    string title, artist, genre, youtubelink;
    int duration;
    Song* next;
    Song* prev;
    Song(string t = "", string a = "", string g = "", int d = 0, string yt = "")
        : title(t), artist(a), genre(g), youtubelink(yt), duration(d >= 0 ? d : 0), next(nullptr), prev(nullptr) {}

    string getformattedDuration() const {
        int minutes = duration / 60;
        int seconds = duration % 60;
        string formatted = to_string(minutes) + ":";
        if (seconds < 10) formatted += "0";
        formatted += to_string(seconds);
        return formatted;
    }

    void display() const {
        cout << title << " by " << artist << endl;
        cout << "Genre: " << genre << endl;
        cout << "Duration: " << getformattedDuration() << endl;
        cout << "YouTube: " << youtubelink << endl;
        cout << "---------------" << endl;
    }
};

// ==================== TREE NODE FOR BST ====================
struct TreeNode {
    Song song;
    TreeNode* left;
    TreeNode* right;
    TreeNode(Song s) : song(s), left(nullptr), right(nullptr) {}
};

// ==================== PLAYLIST MANAGER ====================
class PlaylistManager {
private:
    string name;
    Song* head;
    Song* tail;
    DoublyLinkedList<string> activitylog;
    int count;

    bool isValidyoutubelink(const string& url) {
        string lowerUrl = url;
        for (char& c : lowerUrl) c = tolower(c);
        return (lowerUrl.find("youtube.com") != string::npos ||
                lowerUrl.find("youtu.be") != string::npos);
    }

public:
    PlaylistManager(string n = "My Playlist") : name(n), head(nullptr), tail(nullptr), count(0) {}

    bool isempty() const { return head == nullptr; }

    void Addsong(Song* s) {
        if (!isValidyoutubelink(s->youtubelink)) {
            cout << "INVALID YouTube Link!\n";
            delete s;
            return;
        }
        if (!head) head = tail = s;
        else {
            tail->next = s;
            s->prev = tail;
            tail = s;
        }
        count++;
        logActivity("Added: " + s->title + " (" + s->youtubelink + ")");
        cout << "Song added successfully\n";
    }

    // Remove song by title (new)
    bool removeSongByTitle(const string& title) {
        Song* cur = head;
        while (cur) {
            if (cur->title == title) {
                // Remove cur from list
                if (cur->prev) cur->prev->next = cur->next;
                else head = cur->next;
                if (cur->next) cur->next->prev = cur->prev;
                else tail = cur->prev;
                logActivity("Removed by title: " + cur->title);
                delete cur;
                count--;
                return true;
            }
            cur = cur->next;
        }
        return false;
    }

    void removeLastSong() {
        if (!tail) return;
        Song* temp = tail;
        logActivity("Removed: " + temp->title);
        if (tail->prev) {
            tail = tail->prev;
            tail->next = nullptr;
        } else {
            head = tail = nullptr;
        }
        delete temp;
        count--;
    }

    void displayAll() const {
        cout << "===== Playlist: " << name << " =====\n";
        if (!head) { cout << "Playlist is empty.\n"; return; }
        Song* cur = head;
        int i = 1;
        while (cur) {
            cout << "Song " << i++ << ":\n";
            cur->display();
            cur = cur->next;
        }
    }

    void logActivity(const string& action) {
        time_t now = time(0);
        char buffer[100];
        ctime_s(buffer, sizeof(buffer), &now);
        string timeStr(buffer);
        if (!timeStr.empty()) timeStr.pop_back();
        activitylog.push_back(timeStr + " - " + action);
    }

    void saveToExcel() {
        ofstream file(name + ".csv");
        if (!file) { cout << "Failed to create file.\n"; return; }
        file << "Title,Artist,Genre,Duration,YouTube Link\n";
        Song* cur = head;
        while (cur) {
            file << "\"" << cur->title << "\","
                 << "\"" << cur->artist << "\","
                 << "\"" << cur->genre << "\","
                 << "\"" << cur->getformattedDuration() << "\","
                 << "\"" << cur->youtubelink << "\"\n";
            cur = cur->next;
        }
        file << "\nActivity Log:\n";
        Node<string>* act = activitylog.begin();
        while (act) {
            file << act->data << endl;
            act = act->next;
        }
        file.close();
        cout << "Playlist saved as " << name << ".csv\n";
    }

    // Load from CSV (new)
    bool loadFromCSV(const string& filename) {
        ifstream file(filename);
        if (!file) return false;
        string line;
        getline(file, line); // skip header
        while (getline(file, line)) {
            if (line.empty() || line.find("Activity Log") != string::npos) break;
            // Simple CSV parsing (assumes quoted fields)
            string title, artist, genre, durationStr, yt;
            size_t pos = 0;
            auto extract = [&](string& out) {
                if (line[pos] == '"') {
                    pos++;
                    size_t end = line.find('"', pos);
                    out = line.substr(pos, end - pos);
                    pos = end + 2; // skip ","
                } else {
                    size_t end = line.find(',', pos);
                    out = line.substr(pos, end - pos);
                    pos = end + 1;
                }
            };
            extract(title);
            extract(artist);
            extract(genre);
            extract(durationStr);
            extract(yt);
            // durationStr is like "3:45" -> convert to seconds
            int minutes = 0, seconds = 0;
            sscanf(durationStr.c_str(), "%d:%d", &minutes, &seconds);
            int duration = minutes * 60 + seconds;
            Song* s = new Song(title, artist, genre, duration, yt);
            Addsong(s); // this also validates YouTube link
        }
        file.close();
        return true;
    }

    Song* getBack() { return tail; }
    int size() const { return count; }
    Song* begin() const { return head; }

    // Search by artist or genre (linear)
    void searchByArtist(const string& artist) {
        bool found = false;
        for (Song* cur = head; cur; cur = cur->next) {
            if (cur->artist == artist) {
                cur->display();
                found = true;
            }
        }
        if (!found) cout << "No songs by artist: " << artist << endl;
    }
    void searchByGenre(const string& genre) {
        bool found = false;
        for (Song* cur = head; cur; cur = cur->next) {
            if (cur->genre == genre) {
                cur->display();
                found = true;
            }
        }
        if (!found) cout << "No songs in genre: " << genre << endl;
    }

    // Edit song by title
    bool editSong(const string& title) {
        for (Song* cur = head; cur; cur = cur->next) {
            if (cur->title == title) {
                cout << "Editing: " << cur->title << endl;
                cout << "New title (enter to keep \"" << cur->title << "\"): ";
                string newTitle; getline(cin, newTitle);
                if (!newTitle.empty()) cur->title = newTitle;
                cout << "New artist (enter to keep \"" << cur->artist << "\"): ";
                string newArtist; getline(cin, newArtist);
                if (!newArtist.empty()) cur->artist = newArtist;
                cout << "New genre (enter to keep \"" << cur->genre << "\"): ";
                string newGenre; getline(cin, newGenre);
                if (!newGenre.empty()) cur->genre = newGenre;
                cout << "New duration in seconds (enter to keep " << cur->duration << "): ";
                string durStr; getline(cin, durStr);
                if (!durStr.empty()) cur->duration = stoi(durStr);
                cout << "New YouTube link (enter to keep current): ";
                string newLink; getline(cin, newLink);
                if (!newLink.empty()) cur->youtubelink = newLink;
                logActivity("Edited song: " + title);
                cout << "Song updated.\n";
                return true;
            }
        }
        return false;
    }
};

// ==================== GLOBAL OBJECTS ====================
PlaylistManager myPlaylist("FunPlaylist");
TreeNode* bstRoot = nullptr;

// Undo stack
struct StackNode {
    Song song;
    StackNode* next;
};
StackNode* topUndo = nullptr;
void pushUndo(const Song& s) {
    topUndo = new StackNode{s, topUndo};
}
bool popUndo(Song& s) {
    if (!topUndo) return false;
    s = topUndo->song;
    StackNode* temp = topUndo;
    topUndo = topUndo->next;
    delete temp;
    return true;
}

// Play queue
struct QueueNode {
    Song song;
    QueueNode* next;
};
QueueNode* frontQ = nullptr;
QueueNode* rearQ = nullptr;
void enqueueSong(const Song& s) {
    QueueNode* node = new QueueNode{s, nullptr};
    if (!rearQ) frontQ = rearQ = node;
    else {
        rearQ->next = node;
        rearQ = node;
    }
}
void playNext() {
    if (!frontQ) {
        cout << "Queue empty.\n";
        return;
    }
    cout << "Now playing: " << frontQ->song.title << endl;
    QueueNode* temp = frontQ;
    frontQ = frontQ->next;
    if (!frontQ) rearQ = nullptr;
    delete temp;
}

// BST functions
TreeNode* insertBST(TreeNode* root, const Song& s) {
    if (!root) return new TreeNode(s);
    if (s.title < root->song.title)
        root->left = insertBST(root->left, s);
    else
        root->right = insertBST(root->right, s);
    return root;
}
void searchBST(TreeNode* root, const string& title) {
    if (!root) {
        cout << "Song not found.\n";
        return;
    }
    if (root->song.title == title) {
        cout << "Found:\n";
        root->song.display();
    } else if (title < root->song.title)
        searchBST(root->left, title);
    else
        searchBST(root->right, title);
}

void addSong(const Song& s) {
    Song* newS = new Song(s.title, s.artist, s.genre, s.duration, s.youtubelink);
    myPlaylist.Addsong(newS);
}

// ==================== SORTING & SEARCHING ALGORITHMS ====================
bool compareByTitle(Song* a, Song* b) { return a->title < b->title; }
bool compareByArtist(Song* a, Song* b) { return a->artist < b->artist; }
bool compareByDuration(Song* a, Song* b) { return a->duration < b->duration; }

// Merge Sort
void merge(vector<Song*>& arr, int left, int mid, int right, bool (*compare)(Song*, Song*)) {
    int n1 = mid - left + 1;
    int n2 = right - mid;
    vector<Song*> L(n1), R(n2);
    for (int i = 0; i < n1; i++) L[i] = arr[left + i];
    for (int j = 0; j < n2; j++) R[j] = arr[mid + 1 + j];
    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (compare(L[i], R[j])) arr[k++] = L[i++];
        else arr[k++] = R[j++];
    }
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];
}
void mergeSort(vector<Song*>& arr, int left, int right, bool (*compare)(Song*, Song*)) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        mergeSort(arr, left, mid, compare);
        mergeSort(arr, mid + 1, right, compare);
        merge(arr, left, mid, right, compare);
    }
}

// Quick Sort
int partition(vector<Song*>& arr, int low, int high, bool (*compare)(Song*, Song*)) {
    Song* pivot = arr[high];
    int i = low - 1;
    for (int j = low; j < high; j++) {
        if (compare(arr[j], pivot)) {
            i++;
            swap(arr[i], arr[j]);
        }
    }
    swap(arr[i + 1], arr[high]);
    return i + 1;
}
void quickSort(vector<Song*>& arr, int low, int high, bool (*compare)(Song*, Song*)) {
    if (low < high) {
        int pi = partition(arr, low, high, compare);
        quickSort(arr, low, pi - 1, compare);
        quickSort(arr, pi + 1, high, compare);
    }
}

// Insertion Sort
void insertionSort(vector<Song*>& arr, bool (*compare)(Song*, Song*)) {
    int n = arr.size();
    for (int i = 1; i < n; i++) {
        Song* key = arr[i];
        int j = i - 1;
        while (j >= 0 && !compare(arr[j], key)) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

// Linear / Binary search (by title)
int linearSearch(const vector<Song*>& arr, const string& title) {
    for (int i = 0; i < arr.size(); i++)
        if (arr[i]->title == title) return i;
    return -1;
}
int binarySearch(const vector<Song*>& arr, const string& title) {
    int low = 0, high = arr.size() - 1;
    while (low <= high) {
        int mid = low + (high - low) / 2;
        if (arr[mid]->title == title) return mid;
        else if (arr[mid]->title < title) low = mid + 1;
        else high = mid - 1;
    }
    return -1;
}

void displaySortedSongs(const vector<Song*>& songs, const string& criterion) {
    if (songs.empty()) { cout << "Playlist empty.\n"; return; }
    cout << "\n===== Playlist sorted by " << criterion << " =====\n";
    for (size_t i = 0; i < songs.size(); i++) {
        cout << i + 1 << ". ";
        songs[i]->display();
    }
}

// ==================== BENCHMARKING ====================
void benchmarkSorts() {
    // Copy current playlist into vector
    vector<Song*> original;
    for (Song* cur = myPlaylist.begin(); cur; cur = cur->next)
        original.push_back(cur);
    if (original.size() < 2) {
        cout << "Need at least 2 songs to benchmark.\n";
        return;
    }

    auto runSort = [&](void (*sortFunc)(vector<Song*>&, int, int, bool(*)(Song*, Song*)), bool (*compare)(Song*, Song*), const string& name) {
        vector<Song*> copy = original;
        auto start = high_resolution_clock::now();
        sortFunc(copy, 0, copy.size() - 1, compare);
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start);
        cout << name << " took " << duration.count() << " microseconds.\n";
    };

    cout << "\n===== Sorting Algorithm Benchmark (by Title) =====\n";
    runSort(mergeSort, compareByTitle, "Merge Sort");
    runSort(quickSort, compareByTitle, "Quick Sort");
    // Insertion sort is O(n²) – separate call
    auto runInsertion = [&]() {
        vector<Song*> copy = original;
        auto start = high_resolution_clock::now();
        insertionSort(copy, compareByTitle);
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start);
        cout << "Insertion Sort took " << duration.count() << " microseconds.\n";
    };
    runInsertion();
}

// ==================== COMMAND PARSER ====================
string toLower(string s) {
    for (char& c : s) c = tolower(c);
    return s;
}

void processCommand(string input) {
    string lower = toLower(input);

    // Command mapping (numbers or text)
    if (lower == "1" || lower == "add" || lower.find("add song") != string::npos) {
        string title, artist, genre, yt;
        int duration;
        cout << "Title: "; getline(cin, title);
        cout << "Artist: "; getline(cin, artist);
        cout << "Genre: "; getline(cin, genre);
        cout << "Duration (sec): "; cin >> duration; cin.ignore();
        cout << "YouTube Link: "; getline(cin, yt);
        Song s(title, artist, genre, duration, yt);
        addSong(s);
        enqueueSong(s);
        bstRoot = insertBST(bstRoot, s);
    }
    else if (lower == "2" || lower == "remove" || lower.find("remove last") != string::npos) {
        if (!myPlaylist.isempty()) {
            pushUndo(*myPlaylist.getBack());
            myPlaylist.removeLastSong();
            cout << "Removed last song.\n";
        } else cout << "Playlist empty.\n";
    }
    else if (lower == "3" || lower == "display" || lower.find("display playlist") != string::npos) {
        myPlaylist.displayAll();
    }
    else if (lower == "4" || lower == "play" || lower.find("play next") != string::npos) {
        playNext();
    }
    else if (lower == "5" || lower == "search" || lower.find("search song") != string::npos) {
        string title;
        cout << "Enter song title to search (BST): ";
        getline(cin, title);
        searchBST(bstRoot, title);
    }
    else if (lower == "6" || lower == "undo" || lower.find("undo remove") != string::npos) {
        Song s;
        if (popUndo(s)) {
            addSong(s);
            enqueueSong(s);
            bstRoot = insertBST(bstRoot, s);
            cout << "Undo successful.\n";
        } else cout << "Nothing to undo.\n";
    }
    else if (lower == "7" || lower == "save" || lower.find("save csv") != string::npos) {
        myPlaylist.saveToExcel();
    }
    else if (lower == "8" || lower == "sort") {
        vector<Song*> songList;
        for (Song* cur = myPlaylist.begin(); cur; cur = cur->next) songList.push_back(cur);
        if (songList.empty()) { cout << "Playlist empty.\n"; return; }
        cout << "Sort by:\n1. Title\n2. Artist\n3. Duration\nChoice: ";
        int sortKey; cin >> sortKey; cin.ignore();
        bool (*compare)(Song*, Song*) = nullptr;
        string criterion;
        if (sortKey == 1) { compare = compareByTitle; criterion = "Title"; }
        else if (sortKey == 2) { compare = compareByArtist; criterion = "Artist"; }
        else { compare = compareByDuration; criterion = "Duration"; }
        cout << "Choose algorithm:\n1. Merge Sort\n2. Quick Sort\n3. Insertion Sort\nChoice: ";
        int algo; cin >> algo; cin.ignore();
        if (algo == 1) mergeSort(songList, 0, songList.size()-1, compare);
        else if (algo == 2) quickSort(songList, 0, songList.size()-1, compare);
        else insertionSort(songList, compare);
        displaySortedSongs(songList, criterion);
    }
    else if (lower == "9" || lower == "binary" || lower.find("binary search") != string::npos) {
        vector<Song*> songList;
        for (Song* cur = myPlaylist.begin(); cur; cur = cur->next) songList.push_back(cur);
        if (songList.empty()) { cout << "Playlist empty.\n"; return; }
        mergeSort(songList, 0, songList.size()-1, compareByTitle);
        cout << "Enter title to binary search: ";
        string title; getline(cin, title);
        int idx = binarySearch(songList, title);
        if (idx != -1) songList[idx]->display();
        else cout << "Not found.\n";
    }
    else if (lower == "10" || lower == "linear" || lower.find("linear search") != string::npos) {
        vector<Song*> songList;
        for (Song* cur = myPlaylist.begin(); cur; cur = cur->next) songList.push_back(cur);
        if (songList.empty()) { cout << "Playlist empty.\n"; return; }
        cout << "Enter title to linear search: ";
        string title; getline(cin, title);
        int idx = linearSearch(songList, title);
        if (idx != -1) songList[idx]->display();
        else cout << "Not found.\n";
    }
    // NEW FEATURES:
    else if (lower == "11" || lower == "search artist" || lower.find("by artist") != string::npos) {
        cout << "Enter artist name: ";
        string artist; getline(cin, artist);
        myPlaylist.searchByArtist(artist);
    }
    else if (lower == "12" || lower == "search genre" || lower.find("by genre") != string::npos) {
        cout << "Enter genre: ";
        string genre; getline(cin, genre);
        myPlaylist.searchByGenre(genre);
    }
    else if (lower == "13" || lower == "edit" || lower.find("edit song") != string::npos) {
        cout << "Enter title of song to edit: ";
        string title; getline(cin, title);
        if (!myPlaylist.editSong(title))
            cout << "Song not found.\n";
        // Note: BST is not updated if title changes; for simplicity we ignore BST update.
        // In a full implementation you'd remove and re-insert.
    }
    else if (lower == "14" || lower == "load" || lower.find("load csv") != string::npos) {
        cout << "Enter CSV filename to load (e.g., MyPlaylist.csv): ";
        string fname; getline(cin, fname);
        if (myPlaylist.loadFromCSV(fname))
            cout << "Playlist loaded from " << fname << endl;
        else
            cout << "Failed to load file.\n";
    }
    else if (lower == "15" || lower == "benchmark" || lower.find("compare sorts") != string::npos) {
        benchmarkSorts();
    }
    else if (lower == "16" || lower == "remove title" || lower.find("remove by title") != string::npos) {
        cout << "Enter song title to remove: ";
        string title; getline(cin, title);
        if (myPlaylist.removeSongByTitle(title))
            cout << "Removed: " << title << endl;
        else
            cout << "Song not found.\n";
    }
    else if (lower == "help" || lower == "?") {
        cout << "\n===== COMMAND LIST =====\n";
        cout << "1 / add song          - Add a new song\n";
        cout << "2 / remove last       - Remove the last song\n";
        cout << "3 / display           - Show all songs\n";
        cout << "4 / play next         - Play next in queue\n";
        cout << "5 / search            - Search by title (BST)\n";
        cout << "6 / undo              - Undo last removal\n";
        cout << "7 / save              - Save playlist to CSV\n";
        cout << "8 / sort              - Sort playlist (choose algorithm)\n";
        cout << "9 / binary search     - Binary search by title (requires sort)\n";
        cout << "10 / linear search    - Linear search by title\n";
        cout << "11 / search artist    - Find songs by artist\n";
        cout << "12 / search genre     - Find songs by genre\n";
        cout << "13 / edit song        - Edit a song's details\n";
        cout << "14 / load CSV         - Load playlist from CSV file\n";
        cout << "15 / benchmark        - Compare sorting speeds (Merge vs Quick vs Insertion)\n";
        cout << "16 / remove title     - Remove a song by its title\n";
        cout << "bye / 0               - Exit program\n";
    }
    else if (lower == "bye" || lower == "0" || lower == "exit" || lower == "quit") {
        cout << "Goodbye!\n";
        exit(0);
    }
    else {
        cout << "Unknown command. Type 'help'.\n";
    }
}

// ==================== MAIN ====================
int main() {
    cout << "========================================\n";
    cout << "   YouTube Music Playlist Manager\n";
    cout << "   Type a number OR a command\n";
    cout << "   Type 'help or ? ' for all options\n";
    cout << "========================================\n";
    string input;
    while (true) {
        cout << "\n> ";
        getline(cin, input);
        processCommand(input);
    }
    return 0;
}
