#include <iostream>
#include <functional>

#include "mtmatt.h"

int N, Q;

void connect(const Query &query) {
  const int x = query.connect.x;
  const int y = query.connect.y;
  // Connect x and y
  std::cerr << "Connected " << x << " and " << y << std::endl;
}

void evolve(const Query &query) {
  const int t = query.evolve.t;
  // Evolve with time t
  std::cerr << "Evolved with time " << t << std::endl;
}

void attack(const Query &query) {
  const int t = query.attack.t;
  // Attack with time t
  std::cerr << "Attacked with time " << t << std::endl;
}

void reinstall(const Query &query) {
  const int k = query.reinstall.k;
  const int s = query.reinstall.s;
  // Reinstall with k and s
  std::cerr << "Reinstalled with k " << k << " and s " << s << std::endl;
}

void fusion(const Query &query) {
  const int a = query.fusion.a;
  const int b = query.fusion.b;
  // Fuse a and b
  std::cerr << "Fused " << a << " and " << b << std::endl;
}

void status(const Query &query) {
  const int k = query.status.k;
  // Get status of k
  std::cerr << "Status of " << k << std::endl;
}

void revert(const Query &query) {
  // Revert to the previous state
  std::cerr << "Reverted to previous state" << std::endl;
}

int main(void) {
  std::cin >> N >> Q;
  for (int i = 0; i < Q; ++i) {
    Query query;
    std::cin >> query.type;
    switch (query.type) {
    case CONNECT:
      std::cin >> query.connect.x >> query.connect.y;
      break;
    case EVOLVE:
      std::cin >> query.evolve.t;
      break;
    case ATTACK:
      std::cin >> query.attack.t;
      break;
    case REINSTALL:
      std::cin >> query.reinstall.k >> query.reinstall.s;
      break;
    case FUSION:
      std::cin >> query.fusion.a >> query.fusion.b;
      break;
    case STATUS:
      std::cin >> query.status.k;
      break;
    case REVERT:
      break;
    default:
      throw std::invalid_argument("Invalid query type");
    }
    std::function<void(const Query &)> query_func[] = {
        connect, evolve, attack, reinstall, fusion, status, revert};
    query_func[query.type - 1](query);
  }
}