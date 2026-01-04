#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <queue>
#include <list>
#include <functional> // For priority queue comparison

// ==========================================
// 0. COMMON DEFINITIONS & ENUMS
// ==========================================

enum class ItemType { FOOD, HERB, TOOL, KEY_ITEM };
enum class Role { HUNTER, SCOUT, GUARD, NONE };

// Represents the state of the game loop
enum class GameState { START_SCREEN, PLAYING, EVENT_TRIGGERED, GAMEOVER, VICTORY };

// ==========================================
// MODULE 2: CHARACTER & INVENTORY (Linked List)
// ==========================================

// 1. Data Structure: Singly Linked List Node for Inventory
struct ItemNode {
    std::string name;
    ItemType type;
    int effectValue; // e.g., +20 Health or -10 Hunger
    std::string description;
    
    ItemNode* next; // Pointer to next item

    ItemNode(std::string n, ItemType t, int v, std::string d) 
        : name(n), type(t), effectValue(v), description(d), next(nullptr) {}
};

// 2. Class: Inventory System
class Inventory {
private:
    ItemNode* head;
    int itemCount;
    const int MAX_ITEMS = 10;

public:
    Inventory();
    ~Inventory(); // Destructor to clean up memory

    bool addItem(std::string name, ItemType type, int value, std::string desc);
    bool useItem(std::string itemName, class Wolf* player); // Forward declaration of Wolf needed
    void removeItem(std::string itemName);
    void displayInventory();
    bool isFull();
    
    // For Save/Load
    ItemNode* getHead() const { return head; }
};

// 3. Class: Pack Member (Linked List Node)
struct PackMember {
    std::string name;
    Role role;
    int loyalty;
    PackMember* next;
};

// 4. Class: The Player (Wolf)
class Wolf {
public:
    // Core Stats
    int health;   // 0-100
    int hunger;   // 0-100
    int energy;   // 0-100
    int reputation; // 0-100

    Inventory inventory;
    PackMember* packHead; // Head of the pack linked list

    Wolf();
    
    // Core Mechanics
    void updateStats(int h, int hu, int en, int rep);
    bool isAlive();
    void rest(); // Restores energy
    void feed(int amount);
    void takeDamage(int amount);
    
    // Pack Mechanics
    void recruitMember(std::string name, Role role);
    void displayPack();
};

// ==========================================
// MODULE 1: STORY & DECISION SYSTEM (Binary Tree)
// ==========================================

// 5. Data Structure: Binary Tree Node
struct StoryNode {
    int id;
    std::string scenarioText;
    
    // Choice Texts
    std::string choiceAText;
    std::string choiceBText;

    // Pointers to consequences/next scenarios
    StoryNode* left;  // Path A
    StoryNode* right; // Path B

    bool isEnding;
    std::string endingDescription;

    StoryNode(int _id, std::string text) 
        : id(_id), scenarioText(text), left(nullptr), right(nullptr), isEnding(false) {}
};

// 6. Class: Decision Tree Manager
class StoryTree {
private:
    StoryNode* root;
    StoryNode* currentScenario;

    // Helper for recursive deletion
    void deleteTree(StoryNode* node);
    // Helper for finding a node by ID (for loading games)
    StoryNode* findNode(StoryNode* node, int id);

public:
    StoryTree();
    ~StoryTree();

    void buildTree(); // HARDCODED logic to build the tree (Task for Story Team)
    
    // Navigation
    void moveToLeft();  // Player chose A
    void moveToRight(); // Player chose B
    
    // Getters
    StoryNode* getCurrentNode();
    void setCurrentNode(int id); // Used when loading a save
    bool isAtEnding();
};

// ==========================================
// MODULE 3: COMBAT & EVENTS (Priority Queue)
// ==========================================

// 7. Struct: Event Object
struct GameEvent {
    std::string title;
    std::string description;
    int priority; // 1 = Critical, 2 = Urgent, 3 = Normal
    
    // Comparison operator for Priority Queue (Min-Heap logic)
    // We want smaller numbers (1) to be at the TOP of the queue
    bool operator>(const GameEvent& other) const {
        return priority > other.priority; 
    }
};

// 8. Class: Event Manager
class EventManager {
private:
    // STL Priority Queue: <Type, Container, Comparator>
    std::priority_queue<GameEvent, std::vector<GameEvent>, std::greater<GameEvent>> eventQueue;

public:
    void triggerRandomEvent(); // Logic to generate random events
    void addEvent(std::string title, std::string desc, int priority);
    void processNextEvent(Wolf* player); // Pop and execute
    bool hasPendingEvents();
    GameEvent peekNextEvent();
};

// ==========================================
// MODULE 4: GAME STATE & HISTORY (Stack & Queue)
// ==========================================

// 9. Struct: Snapshot for Undo functionality
struct GameSnapshot {
    int day;
    int health, hunger, energy;
    int currentNodeID;
    // Note: Deep copying inventory for undo is complex, 
    // for this level simply tracking stats + location is usually sufficient.
};

// 10. Class: Game Loop & History
class GameEngine {
private:
    Wolf player;
    StoryTree story;
    EventManager events;
    
    // Stack for Undo (LIFO)
    std::stack<GameSnapshot> historyStack;
    
    // Queue for Multi-turn actions (FIFO)
    std::queue<std::string> actionQueue; 

    int currentDay;
    GameState state;

public:
    GameEngine();
    
    // Main Flow
    void initGame();       // Setup tree, stats
    void updateGameLoop(); // Called every frame by ImGui
    
    // State Management
    void saveState();      // Push to stack
    void undoLastMove();   // Pop from stack
    
    // File I/O
    void saveToFile(std::string filename);
    void loadFromFile(std::string filename);

    // Getters for GUI
    Wolf* getPlayer() { return &player; }
    StoryTree* getStory() { return &story; }
    int getDay() { return currentDay; }
};