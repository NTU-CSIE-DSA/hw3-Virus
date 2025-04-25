#pragma once

#define FEATURE_COMPUTER

#include <stack>
#include <vector>

/// Ensure is a macro that checks if a condition is true.
/// If the condition is false, it throws a runtime error with a message
/// indicating that the assertion failed. The macro is used to enforce
/// preconditions and postconditions in the code. It is a debugging tool
/// that helps catch errors early in the development process.
/// @param x The condition to check.
/// @throws std::runtime_error if the condition is false.
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
/// long longeger value. The values are used to identify the type of query in the
/// Query struct. The enum values are used to improve code readability and
/// malong longainability by providing meaningful names for the query types.
enum { CONNECT = 1, EVOLVE, ATTACK, REINSTALL, FUSION, STATUS, REVERT };

/// Query is a struct that represents a query in the virus simulation.
/// It contains a type field that indicates the type of query and a union
/// that contains the parameters for each type of query. The union allows
/// storing different types of data in the same memory location, which
/// saves memory and allows for more efficient use of resources.
struct Query {
  long long type;
  union {
    struct {
      long long x, y;
    } connect;
    struct {
      long long t;
    } evolve, attack;
    struct {
      long long k, s;
    } reinstall;
    struct {
      long long a, b;
    } fusion;
    struct {
      long long k;
    } status;
    struct {
    } revert;
  };
};

/// History is a stack that stores the history of changes made to the DSU.
/// It allows reverting the changes made to the DSU by restoring the original
/// values of the polong longers stored in the history stack. The History class is
/// used to implement the revert functionality of the virus simulation.
class History {
public:
  History() = default;

  /// Reverts the changes made to the DSU by restoring the original values of
  /// the polong longers stored in the history stack. The revert function is used to
  /// implement the revert functionality of the virus simulation.
  /// It pops all the nodes from the history stack and restores the original
  /// values of the polong longers stored in the nodes. The function returns a
  /// reference to the current History object to allow for method chaining.
  /// @return A reference to the current History object.
  History &revert() {
    while (!history_stack.empty()) {
      HistoryNode node = history_stack.top();
      history_stack.pop();
      *node.ptr = node.original_value;
    }
    return *this;
  }

  /// Adds a new node to the history stack. The add function is used to store
  /// the original value of a polong longer and the new value assigned to it. The
  /// function takes a polong longer to an long longeger and the new value to be assigned
  /// to the polong longer. It creates a new HistoryNode object and pushes it onto
  /// the history stack. The function returns a reference to the current
  /// History object to allow for method chaining.
  /// @param ptr The polong longer to the long longeger to be modified.
  /// @param assigned_value The new value to be assigned to the polong longer.
  /// @return A reference to the current History object.
  History &add(long long *ptr, long long assigned_value) {
    HistoryNode node;
    node.ptr = ptr;
    node.original_value = *ptr;
    *ptr = assigned_value;
    history_stack.push(node);
    return *this;
  }

private:
  struct HistoryNode {
    long long *ptr;
    long long original_value;
  };
  std::stack<HistoryNode, std::vector<HistoryNode>> history_stack;
};

/// Base class for Disjolong long Set Union (DSU).
/// This class implements the union-find algorithm with union by size.
struct DSU {
  std::vector<long long> parent;
  std::vector<long long> size;

  DSU() = default;
  DSU(long long n) : parent(n + 1), size(n + 1, 1) {
    for (long long i = 1; i <= n; ++i) {
      parent[i] = i;
    }
  }

  /// Returns the root of the set containing a.
  /// Path compression is not used to speed up future queries. Otherwise, the
  /// revert and delete operations will not work.
  /// @param a The element to find the root of.
  /// @return The root of the set containing a.
  long long find(long long a) const { return find(parent[a]); }
  bool same(long long a, long long b) const { return find(a) == find(b); }

  /// Merge the sets containing a and b.
  /// Returns 1 if the sets were merged, 0 if they were already in the same set.
  /// Union by size is used to speed up future queries.
  /// @param a The first element.
  /// @param b The second element.
  /// @param h The history stack to store the changes made to the DSU.
  /// @return 1 if the sets were merged, 0 if they were already in the same set.
  long long merge(long long a, long long b, History &h) {
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
#ifdef FEATURE_VIRUS
struct VirusDsu : public DSU {
  std::vector<long long> level;
  std::vector<long long> infected_size;
  std::vector<long long> tag_attack;

  VirusDsu() = default;
  VirusDsu(long long n)
      : DSU(n), level(n + 1, 0), infected_size(n + 1, 0), tag_attack(n + 1, 0) {
  }
};
#endif

/// ComputerDsu is a derived class from DSU that adds additional functionality
/// specific to the computer simulation.
#ifdef FEATURE_COMPUTER
struct ComputerDsu : public DSU {
  std::vector<long long> damage;
  std::vector<long long> virus_type;

  ComputerDsu() = default;
  ComputerDsu(long long n) : DSU(n), damage(n + 1, 0), virus_type(n + 1, 0) {}
};
#endif