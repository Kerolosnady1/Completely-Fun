#include <iostream>
#include <vector>
#include <string>
#include <cctype>
#include <fstream>
#include <ctime>
using namespace std;


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

struct Song {
    string title, artist, genre, youtubelink;
    int duration;
    Song* next;
    Song* prev;
    Song(string t = "", string a = "", string g = "", int d = 0, string yt = "") :
        title(t), artist(a), genre(g), youtubelink(yt), duration(d >= 0 ? d : 0), next(nullptr), prev(nullptr) {}

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

class PlaylistManager {
private:
    string name;
    Song* head;
    Song* tail;
    DoublyLinkedList<string> activitylog;
    int count;

    bool isValidyoutubelink(const string& url) {

        string lowerUrl = url;
        for(char &c : lowerUrl) c = tolower(c);
        return (lowerUrl.find("youtube.com") != string::npos ||
                lowerUrl.find("youtu.be") != string::npos);
    }

public:
    PlaylistManager(string n = "My Playlist") {
        name = n;
        head = tail = nullptr;
        count = 0;
    }

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
            cout << "Song -> " << i << ":\n";
            cur->display();
            cur = cur->next;
            i++;
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

    Song* getBack() { return tail; }
    int size() const { return count; }
};

PlaylistManager myPlaylist("FunPlaylist");

void addSong(const Song& s) {
    Song* newS = new Song(s.title, s.artist, s.genre, s.duration, s.youtubelink);
    myPlaylist.Addsong(newS);
}



void displayPlaylistRecursive(Song* node, int index = 1) {
    if (!node) return;
    cout << index << ". " << node->title << " - " << node->artist << endl;
    displayPlaylistRecursive(node->next, index + 1);
}

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

struct TreeNode {
    Song song;
    TreeNode* left;
    TreeNode* right;
};

TreeNode* insertBST(TreeNode* root, const Song& s) {
    if (!root) return new TreeNode{s, nullptr, nullptr};
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
    if (root->song.title == title)
        cout << "Found: " << root->song.artist << endl;
    else if (title < root->song.title)
        searchBST(root->left, title);
    else
        searchBST(root->right, title);
}

class ResponseSystem {
protected:
    vector<pair<string,string>> rules;

    string toLower(string s) {
        for (char &c : s) c = tolower(c);
        return s;
    }

public:
    virtual string getResponse(const string& input) {
        string msg = toLower(input);
        for (auto &r : rules) {
            if (msg.find(r.first) != string::npos)
                return r.second;
        }
        return "I am listening. Ask me about songs, playlist, or type help.";
    }
};

class ActionBot : public ResponseSystem {
public:
    ActionBot() {

        rules.push_back({"hello", "Hello there! How are you today?"});
        rules.push_back({"hi", "Hi! Ready to manage your playlist?"});
        rules.push_back({"hey", "Hey! Want to listen to some music?"});
        rules.push_back({"how are you", "I'm just a chatbot, but I'm feeling productive!"});
        rules.push_back({"help", "Commands I understand: add song, remove last, display playlist, play next, undo remove, search song, bye."});
        rules.push_back({"bye", "Goodbye! Returning to main menu."});
    }

    string getResponse(const string& input) override {
        string msg = toLower(input);


        if (msg.find("display playlist") != string::npos) {
            if (myPlaylist.isempty()) return "Playlist is empty.";
            myPlaylist.displayAll();
            return "";
        }
        else if (msg.find("play next") != string::npos) {
            playNext();
            return "";
        }
        else if (msg.find("add song") != string::npos) {
            string title, artist, genre, yt;
            int duration;
            cout << "Enter title: "; getline(cin, title);
            cout << "Enter artist: "; getline(cin, artist);
            cout << "Enter genre: "; getline(cin, genre);
            cout << "Enter duration (sec): "; cin >> duration; cin.ignore(1000, '\n');
            cout << "Enter YouTube link: "; getline(cin, yt);
            Song s(title, artist, genre, duration, yt);
            addSong(s);
            enqueueSong(s);
            return "";
        }
        else if (msg.find("remove last") != string::npos) {
            if (!myPlaylist.isempty()) {
                pushUndo(*myPlaylist.getBack());
                myPlaylist.removeLastSong();
                return "Removed last song.";
            } else return "Playlist is empty.";
        }
        else if (msg.find("undo remove") != string::npos) {
            Song s;
            if (popUndo(s)) {
                addSong(s);
                enqueueSong(s);
                return "Undo done.";
            } else return "Nothing to undo.";
        }
        else if (msg.find("search song") != string::npos) {
            string title;
            cout << "Enter title to search: "; getline(cin, title);

            extern TreeNode* bstRoot;
            searchBST(bstRoot, title);
            return "";
        }

        for (auto &r : rules) {
            if (msg.find(r.first) != string::npos)
                return r.second;
        }

        return "I am listening. Ask me about songs, playlist, or type help.";
    }
};

class ChatBot {
    ResponseSystem* system;
public:
    ChatBot(ResponseSystem* s) : system(s) {}

    void chat() {
        string input;
        cout << "Chatbot active (type 'bye' to exit)\n";
        while (true) {
            cout << "> ";
            getline(cin, input);

            string lowerInput = input;
            for (char &c : lowerInput) c = tolower(c);

            if (lowerInput.find("bye") != string::npos) {
                cout << system->getResponse(input) << endl;
                break;
            }

            string response = system->getResponse(input);
            if (!response.empty())
                cout << response << endl;
        }
    }
};

void showMenu() {
    cout << "\n1. Add Song"
         << "\n2. Remove Last Song"
         << "\n3. Display Playlist"
         << "\n4. Play Next"
         << "\n5. Search Song"
         << "\n6. Undo Remove"
         << "\n7. Chatbot"
         << "\n0. Exit"
         << "\nChoice: ";
}

TreeNode* bstRoot = nullptr;

int main() {
    ActionBot actionBot;
    ChatBot bot(&actionBot);
    int choice;

    do {
        showMenu();
        cout << "8. Save (CSV)\nChoice: ";
        cin >> choice;
        cin.ignore(1000, '\n');

        if (choice == 1) {
            string title, artist, genre, yt;
            int duration;
            cout << "Title: "; getline(cin, title);
            cout << "Artist: "; getline(cin, artist);
            cout << "Genre: "; getline(cin, genre);
            cout << "Duration (sec): "; cin >> duration; cin.ignore(1000, '\n');
            cout << "YouTube Link: "; getline(cin, yt);
            Song s(title, artist, genre, duration, yt);
            addSong(s);
            enqueueSong(s);
            bstRoot = insertBST(bstRoot, s);
        }
        else if (choice == 2 && !myPlaylist.isempty()) {
            pushUndo(*myPlaylist.getBack());
            myPlaylist.removeLastSong();
            cout << "Removed.\n";
        }
        else if (choice == 3) {
            if (myPlaylist.isempty()) cout << "Empty playlist.\n";
            else myPlaylist.displayAll();
        }
        else if (choice == 4) playNext();
        else if (choice == 5) {
            string title;
            cout << "Title: "; getline(cin, title);
            searchBST(bstRoot, title);
        }
        else if (choice == 6) {
            Song s;
            if (popUndo(s)) {
                addSong(s);
                enqueueSong(s);
                cout << "Undo done.\n";
            } else cout << "Nothing to undo.\n";
        }
        else if (choice == 7) bot.chat();
        else if (choice == 8) myPlaylist.saveToExcel();

    } while (choice != 0);

    return 0;
}
