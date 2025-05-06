#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <ctime>
using namespace std;

class Song
{
private:
    string title;
    string artist;
    string genre;
    int duration;
    string youtubeLink;

public:
    // Constructor
    Song(string t, string a, string g, int d, string yt)
    {
        title = t;
        artist = a;
        genre = g;
        setDuration(d);
        setLink(yt);
    }

    // Setter functions
    void setTitle(string t)
    {
        title = t;
    }

    void setArtist(string a)
    {
        artist = a;
    }

    void setGenre(string g)
    {
        genre = g;
    }

    void setDuration(int d)
    {
        if (d < 0)
        {
            cout << "Error: Duration must be positive!" << endl;
            return;
        }
        duration = d;
    }

    void setLink(string yt)
    {
        if (yt.find("https://") == string::npos)
        {
            cout << "Error: YouTube link must start with 'https://'" << endl;
            return;
        }
        youtubeLink = yt;
    }

    // Getter functions
    string getTitle() const
    {
        return title;
    }

    string getArtist() const
    {
        return artist;
    }

    string getGenre() const
    {
        return genre;
    }

    int getDuration() const
    {
        return duration;
    }

    string getLink() const
    {
        return youtubeLink;
    }

    // Duration in MM:SS format
    string getFormattedDuration() const
    {
        int minutes = duration / 60;
        int seconds = duration % 60;

        string formatted = to_string(minutes) + ":";
        if (seconds < 10)
            formatted += "0";
        formatted += to_string(seconds);

        return formatted;
    }

    // Display song info
    void display() const
    {
        cout << endl;
        cout << title << " by " << artist << endl;
        cout << "Genre: " << genre << endl;
        cout << "Duration: " << getFormattedDuration() << endl;
        cout << "YouTube: " << youtubeLink << endl;
    }
};
class Playlist
{
private:
    string name;
    vector<Song> songs;
    vector<string> activityLog;

    bool isValidYouTubeLink(const std::string &url)
    {
        if (url.find("youtube.com") != std::string::npos)
        {
            return true;
        }
        else if (url.find("youtu.be") != std::string::npos)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

public:
    Playlist(string n)
    {
        name = n;
    }

    bool isEmpty() const
    {
        return songs.empty();
    }

    void addSong(const Song &song)
    {
        if (!isValidYouTubeLink(song.getLink()))
        {
            cout << "Invalid YouTube URL! Must contain 'youtube.com' or 'youtu.be'" << endl;
            return;
        }
        songs.push_back(song);
        logActivity("Added: " + song.getTitle() + " (" + song.getLink() + ")");
        cout << "Song added successfully!" << endl;
    }

    void removeSong(int i)
    {
        if (i >= 0 && i < songs.size())
        {
            logActivity("Removed: " + songs[i].getTitle());
            songs.erase(songs.begin() + i);
            cout << "Song removed" << endl;
        }
        else
        {
            cout << "Invalid song number!" << endl;
        }
    }
    void displayAll() const
    {
        cout << "===== Playlist: " << name << " =====" << endl;
        if (songs.empty())
        {
            cout << "Playlist is empty." << endl;
            return;
        }
        for (int i = 0; i < songs.size(); i++)
        {
            cout << "Song -> " << i + 1 << ":" << endl;
            songs[i].display();
        }
    }

    void saveToExcel()
    {
        ofstream file(name + ".csv");

        if (!file)
        {
            cout << "Failed to create file." << endl;
            return;
        }

        file << "Title,Artist,Genre,Duration,YouTube Link\n";
        for (const Song &song : songs)
        {
            file << "\"" << song.getTitle() << "\","
                 << "\"" << song.getArtist() << "\","
                 << "\"" << song.getGenre() << "\","
                 << "\"" << song.getFormattedDuration() << "\","
                 << "\"" << song.getLink() << "\"\n";
        }

        file << "Activity Log:" << endl;
        for (const string &log : activityLog)
        {
            file << log << endl;
        }

        file.close();
        cout << "Playlist saved as ExcelSheet." << endl;
    }

    void logActivity(const string &action)
    {
        time_t now = time(0);
        char buffer[100];
        ctime_s(buffer, sizeof(buffer), &now);
        string timeStr(buffer);
        timeStr.pop_back();
        activityLog.push_back(timeStr + " - " + action);
    }
};

void displayMenu()
{
    cout << "==== = MUSIC PLAYLIST GENERATOR ===== " << endl;
    cout << "1. Add Song" << endl;
    cout << "2. Remove Song" << endl;
    cout << "3. View Playlist" << endl;
    cout << "4. Save to Excel" << endl;
    cout << "5. Exit" << endl;
    cout << "Enter choice: " << endl;
}

int manualFind(const string &haystack, const string &needle)
{
    if (needle.empty())
        return 0;

    for (unsigned int i = 0; i <= haystack.length() - needle.length(); ++i)
    {
        bool match = true;
        for (unsigned int j = 0; j < needle.length(); ++j)
        {
            if (haystack[i + j] != needle[j])
            {
                match = false;
                break;
            }
        }
        if (match)
            return i;
    }

    return -1; // Not found
}

string manualSubstr(const string &str, unsigned int start, unsigned int length)
{
    string result = "";

    if (start >= str.length())
        return result;

    for (unsigned int i = start; i < start + length && i < str.length(); ++i)
    {
        result += str[i];
    }

    return result;
}
// ====================== Response System ======================
class ResponseSystem
{
private:
    vector<string> keywords;
    vector<string> responses;

public:
    ResponseSystem()
    {
        // Initialize with some default responses
        keywords = {"sad", "happy", "angry", "school", "love", "bye"};
        responses = {
            "Tell me more about your feelings...",
            "I'm glad you're happy! :)",
            "Take deep breaths. What's wrong?",
            "Education is important. What's troubling you?",
            "Love is a beautiful thing!",
            "Goodbye! It was nice talking to you."};
    }

