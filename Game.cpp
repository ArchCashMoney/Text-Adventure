// Game.cpp  —  Implementation of the Game class
// Builds the dungeon graph and runs the main command loop.

#include "Game.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cctype>

// Constructor: set up player and populate the dungeon
Game::Game()
    : currentRoom("Cave Entrance"),
      playerHP(100), playerAttack(10),
      hasWeapon(false), gameOver(false), won(false)
{
    buildWorld();
}

// buildWorld: Create every room and wire them together as a graph.
//
// Dungeon layout:
//
//   [Cave Entrance] --east--> [Dim Corridor] --north--> [Armory]
//                                  |
//                                south
//                                  |
//                          [Ancient Library]
//                                  |
//                    (east from Dim Corridor)
//                                  |
//                           [Stone Gate] --east--> [Dragon's Lair]
//                                                        |
//                                                      north
//                                                        |
//                                                   [Treasury]  <- WIN

void Game::buildWorld() {

    // ---- Cave Entrance (starting room) ----
    Room caveEntrance("Cave Entrance",
        "A damp cave entrance. Moss clings to the stone walls.\n"
        "Daylight fades behind you. The only way forward is east.");
    caveEntrance.exits["east"] = "Dim Corridor";

    // ---- Dim Corridor (hub room) ----
    Room dimCorridor("Dim Corridor",
        "A dimly lit stone corridor. Torchlight flickers on the walls.\n"
        "Passages lead north, south, and east.");
    dimCorridor.exits["west"]  = "Cave Entrance";
    dimCorridor.exits["north"] = "Armory";
    dimCorridor.exits["south"] = "Ancient Library";
    dimCorridor.exits["east"]  = "Stone Gate";

    // ---- Armory (has weapon, has enemy) ----
    Room armory("Armory",
        "A cluttered armory. Rusted shields line the walls.\n"
        "An iron sword gleams on a rack — and a goblin growls in the corner!");
    armory.exits["south"]   = "Dim Corridor";
    armory.items.push_back("iron sword");
    armory.items.push_back("health potion");
    armory.enemy       = "Goblin";
    armory.enemyHP     = 30;
    armory.enemyAttack = 8;

    // ---- Ancient Library (has key) ----
    Room library("Ancient Library",
        "Dusty bookshelves stretch floor to ceiling.\n"
        "An iron key hangs on a hook by the door. A torch sits on the table.");
    library.exits["north"] = "Dim Corridor";
    library.items.push_back("iron key");
    library.items.push_back("torch");

    // ---- Stone Gate (locked — needs iron key) ----
    Room stoneGate("Stone Gate",
        "A massive iron gate blocks the eastern passage.\n"
        "It has a large keyhole. You'll need a key to proceed west.");
    stoneGate.exits["west"] = "Dim Corridor";
    stoneGate.exits["east"] = "Dragon's Lair";
    stoneGate.locked = true;   // player must have iron key

    // ---- Dragon's Lair (boss room) ----
    Room dragonsLair("Dragon's Lair",
        "The air is hot and reeks of sulfur. Scorched bones litter the floor.\n"
        "A massive dragon coils in the center, eyes locked on you!\n"
        "A passage leads north to the Treasury — if you can survive.");
    dragonsLair.exits["west"]  = "Stone Gate";
    dragonsLair.exits["north"] = "Treasury";
    dragonsLair.enemy       = "Dragon";
    dragonsLair.enemyHP     = 80;
    dragonsLair.enemyAttack = 20;

    // ---- Treasury (win condition) ----
    Room treasury("Treasury",
        "Gold coins and jewels overflow from countless chests.\n"
        "A shaft of light beams down from a hole in the ceiling — your escape!\n"
        "*** YOU WIN! Congratulations, adventurer! ***");

    // ---- Insert all rooms into the dungeon graph (unordered_map) ----
    rooms["Cave Entrance"]   = caveEntrance;
    rooms["Dim Corridor"]    = dimCorridor;
    rooms["Armory"]          = armory;
    rooms["Ancient Library"] = library;
    rooms["Stone Gate"]      = stoneGate;
    rooms["Dragon's Lair"]   = dragonsLair;
    rooms["Treasury"]        = treasury;
}

// run: Main game loop — print banner, show starting room, then read input
void Game::run() {
    printBanner();
    cmdLook();  // describe the starting room

    std::string line;
    while (!gameOver) {
        printSep();
        std::cout << "> ";
        if (!std::getline(std::cin, line)) break;  // handle EOF (Ctrl+D)
        if (!line.empty())
            processCommand(line);
    }

    // Final message based on outcome
    if (won)
        std::cout << "\nThank you for playing!\n";
    else if (playerHP <= 0)
        std::cout << "\nYou have been slain. Game over.\n";
    else
        std::cout << "\nFarewell, adventurer.\n";
}

