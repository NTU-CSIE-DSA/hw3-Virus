/// Copyright 2025 ShangJhe Li (mtmatt)
/// The program uses the DSU to maintain the simulation of the virus
/// and the computer. The program implements the following operations:
/// 1. Connect two computers.
/// 2. Evolve a virus.
/// 3. Attack all computers with virus t.
/// 4. Reinstall a computer.
/// 5. Fusion of two viruses.
/// 6. Print the status of a computer.
/// 7. Revert the last operation.
#include <functional>
#include <iostream>
#include <stack>

#include "mtmatt.h"
#define MAX_NM 500000
#define int long long

int N, Q;
std::stack<History, std::vector<History>> history;
ComputerDsu computer_dsu;
#ifdef FEATURE_VIRUS
VirusDsu virus_dsu;
#else
struct Virus {
  int level;
  int infected_size;
  int tag_attack;
  int effected_id;
};

int v_level[MAX_NM + 1];
int v_infected_size[MAX_NM + 1];
int v_tag_attack[MAX_NM + 1];
int v_effected_id[MAX_NM + 1];
int v_vsize[MAX_NM + 1];

struct Node {
  Node *next, *prev;
  int id;
} v_mem_buf[MAX_NM * 100];

Node *v_group[MAX_NM + 1];

Node *new_node(int id) {
  static Node *v_mem = v_mem_buf;
  Node *node = v_mem++;
  node->next = nullptr;
  node->prev = nullptr;
  node->id = id;
  return node;
}

void v_add_group(int id, int virus_id) {
  History &h = history.top();
  Node *node = new_node(id);
  if (v_group[virus_id] == nullptr) {
    h.add((long long *)&v_group[virus_id], (long long)node);
  } else {
    node->next = v_group[virus_id];
    h.add((long long *)&v_group[virus_id]->prev, (long long)node);
    h.add((long long *)&v_group[virus_id], (long long)node);
  }
}

#endif

int get_virus_type(ComputerDsu computers, int a) {
  return v_effected_id[computers.find(a)];
}

// TODO
void connect(const Query &query) {
  const int x = query.connect.x;
  const int y = query.connect.y;
  History &h = history.top();
  std::cerr << "Connected " << x << " and " << y << std::endl;
  if (v_level[v_effected_id[computer_dsu.find(x)]] >
      v_level[v_effected_id[computer_dsu.find(y)]]) {
    h.add(&computer_dsu.virus_type[y], computer_dsu.virus_type[x]);
  } else {
    h.add(&computer_dsu.virus_type[x], computer_dsu.virus_type[y]);
  }
  computer_dsu.merge(x, y, h);
}

/// Evolve the virus t. Simply add one to the level of the virus t.
void evolve(const Query &query) {
  const int t = query.evolve.t;
  History &h = history.top();
  std::cerr << "Evolved virus " << t << std::endl;
  h.add(&v_level[v_effected_id[t]], v_level[v_effected_id[t]] + 1);
}

/// Attack the computer with virus t. Simply add the level of the virus t to
/// the tag_damage of the virus t.
void attack(const Query &query) {
  const int t = query.attack.t;
  History &h = history.top();
  std::cerr << "Attacked virus " << t << std::endl;
  h.add(&v_tag_attack[v_effected_id[t]],
        v_tag_attack[v_effected_id[t]] + v_level[v_effected_id[t]]);
}

// TODO
void reinstall(const Query &query) {
  const int k = query.reinstall.k;
  const int s = query.reinstall.s;
  History &h = history.top();
  std::cerr << "Reinstalled with k " << k << " and s " << s << std::endl;
  int virus_id = get_virus_type(computer_dsu, k);
  int old_virus_id = computer_dsu.virus_type[k];
  if (virus_id != old_virus_id) {
    h.add(&computer_dsu.virus_type[k], virus_id);
    h.add(&v_effected_id[old_virus_id], virus_id);
    h.add(&v_effected_id[virus_id], old_virus_id);
    v_group[old_virus_id]->id = s;
    v_add_group(s, virus_id);
  }
}

void fusion(const Query &query) {
  const int a = query.fusion.a;
  const int b = query.fusion.b;
  History &h = history.top();
  std::cerr << "Fusion of " << a << " and " << b << std::endl;
  int a_virus = get_virus_type(computer_dsu, a);
  int b_virus = get_virus_type(computer_dsu, b);
  if (v_vsize[a_virus] < v_vsize[b_virus]) {
    std::swap(a_virus, b_virus);
  }
  h.add(&v_vsize[a_virus], v_vsize[a_virus] + v_vsize[b_virus]);
  h.add(&v_vsize[b_virus], 0);
  h.add(&v_effected_id[b_virus], a_virus);

  Node *node = v_group[b_virus];
  while (node != nullptr) {
    Node *next = node->next;
    h.add((long long *)&node->next, (long long)v_group[a_virus]);
    if (v_group[a_virus] != nullptr)
      h.add((long long *)&v_group[a_virus]->prev, (long long)node);
    h.add((long long *)&v_group[a_virus], (long long)node);
    node = next;
  }
  v_group[b_virus] = nullptr;
}

/// Print the damage of the computer k. The level of the virus in the
/// computer k, the number of computers infected by the virus in the computer k.
void status(const Query &query) {
  const int k = query.status.k;
  History &h = history.top();
  std::cerr << "Status of computer " << k << std::endl;
  int virus_id = get_virus_type(computer_dsu, k);
  int damage = v_tag_attack[v_effected_id[virus_id]];
  int size = v_infected_size[v_effected_id[virus_id]];
  int level = v_level[v_effected_id[virus_id]];
  std::cout << damage << " " << size << " " << level << std::endl;
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

signed main(void) {
  std::cin >> N >> Q;
  ensure(N <= MAX_NM && Q <= MAX_NM);
  computer_dsu = ComputerDsu(N);
  for (int i = 0; i < Q; ++i) {
    Query query;
    std::cin >> query.type;
    ensure(query.type >= CONNECT && query.type <= REVERT);
    if (query.type >= CONNECT && query.type <= FUSION)
      history.push(History());
    switch (query.type) {
    case CONNECT:
      std::cin >> query.connect.x >> query.connect.y;
      ensure(query.connect.x != query.connect.y);
      ensure(query.connect.x > 0 && query.connect.x <= N);
      ensure(query.connect.y > 0 && query.connect.y <= N);
      break;
    case EVOLVE:
      std::cin >> query.evolve.t;
      ensure(query.evolve.t > 0 && query.evolve.t <= N);
      break;
    case ATTACK:
      std::cin >> query.attack.t;
      ensure(query.attack.t > 0 && query.attack.t <= N);
      break;
    case REINSTALL:
      std::cin >> query.reinstall.k >> query.reinstall.s;
      ensure(query.reinstall.k > 0 && query.reinstall.k <= N);
      ensure(query.reinstall.s > 0 && query.reinstall.s <= N);
      break;
    case FUSION:
      std::cin >> query.fusion.a >> query.fusion.b;
      ensure(query.fusion.a != query.fusion.b);
      ensure(query.fusion.a > 0 && query.fusion.a <= N);
      ensure(query.fusion.b > 0 && query.fusion.b <= N);
      break;
    case STATUS:
      std::cin >> query.status.k;
      ensure(query.status.k > 0 && query.status.k <= N);
      break;
    case REVERT:
      break;
    default:
      throw std::invalid_argument("Invalid query type");
    }
    query_func[query.type - 1](query);
  }
}