    string getResponse(const string &keyword)
    {
        string lowerKeyword = toLower(keyword);
        for (size_t i = 0; i < keywords.size(); i++)
        {
            if (lowerKeyword.find(toLower(keywords[i])) != string::npos)
            {
                return responses[i];
            }
        }
        return "I'm not sure how to respond to that. Can you elaborate?";
    }

    string toLower(const string &input)
    {
        string lower = input;
        transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        return lower;
    }
};

string manualSubstr(const string &str, unsigned int start, size_t length)
{
    string result = "";

    if (start >= str.size())
        return result;

    for (unsigned int i = start; i < start + length && i < str.size(); ++i)
    {
        result += str[i];
    }

    return result;
}
bool isInteger(const string &str)
{
    if (str.empty())
        return false;
    for (char c : str)
    {
        if (!isdigit(c))
            return false;
    }
    return true;
}

class ChatBot
{
protected:
    string userName;
    ResponseSystem responder;
    vector<string> chatHistory;

public:
    ChatBot(const string &botName) : userName(botName) {}

    void greet()
    {
        string greeting = userName + ": Hello! I'm " + userName + ". How are you feeling today?";
        cout << greeting << endl;
        chatHistory.push_back(greeting);
    }

    virtual string reply(const string &input)
    {
        string response = responder.getResponse(input);
        string fullResponse = userName + ": " + response;
        cout << fullResponse << endl;
        chatHistory.push_back("User: " + input);
        chatHistory.push_back(fullResponse);
        return response;
    }

    void saveToExcel(const string &filename)
    {
        ofstream excelFile(filename);
        if (!excelFile)
        {
            cout << "Error creating file!" << endl;
            return;
        }

        time_t now = time(0);
        excelFile << "Chat Session," << ctime(&now) << endl;

        excelFile << "Speaker,Message" << endl;

        for (int i = 0; i < chatHistory.size(); ++i)
        {
            string message = chatHistory[i];
            unsigned int colonPos = manualFind(message, ":");
            string speaker = manualSubstr(message, 0, colonPos);
            string msg = message.substr(colonPos + 2);
            excelFile << speaker << "," << msg << endl;
        }

        excelFile.close();
        cout << "Chat saved to " << filename << endl;
    }
};

// ====================== AngryBot ======================
class AngryBot : public ChatBot
{
public:
    AngryBot(const string &botName) : ChatBot(botName) {}