// processCommand: Parse the verb (first word) and dispatch to a handler
void Game::processCommand(const std::string& input) {
    std::istringstream ss(toLower(input));
    std::string verb;
    ss >> verb;

    // Everything after the verb becomes the argument
    std::string arg;
    std::getline(ss, arg);
    if (!arg.empty() && arg[0] == ' ')
        arg = arg.substr(1);  // strip leading space

    // Dispatch to the appropriate command handler
    if      (verb == "look"  || verb == "l")                  cmdLook();
    else if (verb == "go"    || verb == "move")               cmdGo(arg);
    else if (verb == "north" || verb == "n")                  cmdGo("north");
    else if (verb == "south" || verb == "s")                  cmdGo("south");
    else if (verb == "east"  || verb == "e")                  cmdGo("east");
    else if (verb == "west"  || verb == "w")                  cmdGo("west");
    else if (verb == "take"  || verb == "get" || verb == "pick") cmdTake(arg);
    else if (verb == "inventory" || verb == "inv" || verb == "i") cmdInventory();
    else if (verb == "use")                                   cmdUse(arg);
    else if (verb == "attack" || verb == "fight" || verb == "a") cmdAttack();
    else if (verb == "undo"  || verb == "back")               cmdUndo();
    else if (verb == "stats" || verb == "status")             cmdStats();
    else if (verb == "help"  || verb == "h" || verb == "?")   cmdHelp();
    else if (verb == "quit"  || verb == "exit" || verb == "q") {
        std::cout << "Giving up so soon? Farewell, adventurer.\n";
        gameOver = true;
    }
    else {
        std::cout << "Unknown command: '" << verb
                  << "'. Type 'help' for a list of commands.\n";
    }
}

// cmdLook: Print the name, description, items, enemy, and exits
void Game::cmdLook() {
    Room& r = rooms[currentRoom];

    std::cout << "\n=== " << r.name << " ===\n";
    std::cout << r.description << "\n";

    // Show items on the floor
    if (!r.items.empty()) {
        std::cout << "\nItems here:\n";
        for (const auto& item : r.items)
            std::cout << "  - " << item << "\n";
    }

    // Show a live enemy (dead enemies have been cleared from the room)
    if (!r.enemy.empty() && r.enemyHP > 0)
        std::cout << "\n[!] " << r.enemy << " is here! (HP: " << r.enemyHP << ")\n";

    // List available exits
    std::cout << "\nExits: ";
    for (const auto& exit : r.exits)
        std::cout << exit.first << "  ";
    std::cout << "\n";

    r.visited = true;  // mark the room as visited
}

// cmdGo: Move the player in the given direction (graph traversal)
void Game::cmdGo(const std::string& dir) {
    if (dir.empty()) {
        std::cout << "Go where? Specify a direction (north, south, east, west).\n";
        return;
    }

    Room& current = rooms[currentRoom];

    // Check the adjacency list for this direction
    if (current.exits.find(dir) == current.exits.end()) {
        std::cout << "You can't go " << dir << " from here.\n";
        return;
    }

    // A live enemy blocks movement out of the room
    if (!current.enemy.empty() && current.enemyHP > 0) {
        std::cout << "The " << current.enemy
                  << " blocks your way! Defeat it before moving on.\n";
        return;
    }

    std::string nextName = current.exits.at(dir);
    Room& next = rooms[nextName];

    // Handle locked rooms — consume the key on first use
    if (next.locked) {
        if (inventory.contains("iron key")) {
            std::cout << "You use the iron key. The gate swings open!\n";
            next.locked = false;   // room is now permanently unlocked
        } else {
            std::cout << "The gate is locked. You need an iron key to open it.\n";
            return;
        }
    }

    // Push current room onto the history stack before moving (enables undo)
    moveHistory.push(currentRoom);

    // Traverse the graph edge — move to the new room
    currentRoom = nextName;
    cmdLook();

    // Check win condition
    if (currentRoom == "Treasury") {
        won = true;
        gameOver = true;
    }
}

// cmdTake: Pick up a named item from the current room's floor
void Game::cmdTake(const std::string& item) {
    if (item.empty()) {
        std::cout << "Take what? Specify an item name.\n";
        return;
    }

    Room& r = rooms[currentRoom];

    // Search room's item list for a case-insensitive match
    for (auto it = r.items.begin(); it != r.items.end(); ++it) {
        if (toLower(*it) == item) {
            // Add to the linked list inventory
            inventory.add(*it);
            std::cout << "You pick up the " << *it << ".\n";

            // Equipping the sword increases player attack stat
            if (*it == "iron sword") {
                hasWeapon    = true;
                playerAttack = 20;
                std::cout << "You equip the iron sword! Attack increased to "
                          << playerAttack << ".\n";
            }

            r.items.erase(it);  // remove from the room
            return;
        }
    }
    std::cout << "There is no '" << item << "' here.\n";
}

