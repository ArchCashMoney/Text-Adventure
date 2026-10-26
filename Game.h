// CS202 Data Structures used:
//   #1  LinkedList<string>           — player inventory
//   #2  unordered_map<string, Room>  — dungeon graph (rooms as nodes)
//   #3  stack<string>                — move history for the "undo" command

#pragma once
#include "LinkedList.h"
#include "Room.h"
#include <unordered_map>
#include <stack>
#include <string>

class Game {
private:

    // DS #1: Custom linked list storing the player's held items
    LinkedList<std::string> inventory;

    // DS #2: Dungeon graph — maps room name -> Room object (hash map)
    //        Each Room's 'exits' map forms the directed graph edges.
    std::unordered_map<std::string, Room> rooms;

    // DS #3: Stack of previously visited room names for "undo last move"
    std::stack<std::string> moveHistory;

    // Player State
    std::string currentRoom;  // name of the room the player is in
    int         playerHP;     // player health (0 = dead)
    int         playerAttack; // damage dealt per attack (increases with weapon)
    bool        hasWeapon;    // true once the iron sword is picked up
    bool        gameOver;     // set true when player wins or dies
    bool        won;          // distinguishes a win from a loss

    //  Private Helpers
    void buildWorld();                            // populate the dungeon graph
    void processCommand(const std::string& cmd);  // parse input and dispatch

    // Command handlers — each corresponds to one player action
    void cmdLook();
    void cmdGo(const std::string& dir);
    void cmdTake(const std::string& item);
    void cmdInventory();
    void cmdUse(const std::string& item);
    void cmdAttack();
    void cmdUndo();
    void cmdStats();
    void cmdHelp();

    // Utility helpers
    std::string toLower(const std::string& s) const;
    void printBanner() const;
    void printSep() const;

public:
    Game();   // initializes player stats and builds the world
    void run(); // starts the main game loop
};
