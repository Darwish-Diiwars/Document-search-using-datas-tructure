#include <iostream>
#include<fstream>
using namespace std;

const int ALPHABET_SIZE = 26;

template <typename T>
class Vector {
private:
    T* data;
    size_t capacity;
    size_t size;

public:

    Vector() : data(nullptr), capacity(0), size(0) {}

    ~Vector() {
        delete[] data;
    }

    void push_back(const T& element) {
        if (size >= capacity) {
            expand();
        }
        data[size++] = element;
    }

    void expand() {
        capacity = (capacity == 0) ? 1 : capacity * 2;
        T* newData = new T[capacity];
        for (size_t i = 0; i < size; ++i) {
            newData[i] = data[i];
        }
        delete[] data;
        data = newData;
    }

    size_t getSize() const {
        return size;
    }


    T& operator[](size_t index) {
        if (index >= size) {
            throw out_of_range("Index out of range");
        }
        return data[index];
    }

    const T& operator[](size_t index) const {
        if (index >= size) {
            throw out_of_range("Index out of range");
        }
        return data[index];
    }
};


class TrieNode {
public:
    TrieNode* children[ALPHABET_SIZE];
    bool isEndOfWord;


    TrieNode() : isEndOfWord(false) {
        for (int i = 0; i < ALPHABET_SIZE; ++i) {
            children[i] = nullptr;
        }
    }
};


class Trie {
private:
    TrieNode* root;
    Vector<TrieNode*> nodes;

public:

    Trie() : root(new TrieNode()) {
        nodes.push_back(root);
    }

    ~Trie() {
        clear();
    }

    void insert(const string& word) {
        TrieNode* current = root;
        for (char c : word) {
            int index = c - 'a';
            if (!current->children[index]) {
                current->children[index] = new TrieNode();
                nodes.push_back(current->children[index]);
            }
            current = current->children[index];
        }
        current->isEndOfWord = true;
    }


    bool search(const string& word) {
        TrieNode* current = root;
        for (char c : word) {
            int index = c - 'a';
            if (!current->children[index]) {
                return false;
            }
            current = current->children[index];
        }
        return (current != nullptr && current->isEndOfWord);
    }

    void deleteWord(const string& word) {
        Erase(root, word, 0);
    }

    void printAllWords() {
        string word;
        Run_off(root, word);
    }

    void partialWordSuffixSearch(const string& suffix, Vector<string>& words) {
        partialWordSuffix(root, suffix, "", words);
    }

    void fuzzySearch(const string& pattern, Vector<string>& words) {
        fuzzySearch(root, pattern, "", words);
    }

    void misspelledSearch(const string& word, int maxEdits, Vector<string>& words) {
        misspelledSearch(root, word, "", 0, maxEdits, words);
    }

    bool singleWordSearch(const string& word) {
        return search(word);
    }

    Vector<bool> multipleWordsSearch(const Vector<string>& words) {
        Vector<bool> results;
        for (size_t i = 0; i < words.getSize(); ++i) {
            results.push_back(search(words[i]));
        }
        return results;
    }

    Vector<string> partialWordSearch(const string& prefix) {
        Vector<string> words;
        TrieNode* current = root;
        for (char c : prefix) {
            int index = c - 'a';
            if (!current->children[index]) {
                return words;
            }
            current = current->children[index];
        }
        partialWordprefix(current, prefix, words);
        return words;
    }

    Vector<string> wildcardSearch(const string& pattern) {
        Vector<string> words;
        wildcardSearch(root, pattern, "", words);
        return words;
    }

private:

    void Erase(TrieNode* node, const string& word, int depth) {
        if (depth == word.length()) {
            if (node->isEndOfWord) {
                node->isEndOfWord = false;
            }
            return;
        }

        int index = word[depth] - 'a';
        if (!node->children[index]) {
            return;
        }

        Erase(node->children[index], word, depth + 1);

        if (isEmpty(node->children[index]) && !node->children[index]->isEndOfWord) {
            deleteNode(node->children[index]);
            delete node->children[index];
            node->children[index] = nullptr;
        }
    }

