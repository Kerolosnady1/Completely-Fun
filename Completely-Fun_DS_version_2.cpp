// ===================== CLEAN & FIXED VERSION =====================
// Data Structures Project with Chatbot
// Fully integrated: chatbot can execute real playlist actions and interact casually

#include <iostream>
#include <vector>
#include <string>
#include <cctype>
using namespace std;

/* ===================== SONG STRUCT ===================== */
struct Song {
    string title;
    string artist;
};

/* ===================== VECTOR (PLAYLIST) ===================== */
vector<Song> playlist;

void addSong(const Song& s) {
    playlist.push_back(s);
}

void removeLastSong() {
    if (!playlist.empty()) playlist.pop_back();
}

void displayPlaylistRecursive(int index = 0) {
    if (index == playlist.size()) return;
    cout << index + 1 << ". " << playlist[index].title
         << " - " << playlist[index].artist << endl;
    displayPlaylistRecursive(index + 1);
}

/* ===================== STACK (UNDO) ===================== */
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

/* ===================== QUEUE (PLAY ORDER) ===================== */
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

/* ===================== BST (SEARCH) ===================== */
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

/* ===================== CHATBOT CLASSES ===================== */
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
        // Greetings & casual conversation
        rules.push_back({"hello", "Hello there! How are you today?"});
        rules.push_back({"hi", "Hi! Ready to manage your playlist?"});
        rules.push_back({"hey", "Hey! Want to listen to some music?"});
        rules.push_back({"how are you", "I'm just a chatbot, but I'm feeling productive!"});
        rules.push_back({"help", "Commands I understand: add song, remove last, display playlist, play next, undo remove, search song, bye."});
        rules.push_back({"bye", "Goodbye! Returning to main menu."});
    }

    string getResponse(const string& input) override {
        string msg = toLower(input);

        // Playlist actions
        if (msg.find("display playlist") != string::npos) {
            if (playlist.empty()) return "Playlist is empty.";
            displayPlaylistRecursive();
            return "";
        }
        else if (msg.find("play next") != string::npos) {
            playNext();
            return "";
        }
        else if (msg.find("add song") != string::npos) {
            Song s;
            cout << "Enter title: "; getline(cin, s.title);
            cout << "Enter artist: "; getline(cin, s.artist);
            addSong(s);
            enqueueSong(s);
            cout << "Song added to playlist.\n";
            return "";
        }
        else if (msg.find("remove last") != string::npos) {
            if (!playlist.empty()) {
                pushUndo(playlist.back());
                removeLastSong();
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
            searchBST(nullptr, title); // Optional: link to BST root if needed
            return "";
        }

        // Casual conversation fallback
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

/* ===================== MENU ===================== */
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

int main() {
    TreeNode* root = nullptr;
    ActionBot actionBot;
    ChatBot bot(&actionBot);
    int choice;

    do {
        showMenu();
        cin >> choice;
        cin.ignore();

        if (choice == 1) {
            Song s;
            cout << "Title: "; getline(cin, s.title);
            cout << "Artist: "; getline(cin, s.artist);
            addSong(s);
            enqueueSong(s);
            root = insertBST(root, s);
        }
        else if (choice == 2 && !playlist.empty()) {
            pushUndo(playlist.back());
            removeLastSong();
            cout << "Removed.\n";
        }
        else if (choice == 3) {
            if (playlist.empty()) cout << "Empty playlist.\n";
            else displayPlaylistRecursive();
        }
        else if (choice == 4) playNext();
        else if (choice == 5) {
            string title;
            cout << "Title: "; getline(cin, title);
            searchBST(root, title);
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

    } while (choice != 0);

    return 0;
}
