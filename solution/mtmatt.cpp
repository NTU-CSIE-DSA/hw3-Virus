#include <functional>
#include <iostream>
#include <stack>

#include "mtmatt.h"

int N, Q;
std::stack<History, std::vector<History>> history;
ComputerDsu computer_dsu;
VirusDsu virus_dsu;

// TODO
void connect(const Query &query) {
  const int x = query.connect.x;
  const int y = query.connect.y;
  History &h = history.top();
  std::cerr << "Connected " << x << " and " << y << std::endl;
}

/// Evolve the virus t. Simply add one to the level of the virus t.
void evolve(const Query &query) {
  const int t = query.evolve.t;
  History &h = history.top();
  std::cerr << "Evolved with vrius: " << t << std::endl;
  h.add(&virus_dsu.level[t], virus_dsu.level[t] + 1);
}

/// Attack the computer with virus t. Simply add the level of the virus t to
/// the tag_damage of the virus t.
void attack(const Query &query) {
  const int t = query.attack.t;
  History &h = history.top();
  std::cerr << "Attacked with virus: " << t << std::endl;
  h.add(&virus_dsu.tag_attack[t], virus_dsu.tag_attack[t] + virus_dsu.level[t]);
}

// TODO
void reinstall(const Query &query) {
  const int k = query.reinstall.k;
  const int s = query.reinstall.s;
  History &h = history.top();
  std::cerr << "Reinstalled with k " << k << " and s " << s << std::endl;
}

// FIXME
void fusion(const Query &query) {
  const int a = query.fusion.a;
  const int b = query.fusion.b;
  History &h = history.top();
  std::cerr << "Fused " << a << " and " << b << std::endl;
  int ra = virus_dsu.find(a);
  int rb = virus_dsu.find(b);

  if (ra == rb) {
    std::cerr << "Already in the same set" << std::endl;
    return;
  }

  if (virus_dsu.size[ra] < virus_dsu.size[rb]) {
    std::swap(ra, rb);
  }
  h.add(&virus_dsu.level[ra], virus_dsu.level[ra] + virus_dsu.level[rb]);
  h.add(&virus_dsu.infected_size[ra],
        virus_dsu.infected_size[ra] + virus_dsu.infected_size[rb]);
  h.add(&virus_dsu.infected_size[rb], 0);
  // TODO: how to update the attack tag?
  virus_dsu.merge(ra, rb, h);
}

/// Print the damage of the computer k. The level of the virus in the
/// computer k, the number of computers infected by the virus in the computer k.
void status(const Query &query) {
  const int k = query.status.k;
  History &h = history.top();
  std::cerr << "Status of " << k << std::endl;
  // TODO: calculate the damage.
  int damage = 0;
  std::cout << damage << " "
            << virus_dsu.level[computer_dsu.virus_type[k]] << " "
            << virus_dsu.size[computer_dsu.virus_type[k]] << "\n";
}

/// Revert the last operation. Simply pop the last operation from the history
/// stack and revert the changes.
/// If there is no operation to revert, do nothing.
void revert(const Query &query) {
  std::cerr << "Reverted to previous state" << std::endl;
  if (history.empty())
    return;
  history.top().revert();
  history.pop();
}

const std::function<void(const Query &)> query_func[] = {
    connect, evolve, attack, reinstall, fusion, status, revert};

int main(void) {
  std::cin >> N >> Q;
  computer_dsu = ComputerDsu(N);
  virus_dsu = VirusDsu(N);
  for (int i = 0; i < Q; ++i) {
    Query query;
    std::cin >> query.type;
    if (query.type >= CONNECT && query.type <= FUSION)
      history.push(History());
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
    query_func[query.type - 1](query);
  }
}