    bool isEmpty(TrieNode* node) {
        for (int i = 0; i < ALPHABET_SIZE; ++i) {
            if (node->children[i]) {
                return false;
            }
        }
        return true;
    }

    void Run_off(TrieNode* node, string& word) {
        if (node->isEndOfWord) {
            cout << word << endl;
        }
        for (int i = 0; i < ALPHABET_SIZE; ++i) {
            if (node->children[i]) {
                word.push_back('a' + i);
                Run_off(node->children[i], word);
                word.pop_back();
            }
        }
    }

    void clear() {
        for (size_t i = 0; i < nodes.getSize(); ++i) {
            delete nodes[i];
        }
    }

    void deleteNode(TrieNode* node) {
        for (size_t i = 0; i < nodes.getSize(); ++i) {
            if (nodes[i] == node) {
                nodes[i] = nullptr;
                break;
            }
        }
    }

    void partialWordSuffix(TrieNode* node, const string& suffix, const string& currentWord, Vector<string>& words) {
        if (node == nullptr) return;

        if (node->isEndOfWord) {
            if (currentWord.size() >= suffix.size()) {
                int currentindx = 0;
                bool isFound = true;
                for (int i = currentWord.size() - suffix.size(); i < currentWord.size(); i++) {
                    if (currentWord[i] != suffix[currentindx++]) {
                        isFound = false;
                        break;
                    }
                }
                if (isFound)
                    words.push_back(currentWord);
            }
        }

        for (int i = 0; i < ALPHABET_SIZE; i++) {
            if (node->children[i]) {
                partialWordSuffix(node->children[i], suffix, currentWord + char('a' + i), words);
            }
        }
    }

    void fuzzySearch(TrieNode* node, const string& pattern, const string& currentWord, Vector<string>& words, int patternindx = 0) {
        if (node == nullptr) return;

        if (patternindx == pattern.size()) {
            if (node->isEndOfWord) {
                words.push_back(currentWord);
            }
        }
        else {
            char nextChar = pattern[patternindx];
            int nextCharint = nextChar - 'a';
            if (nextCharint < 0 || nextCharint >= 26) {
                for (int i = 0; i < ALPHABET_SIZE; i++) {
                    if (node->children[i]) {
                        fuzzySearch(node->children[i], pattern, currentWord + char('a' + i), words, patternindx + 1);
                    }
                }
            }
            else {
                int index = nextChar - 'a';
                if (node->children[index]) {
                    fuzzySearch(node->children[index], pattern, currentWord + nextChar, words, patternindx + 1);
                }
            }
        }
    }


    void misspelledSearch(TrieNode* node, const string& word, const string& currentWord, int currentEdits, int maxEdits, Vector<string>& words, int wordindx = 0) {
        if (node == nullptr || currentEdits > maxEdits) return;

        if (wordindx == word.size()) {
            if (node->isEndOfWord) {
                words.push_back(currentWord);
            }
            return;
        }

        char nextChar = word[wordindx];
        for (int i = 0; i < ALPHABET_SIZE; ++i) {
            if (node->children[i]) {
                misspelledSearch(node->children[i], word, currentWord + char('a' + i), currentEdits + (nextChar != 'a' + i), maxEdits, words, wordindx + 1);
            }
        }
    }

    void partialWordprefix(TrieNode* node, const string& prefix, Vector<string>& words) {
        if (node == nullptr) return;

        if (node->isEndOfWord) {
            words.push_back(prefix);
        }

        for (int i = 0; i < ALPHABET_SIZE; ++i) {
            if (node->children[i]) {
                partialWordprefix(node->children[i], prefix + char('a' + i), words);
            }
        }
    }

    void wildcardSearch(TrieNode* node, const string& pattern, const string& current, Vector<string>& words, int pattern_Index = 0) {
        if (!node) return;

        if (pattern_Index == pattern.size()) {
            if (node->isEndOfWord) {
                words.push_back(current);
            }
            return;
        }

        char c = pattern[pattern_Index];
        if (c == '*') {
            for (int i = 0; i < ALPHABET_SIZE; ++i) {
                if (node->children[i]) {
                    wildcardSearch(node->children[i], pattern, current + char('a' + i), words, pattern_Index + 1);
                }
            }
        }
        else {
            int index = c - 'a';
            if (index >= 0 && index < ALPHABET_SIZE && node->children[index]) {
                wildcardSearch(node->children[index], pattern, current + c, words, pattern_Index + 1);
            }
        }
    }
};

