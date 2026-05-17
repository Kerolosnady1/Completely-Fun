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
// SONG
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
// PLAYLIST MANAGER
// =========================

class PlaylistManager {

private:

    string name;

    vector<Song> songs;
    vector<string> logs;

    queue<Song> playQueue;
    stack<Song> undoStack;

    bool validYoutube(const string& url) {
        return url.find("youtube.com") != string::npos ||
            url.find("youtu.be") != string::npos;
    }

    // =========================
    // QUICK SORT (PDF SYNTAX)
    // =========================

    int partition(int low, int high) {

        Song pivot = songs[high];
        int i = (low - 1);

        for (int j = low; j < high; j++) {

            if (songs[j].title < pivot.title) {
                i++;
                swap(songs[i], songs[j]);
            }
        }

        swap(songs[i + 1], songs[high]);
        return (i + 1);
    }

    void quickSort(int low, int high) {

        if (low < high) {

            int pi = partition(low, high);

            quickSort(low, pi - 1);
            quickSort(pi + 1, high);
        }
    }

    void sortSongs() {

        if (!songs.empty())
            quickSort(0, songs.size() - 1);
    }

public:

    PlaylistManager(string n = "Playlist") : name(n) {}

    // =========================
    // LOG
    // =========================

    void logActivity(const string& action) {
        time_t now = time(0);
        tm t;

        localtime_s(&t, &now);

        stringstream ss;
        ss << put_time(&t, "%Y-%m-%d %H:%M:%S");

        logs.push_back(ss.str() + " - " + action);
    }

    // =========================
    // ADD SONG
    // =========================

    void addSong(const Song& s) {

        if (!validYoutube(s.youtubeLink)) {
            cout << "Invalid YouTube Link\n";
            return;
        }

        songs.push_back(s);
        playQueue.push(s);

        logActivity("Added: " + s.title);

        cout << "Song Added\n";
    }

    // =========================
    // REMOVE LAST
    // =========================

    void removeLastSong() {

        if (songs.empty()) {
            cout << "Empty Playlist\n";
            return;
        }

        Song last = songs.back();

        undoStack.push(last);
        songs.pop_back();

        logActivity("Removed: " + last.title);

        cout << "Removed Last Song\n";
    }

    // =========================
    // DISPLAY
    // =========================

    void displayAll() const {

        if (songs.empty()) {
            cout << "Empty Playlist\n";
            return;
        }

        int i = 1;

        for (const auto& s : songs) {
            cout << i++ << ".\n";
            s.display();
        }
    }

    // =========================
    // PLAY NEXT
    // =========================

    void playNext() {

        if (playQueue.empty()) {
            cout << "Queue Empty\n";
            return;
        }

        cout << "Now Playing: "
            << playQueue.front().title << "\n";

        playQueue.pop();
    }

    // =========================
    // UNDO
    // =========================

    void undoRemove() {

        if (undoStack.empty()) {
            cout << "Nothing To Undo\n";
            return;
        }

        songs.push_back(undoStack.top());
        undoStack.pop();

        cout << "Undo Done\n";
    }

    // =========================
    // BINARY SEARCH
    // =========================

    void searchSong(const string& title) {

        sortSongs();

        int l = 0, r = songs.size() - 1;

        while (l <= r) {

            int mid = (l + r) / 2;

            if (songs[mid].title == title) {
                cout << "Found Song\n";
                songs[mid].display();
                return;
            }

            if (songs[mid].title < title)
                l = mid + 1;
            else
                r = mid - 1;
        }

        cout << "Song Not Found\n";
    }

    // =========================
    // SORT
    // =========================

    void sortPlaylist() {
        sortSongs();
        cout << "Playlist Sorted\n";
    }

    // =========================
    // SAVE CSV
    // =========================

    void saveCSV() {

        ofstream file(name + ".csv");

        file << "Title,Artist,Genre,Duration,Youtube\n";

        for (auto& s : songs) {
            file << s.title << ","
                << s.artist << ","
                << s.genre << ","
                << s.duration << ","
                << s.youtubeLink << "\n";
        }

        file << "\nLogs\n";

        for (auto& l : logs) {
            file << l << "\n";
        }

        cout << "Saved Successfully\n";
    }
};

// =========================
// CHATBOT
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
            }
            else if (input == "hello") {
                cout << "Hello User\n";
            }
            else if (input == "help") {
                cout << "Commands: add, remove, play, search\n";
            }
            else {
                cout << "I am listening...\n";
            }
        }
    }
};

// =========================
// GLOBAL
// =========================

PlaylistManager playlist("FunPlaylist");

// =========================
// MENU
// =========================

void showMenu() {

    cout << "\n1. Add Song";
    cout << "\n2. Remove Last";
    cout << "\n3. Display";
    cout << "\n4. Play Next";
    cout << "\n5. Search";
    cout << "\n6. Undo";
    cout << "\n7. Sort";
    cout << "\n8. Save CSV";
    cout << "\n9. ChatBot";
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

            cout << "Title: ";
            getline(cin, t);

            cout << "Artist: ";
            getline(cin, a);

            cout << "Genre: ";
            getline(cin, g);

            cout << "Duration: ";
            cin >> d;
            cin.ignore();

            cout << "YouTube: ";
            getline(cin, yt);

            playlist.addSong(Song(t, a, g, d, yt));
        }

        else if (choice == 2) playlist.removeLastSong();
        else if (choice == 3) playlist.displayAll();
        else if (choice == 4) playlist.playNext();
        else if (choice == 5) {

            string t;
            cout << "Enter Title: ";
            getline(cin, t);

            playlist.searchSong(t);
        }
        else if (choice == 6) playlist.undoRemove();
        else if (choice == 7) playlist.sortPlaylist();
        else if (choice == 8) playlist.saveCSV();
        else if (choice == 9) bot.start();

    } while (choice != 0);

    return 0;
}

