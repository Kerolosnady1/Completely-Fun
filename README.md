Project Description
This C++ program combines two main functionalities:

Music Playlist Manager: A system to create and manage music playlists with YouTube links

ChatBot System: An interactive chatbot with different personality modes

The program offers a menu-driven interface to switch between these two functionalities.

Features
Music Playlist Manager
Create playlists with custom names

Add songs with title, artist, genre, duration, and YouTube link

Validate YouTube links (checks for youtube.com or youtu.be)

Remove songs from playlist

View all songs in playlist

Save playlist to CSV file (Excel-compatible)

Activity logging with timestamps

ChatBot System
Interactive conversation system

Keyword-based responses

Multiple bot personalities (standard and angry modes)

Save chat history to CSV file

Emotion detection based on keywords

Installation
Ensure you have a C++ compiler installed (g++, clang, or MSVC)

Clone this repository or download the source code

Compile the program:

bash
g++ main.cpp -o playlist_chatbot
Run the executable:

bash
./playlist_chatbot
Usage
Main Menu:

Choose between Music Playlist Manager (1) or ChatBot Program (2)

Select option 3 to exit

Music Playlist Manager:

Create a new playlist by entering a name

Use the menu to:

Add songs (with validation)

Remove songs

View playlist

Save to CSV

Return to main menu

ChatBot Program:

Enter your name to start

Chat with the bot (type "bye" to exit)

Conversation will be saved to chat_history.csv

File Structure
The program generates the following files:

[playlist_name].csv - Playlist data in CSV format

chat_history.csv - Saved conversation history

Technologies Used
C++ Standard Library

File I/O operations

Object-Oriented Programming

Time handling

String manipulation

Contributing
Contributions are welcome! Please follow these steps:

Fork the repository

Create a new branch for your feature

Commit your changes

Push to the branch

Create a Pull Request

License
This project is open-source and available under the MIT License.