int main() {
    Trie trie;
    ifstream File("dataset.txt"); 
    string word;
    while (File >> word) {
        trie.insert(word);
    }
    File.close();

    while (true) {
        cout << "What type of operation do you want to perform?" << endl;
        cout << "1. Single Word Search" << endl;
        cout << "2. Multiple Words Search" << endl;
        cout << "3. Prefix Search" << endl;
        cout << "4. Suffix Search" << endl;
        cout << "5. Fuzzy Search" << endl;
        cout << "6. Misspelled Search" << endl;
        cout << "7. Wildcard Search" << endl;
        cout << "8. Print All Words" << endl;
        cout << "9. Delete Word" << endl;
        cout << "Enter your choice : ";
        int choice;
        cin >> choice;

        if (choice == 0) {
            cout << "Exiting..." << endl;
            break;
        }

        string searchWord;
        if (choice >= 1 && choice <= 7 || choice == 9) {
            cout << "Enter the word: ";
            cin >> searchWord;
        }

        switch (choice) {
        case 1: {
            cout << "Single word search for '" << searchWord << "': " << (trie.singleWordSearch(searchWord) ? "Found" : "Not Found") << endl;
            break;
        }
        case 2: {
            Vector<string> multipleWords;
            cout << "Enter multiple words separated by space (end with 'END'): ";
            while (cin >> word && word != "END") {
                multipleWords.push_back(word);
            }
            Vector<bool> results = trie.multipleWordsSearch(multipleWords);
            cout << "Multiple words search results:" << endl;
            for (size_t i = 0; i < results.getSize(); ++i) {
                cout << multipleWords[i] << ": " << (results[i] ? "Found" : "Not Found") << endl;
            }
            break;
        }
        case 3: {
            Vector<string> prefixWords = trie.partialWordSearch(searchWord);
            cout << "Words with prefix '" << searchWord << "':" << endl;
            for (size_t i = 0; i < prefixWords.getSize(); ++i) {
                cout << prefixWords[i] << endl;
            }
            break;
        }
        case 4: {
            Vector<string> suffixWords;
            trie.partialWordSuffixSearch(searchWord, suffixWords);
            cout << "Words with suffix '" << searchWord << "':" << endl;
            for (size_t i = 0; i < suffixWords.getSize(); ++i) {
                cout << suffixWords[i] << endl;
            }
            break;
        }
        case 5: {
            Vector<string> fuzzyWords;
            trie.fuzzySearch(searchWord, fuzzyWords);
            cout << "Fuzzy search results for '" << searchWord << "':" << endl;
            for (size_t i = 0; i < fuzzyWords.getSize(); ++i) {
                cout << fuzzyWords[i] << endl;
            }
            break;
        }
        case 6: {
            int editDistance;
            cout << "Enter the maximum edit distance: ";
            cin >> editDistance;
            Vector<string> misspelledWords;
            trie.misspelledSearch(searchWord, editDistance, misspelledWords);
            cout << "Misspelled search results for '" << searchWord << "' with " << editDistance << " edit(s):" << endl;
            for (size_t i = 0; i < misspelledWords.getSize(); ++i) {
                cout << misspelledWords[i] << endl;
            }
            break;
        }
        case 7: {
            cout << "Wildcard search results for \"" << searchWord << "\":" << endl;
            Vector<string> wildcardWords = trie.wildcardSearch(searchWord);
            for (size_t i = 0; i < wildcardWords.getSize(); ++i) {
                cout << wildcardWords[i] << endl;
            }
            break;
        }
        case 8: {
            cout << "Printing all words in the Trie:" << endl;
            trie.printAllWords();
            break;
        }
        case 9: {
            trie.deleteWord(searchWord);
            cout << "Deleted word '" << searchWord << "' from the Trie." << endl;
            break;
        }
        default: {
            cout << "Invalid choice." << endl;
            break;
        }
        }
    }

    return 0;
}
