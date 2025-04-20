#pragma once

#include <stack>
#include <vector>

/// Ensure is a macro that checks if a condition is true.
/// If the condition is false, it throws a runtime error with a message
/// indicating that the assertion failed. The macro is used to enforce
/// preconditions and postconditions in the code. It is a debugging tool
/// that helps catch errors early in the development process.
#define ensure(x)                                                              \
  if (!(x)) {                                                                  \
    throw std::runtime_error("Assertion failed: " #x);                         \
  }
#define ensure_msg(x, msg)                                                     \
  if (!(x)) {                                                                  \
    throw std::runtime_error(msg + ": " #x);                                   \
  }

/// Virus simulation query types.
/// The enum defines the different types of queries that can be performed
/// in the virus simulation. Each type of query is represented by a unique
/// integer value. The values are used to identify the type of query in the
/// Query struct. The enum values are used to improve code readability and
/// maintainability by providing meaningful names for the query types.
enum { CONNECT = 1, EVOLVE, ATTACK, REINSTALL, FUSION, STATUS, REVERT };

/// Query is a struct that represents a query in the virus simulation.
/// It contains a type field that indicates the type of query and a union
/// that contains the parameters for each type of query. The union allows
/// storing different types of data in the same memory location, which
/// saves memory and allows for more efficient use of resources.
struct Query {
  int type;
  union {
    struct {
      int x, y;
    } connect;
    struct {
      int t;
    } evolve, attack;
    struct {
      int k, s;
    } reinstall;
    struct {
      int a, b;
    } fusion;
    struct {
      int k;
    } status;
    struct {
    } revert;
  };
};

/// History is a stack that stores the history of changes made to the DSU.
/// It allows reverting the changes made to the DSU by restoring the original
/// values of the pointers stored in the history stack. The History class is
/// used to implement the revert functionality of the virus simulation.
class History {
public:
  History() = default;

  /// Reverts the changes made to the DSU by restoring the original values of
  /// the pointers stored in the history stack. The revert function is used to
  /// implement the revert functionality of the virus simulation.
  /// It pops all the nodes from the history stack and restores the original
  /// values of the pointers stored in the nodes. The function returns a
  /// reference to the current History object to allow for method chaining.
  History &revert() {
    while (!history_stack.empty()) {
      HistoryNode node = history_stack.top();
      history_stack.pop();
      *node.ptr = node.original_value;
    }
    return *this;
  }

  /// Adds a new node to the history stack. The add function is used to store
  /// the original value of a pointer and the new value assigned to it. The
  /// function takes a pointer to an integer and the new value to be assigned
  /// to the pointer. It creates a new HistoryNode object and pushes it onto
  /// the history stack. The function returns a reference to the current
  /// History object to allow for method chaining.
  History &add(int *ptr, int assigned_value) {
    HistoryNode node;
    node.ptr = ptr;
    node.original_value = *ptr;
    *ptr = assigned_value;
    history_stack.push(node);
    return *this;
  }

private:
  struct HistoryNode {
    int *ptr;
    int original_value;
  };
  std::stack<HistoryNode, std::vector<HistoryNode>> history_stack;
};

/// Base class for Disjoint Set Union (DSU).
/// This class implements the union-find algorithm with union by size.
struct DSU {
  std::vector<int> parent;
  std::vector<int> size;

  DSU() = default;
  DSU(int n) : parent(n + 1), size(n + 1, 1) {
    for (int i = 1; i <= n; ++i) {
      parent[i] = i;
    }
  }

  /// Returns the root of the set containing a.
  /// Path compression is not used to speed up future queries. Otherwise, the
  /// revert and delete operations will not work.
  int find(int a) const { return find(parent[a]); }
  bool same(int a, int b) const { return find(a) == find(b); }

  /// Merge the sets containing a and b.
  /// Returns 1 if the sets were merged, 0 if they were already in the same set.
  /// Union by size is used to speed up future queries.
  int merge(int a, int b, History &h) {
    a = find(a);
    b = find(b);
    if (a == b)
      return 0;
    if (size[a] < size[b])
      std::swap(a, b);
    h.add(&parent[b], a);
    h.add(&size[a], size[a] + size[b]);
    h.add(&size[b], 0);
    return 1;
  }
};

/// VirusDsu is a derived class from DSU that adds additional functionality
/// specific to the virus simulation.
struct VirusDsu : public DSU {
  std::vector<int> level;
  std::vector<int> infected_size;
  std::vector<int> tag_attack;

  VirusDsu() = default;
  VirusDsu(int n)
      : DSU(n), level(n + 1, 0), infected_size(n + 1, 0), tag_attack(n + 1, 0) {
  }
};

/// ComputerDsu is a derived class from DSU that adds additional functionality
/// specific to the computer simulation.
struct ComputerDsu : public DSU {
  std::vector<int> damage;
  std::vector<int> virus_type;

  ComputerDsu() = default;
  ComputerDsu(int n) : DSU(n), damage(n + 1, 0), virus_type(n + 1, 0) {}
};