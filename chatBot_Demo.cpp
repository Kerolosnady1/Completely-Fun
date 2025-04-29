// ================= ProtoType ===================
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <ctime>
using namespace std;

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

// ====================== ChatBot Class ======================
class ChatBot
{
protected:
    string name;
    ResponseSystem responder;
    vector<string> chatHistory;

public:
    ChatBot(const string &botName) : name(botName) {}

    void greet()
    {
        string greeting = name + ": Hi! I'm " + name + ". How are you feeling today?";
        cout << greeting << endl;
        chatHistory.push_back(greeting);
    }

    virtual string reply(const string &input)
    {
        string response = responder.getResponse(input);
        string fullResponse = name + ": " + response;
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
            cerr << "Error creating file!" << endl;
            return;
        }

        // Add timestamp
        time_t now = time(0);
        excelFile << "Chat Session," << ctime(&now) << "\n";

        // Write headers
        excelFile << "Speaker,Message\n";

        // Write chat history
        for (const auto &message : chatHistory)
        {
            size_t colonPos = message.find(":");
            string speaker = message.substr(0, colonPos);
            string msg = message.substr(colonPos + 2);
            excelFile << speaker << "," << msg << "\n";
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
        string angryResponse = name + ": Ugh. " + baseResponse + " Stop complaining!";
        cout << angryResponse << endl;
        chatHistory.push_back("User: " + input);
        chatHistory.push_back(angryResponse);
        return angryResponse;
    }
};

// ====================== Main Program ======================
int main()
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
    return 0;
}