// Each Room stores its adjacency list as an unordered_map
// (direction -> room name), forming the edges of the graph.
// Together, all rooms stored in Game's unordered_map compose
// CS202 Data Structure #2: a hash-map-backed graph.

#pragma once
#include <string>
#include <unordered_map>
#include <vector>

struct Room {
    std::string name;
    std::string description;

    // Graph adjacency list: direction (north/south/east/west) -> destination room name
    std::unordered_map<std::string, std::string> exits;

    // Items currently on the floor of this room
    std::vector<std::string> items;

    // Enemy occupying this room (empty string = no enemy present)
    std::string enemy;
    int enemyHP;       // current enemy health; 0 = dead
    int enemyAttack;   // damage the enemy deals per hit

    // Locked rooms require the player to have the iron key before entering
    bool locked;

    // Tracks whether the player has visited; could be used for short descriptions
    bool visited;

    // Default constructor
    Room()
        : enemyHP(0), enemyAttack(0), locked(false), visited(false) {}

    // Convenience constructor
    Room(const std::string& n, const std::string& desc)
        : name(n), description(desc),
          enemyHP(0), enemyAttack(0), locked(false), visited(false) {}
};
