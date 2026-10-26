// A generic singly linked list used for the player's inventory.
// Supports add, remove, contains, and print operations.

#pragma once
#include <iostream>
#include <string>

template <typename T>
class LinkedList {
private:
    // Internal node: holds data and a pointer to the next node
    struct Node {
        T data;
        Node* next;
        Node(const T& val) : data(val), next(nullptr) {}
    };

    Node* head;   // pointer to the front of the list
    int   size_;  // tracks number of elements

public:
    // Constructor: empty list
    LinkedList() : head(nullptr), size_(0) {}

    // Destructor: walk the list and free every node
    ~LinkedList() {
        Node* curr = head;
        while (curr) {
            Node* temp = curr->next;
            delete curr;
            curr = temp;
        }
    }

    // Add a new item to the front of the list — O(1)
    void add(const T& val) {
        Node* node = new Node(val);
        node->next = head;
        head = node;
        size_++;
    }

    // Remove the first occurrence of val; returns true if found — O(n)
    bool remove(const T& val) {
        if (!head) return false;

        // Special case: val is at the head
        if (head->data == val) {
            Node* temp = head;
            head = head->next;
            delete temp;
            size_--;
            return true;
        }

        // Walk the list looking for val
        Node* curr = head;
        while (curr->next) {
            if (curr->next->data == val) {
                Node* temp = curr->next;
                curr->next = temp->next;
                delete temp;
                size_--;
                return true;
            }
            curr = curr->next;
        }
        return false;  // not found
    }

    // Returns true if val exists anywhere in the list — O(n)
    bool contains(const T& val) const {
        Node* curr = head;
        while (curr) {
            if (curr->data == val) return true;
            curr = curr->next;
        }
        return false;
    }

    // Print every item in the list, one per line
    void print() const {
        if (!head) {
            std::cout << "  (empty)\n";
            return;
        }
        Node* curr = head;
        while (curr) {
            std::cout << "  - " << curr->data << "\n";
            curr = curr->next;
        }
    }

    int  size()  const { return size_; }
    bool empty() const { return size_ == 0; }
};