// cmdInventory: Display all items in the linked list
void Game::cmdInventory() {
    std::cout << "\nInventory:\n";
    inventory.print();  // traverses the linked list from head to tail
}
 
// cmdUse: Use a consumable or inspect a held item
void Game::cmdUse(const std::string& item) {
    if (item.empty()) {
        std::cout << "Use what?\n";
        return;
    }

    // Ensure the player actually has the item (O(n) linked list search)
    if (!inventory.contains(item)) {
        std::cout << "You don't have '" << item << "' in your inventory.\n";
        return;
    }

    if (item == "health potion") {
        // Cap HP at 100
        int before = playerHP;
        playerHP = std::min(100, playerHP + 40);
        inventory.remove("health potion");  // consumed — remove from linked list
        std::cout << "You drink the health potion. HP: "
                  << before << " -> " << playerHP << ".\n";
    }
    else if (item == "torch") {
        std::cout << "You hold the torch aloft. The shadows retreat slightly.\n";
    }
    else if (item == "iron key") {
        std::cout << "This key looks like it fits a large gate to the east.\n";
    }
    else if (item == "iron sword") {
        std::cout << "You're already wielding the iron sword (attack: "
                  << playerAttack << ").\n";
    }
    else {
        std::cout << "You can't figure out how to use that here.\n";
    }
}
 
// cmdAttack: Fight the enemy in the current room (round-based combat)
void Game::cmdAttack() {
    Room& r = rooms[currentRoom];

    // Check there is a living enemy to fight
    if (r.enemy.empty() || r.enemyHP <= 0) {
        std::cout << "There is nothing to attack here.\n";
        return;
    }

    // Player's turn — deal damage to the enemy
    r.enemyHP -= playerAttack;
    std::cout << "You strike the " << r.enemy << " for "
              << playerAttack << " damage!";

    if (r.enemyHP <= 0) {
        // Enemy defeated — clear it from the room
        std::cout << "\nThe " << r.enemy << " collapses! The path is clear.\n";
        r.enemy = "";
        return;
    }
    std::cout << " (" << r.enemy << " HP: " << r.enemyHP << ")\n";

    // Enemy's counter-attack
    playerHP -= r.enemyAttack;
    std::cout << "The " << r.enemy << " hits you for "
              << r.enemyAttack << " damage! (Your HP: " << playerHP << ")\n";

    // Check if the player has died
    if (playerHP <= 0) {
        std::cout << "\nYou collapse to the ground...\n";
        gameOver = true;
    }
}

// cmdUndo: Pop the move history stack to backtrack one room
//          DS #3 in action — O(1) pop from the stack
void Game::cmdUndo() {
    if (moveHistory.empty()) {
        std::cout << "Nothing to undo — you haven't moved yet.\n";
        return;
    }

    // Retrieve and remove the last room from the stack
    std::string prev = moveHistory.top();
    moveHistory.pop();
    currentRoom = prev;
    std::cout << "You backtrack to " << currentRoom << ".\n";
    cmdLook();
}

// cmdStats: Display the player's current health and attack
void Game::cmdStats() {
    std::cout << "\n--- Player Stats ---\n";
    std::cout << "  HP:     " << playerHP << " / 100\n";
    std::cout << "  Attack: " << playerAttack;
    if (hasWeapon) std::cout << "  (iron sword equipped)";
    std::cout << "\n";
}

// cmdHelp: Print the list of available commands
void Game::cmdHelp() {
    std::cout << "\n--- Commands ---\n"
              << "  look / l              - Describe the current room\n"
              << "  go [dir] / n/s/e/w    - Move in a direction\n"
              << "  take [item]           - Pick up an item\n"
              << "  inventory / inv / i   - Show your inventory\n"
              << "  use [item]            - Use an item from inventory\n"
              << "  attack / fight / a    - Attack the enemy in this room\n"
              << "  undo / back           - Backtrack to the previous room\n"
              << "  stats                 - Show HP and attack power\n"
              << "  help / ?              - Show this help text\n"
              << "  quit / q              - Exit the game\n";
}


// toLower: Return a lowercase copy of the given string (for parsing)
std::string Game::toLower(const std::string& s) const {
    std::string result = s;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

// printBanner: Display the title screen
void Game::printBanner() const {
    std::cout << "============================================\n"
              << "       DUNGEON OF THE LOST\n"
              << "       A CS202 Text Adventure\n"
              << "============================================\n"
              << "Type 'help' for a list of commands.\n\n";
}

// printSep: Print a divider between turns
void Game::printSep() const {
    std::cout << "--------------------------------------------\n";
}
