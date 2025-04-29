// ============= ProtoType ===============
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <ctime>
using namespace std;

// ====================== Song Class ======================
class Song
{
private:
    string title;
    string artist;
    string genre;
    int duration; // in seconds
    string youtubeLink;

public:
    Song(string t, string a, string g, int d, string yt)
        : title(t), artist(a), genre(g), duration(d), youtubeLink(yt) {}

    // Getters
    string getTitle() const { return title; }
    string getArtist() const { return artist; }
    string getGenre() const { return genre; }
    int getDuration() const { return duration; }
    string getLink() const { return youtubeLink; }

    // Format duration as MM:SS
    string getFormattedDuration() const
    {
        int mins = duration / 60;
        int secs = duration % 60;
        return to_string(mins) + ":" + (secs < 10 ? "0" : "") + to_string(secs);
    }

    // Display song info
    void display() const
    {
        cout << "\n"
             << title << " by " << artist
             << "\nGenre: " << genre
             << "\nDuration: " << getFormattedDuration()
             << "\nYouTube: " << youtubeLink << "\n";
    }
};

// ====================== Playlist Class ======================
class Playlist
{
private:
    string name;
    vector<Song> songs;
    vector<string> activityLog;

    // Helper function to validate YouTube URL
    bool isValidYouTubeLink(const string &url)
    {
        return (url.find("youtube.com") != string::npos ||
                url.find("youtu.be") != string::npos);
    }

public:
    Playlist(string n) : name(n) {}

    bool isEmpty() const
    {
        return songs.empty();
    }

    void addSong(const Song &song)
    {
        if (!isValidYouTubeLink(song.getLink()))
        {
            cout << "Invalid YouTube URL! Must contain 'youtube.com' or 'youtu.be'\n";
            return;
        }
        songs.push_back(song);
        logActivity("Added: " + song.getTitle() + " (" + song.getLink() + ")");
        cout << "Song added successfully!\n";
    }

    void removeSong(int index)
    {
        if (index >= 0 && index < songs.size())
        {
            logActivity("Removed: " + songs[index].getTitle());
            songs.erase(songs.begin() + index);
            cout << "Song removed successfully!\n";
        }
        else
        {
            cout << "Invalid song number!\n";
        }
    }

    void displayAll() const
    {
        cout << "\n===== " << name << " (" << songs.size() << " songs) =====";
        if (songs.empty())
        {
            cout << "\nPlaylist is empty!\n";
            return;
        }
        for (size_t i = 0; i < songs.size(); i++)
        {
            cout << "\n"
                 << i + 1 << ".";
            songs[i].display();
        }
    }

    void saveToExcel()
    {
        string filename = name + ".csv";
        ofstream excelFile(filename);

        if (!excelFile)
        {
            cerr << "Error creating file!" << endl;
            return;
        }

        // Header with timestamp
        time_t now = time(0);
        excelFile << "Playlist: " << name << "," << ctime(&now);
        excelFile << "Title,Artist,Genre,Duration,YouTube Link\n";

        // Song data
        for (const auto &song : songs)
        {
            excelFile << "\"" << song.getTitle() << "\","
                      << "\"" << song.getArtist() << "\","
                      << "\"" << song.getGenre() << "\","
                      << "\"" << song.getFormattedDuration() << "\","
                      << "\"" << song.getLink() << "\"\n";
        }

        // Activity log
        excelFile << "\nActivity Log\n";
        for (const auto &entry : activityLog)
        {
            excelFile << entry << "\n";
        }

        excelFile.close();
        cout << "\nPlaylist saved to '" << filename << "'\n";
    }

private:
    void logActivity(const string &action)
    {
        time_t now = time(0);
        char timestamp[20];
        strftime(timestamp, 20, "%H:%M:%S", localtime(&now));
        activityLog.push_back(string(timestamp) + " - " + action);
    }
};

// ====================== Main Program ======================
void displayMenu()
{
    cout << "\n===== MUSIC PLAYLIST GENERATOR ====="
         << "\n1. Add Song"
         << "\n2. Remove Song"
         << "\n3. View Playlist"
         << "\n4. Save to Excel"
         << "\n5. Exit"
         << "\nEnter choice: ";
}

int main()
{
    cout << "=== YouTube Music Playlist Manager ===" << endl;

    // Create playlist
    string playlistName;
    cout << "Enter playlist name: ";
    getline(cin, playlistName);
    Playlist myPlaylist(playlistName);

    while (true)
    {
        displayMenu();
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

    return 0;
}