    string reply(const string &input) override
    {
        string baseResponse = responder.getResponse(input);
        string angryResponse = userName + ": Ugh. " + baseResponse + " Stop complaining!";
        cout << angryResponse << endl;
        chatHistory.push_back("User: " + input);
        chatHistory.push_back(angryResponse);
        return angryResponse;
    }
};

void runMusicPlaylistManager()
{
    cout << "=== YouTube Music Playlist Manager ===" << endl;

    // Create playlist
    string playlistName;
    cout << "Enter playlist name: ";
    getline(cin, playlistName);
    Playlist myPlaylist(playlistName);

    while (true)
    {
        cout << "\n===== MUSIC PLAYLIST GENERATOR ====="
             << "\n1. Add Song"
             << "\n2. Remove Song"
             << "\n3. View Playlist"
             << "\n4. Save to Excel"
             << "\n5. Exit to Main Menu"
             << "\nEnter choice: ";

        int choice;
        cin >> choice;
        cin.ignore(); // Clear newline

        if (choice == 5)
            break;

        switch (choice)
        {
        case 1:
        {
            string title, artist, genre, ytLink;
            int mins, secs;

            cout << "\nEnter Song Details:\n";
            cout << "Title: ";
            getline(cin, title);
            cout << "Artist: ";
            getline(cin, artist);
            cout << "Genre: ";
            getline(cin, genre);
            cout << "YouTube Link: ";
            getline(cin, ytLink);
            cout << "Duration (minutes seconds): ";
            cin >> mins >> secs;
            cin.ignore();

            myPlaylist.addSong(Song(title, artist, genre, mins * 60 + secs, ytLink));
            break;
        }
        case 2:
        {
            int index;
            myPlaylist.displayAll();
            if (!myPlaylist.isEmpty())
            {
                cout << "\nEnter song number to remove (0 to cancel): ";
                cin >> index;
                cin.ignore();
                if (index > 0)
                    myPlaylist.removeSong(index - 1);
            }
            break;
        }
        case 3:
            myPlaylist.displayAll();
            break;
        case 4:
            myPlaylist.saveToExcel();
            break;
        default:
            cout << "Invalid choice!\n";
        }
    }
}

// ====================== Project 2 Functions ======================
void runChatBot()
{
    string userName;
    cout << "Enter your name: ";
    getline(cin, userName);

    // Create bot (switch between ChatBot and AngryBot)
    ChatBot *bot = new ChatBot("Therapy");
    // ChatBot* bot = new AngryBot("Grumpy");

    bot->greet();

    while (true)
    {
        string input;
        cout << userName << ": ";
        getline(cin, input);

        if (input == "bye")
        {
            bot->reply(input);
            break;
        }

        bot->reply(input);
    }

    // Save to Excel (CSV format)
    bot->saveToExcel("chat_history.csv");

    delete bot;
}

// ====================== Main Program ======================
int main()
{
    while (true)
    {
        cout << "\n===== MAIN MENU ====="
             << "\n1. Music Playlist Manager"
             << "\n2. ChatBot Program"
             << "\n3. Exit"
             << "\nEnter your choice (1-3): ";

        int choice;
        cin >> choice;
        cin.ignore(); // Clear newline

        switch (choice)
        {
        case 1:
            runMusicPlaylistManager();
            break;
        case 2:
            runChatBot();
            break;
        case 3:
            cout << "Exiting program. Goodbye!" << endl;
            return 0;
        default:
            cout << "Invalid choice. Please try again." << endl;
        }
    }
}