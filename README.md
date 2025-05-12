# 🎵 YouTube Music Playlist & ChatBot Manager (C++)

A console-based C++ application that provides two core functionalities:

1. **Music Playlist Manager** – Build, view, and export your custom YouTube playlists.
2. **Emotion-Responsive ChatBot** – Interact with a basic emotional chatbot or its grumpy variant.

---

## 🧠 Features

### 🎶 Music Playlist Manager
- Add songs with title, artist, genre, duration, and YouTube link
- Validate YouTube links (`youtube.com` or `youtu.be`)
- Export playlists and logs to **CSV files** compatible with Excel
- Remove or view songs in a user-friendly menu
- Logs all actions with timestamps

### 🤖 ChatBot Interface
- Text-based chatbot with emotion-aware keyword detection
- Switch between friendly and angry bot personalities
- Conversation history saved in CSV format
- Recognizes keywords like `sad`, `happy`, `angry`, `school`, `love`, and `bye`

---

## 💡 How It Works

The application is menu-driven with a simple CLI interface. Upon launch, users can choose between:
1. Managing a **YouTube playlist**
2. Chatting with a **ChatBot**
3. Exiting the application

### Data Persistence
- **Playlist**: Saved to `your_playlist_name.csv`
- **ChatBot**: Saved to `chat_history.csv`

Both files include headers for use with Excel or Google Sheets.

---

## 🛠️ Technologies Used

- **Language**: C++17
- **Libraries**:
  - `<iostream>`, `<vector>`, `<string>`, `<fstream>`, `<ctime>`, `<algorithm>`
- **File Format**: `.csv` for data export

---

## 📦 File Structure

📁 Project Root
├── main.cpp # Complete program with playlist and chatbot logic
├── chat_history.csv # Saved chatbot logs (auto-generated)
├── playlist_name.csv # Saved playlist data (auto-generated)
└── README.md # Project documentation

---

## 🏃 Getting Started

### Prerequisites
- A C++ compiler such as `g++` (recommended: GCC 9+ or MSVC)

### Build & Run

```bash
# Compile
g++ -std=c++17 main.cpp -o playlist_chatbot

# Run
./playlist_chatbot

📋 Sample Output

Playlist Mode:
Enter playlist name: MyPlaylist

===== MUSIC PLAYLIST GENERATOR =====
1. Add Song
2. Remove Song
3. View Playlist
4. Save to Excel
5. Exit to Main Menu
Enter choice: 1

ChatBot Mode:
Enter your name: Alice
Alice: Hello! I'm Therapy. How are you feeling today?
Alice: I'm sad
Therapy: Tell me more about your feelings...

📜 License
This project is open-source and available under the MIT License.
