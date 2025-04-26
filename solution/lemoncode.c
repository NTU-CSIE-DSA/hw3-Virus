#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#define MAXN ((int)(500000 + 5))
typedef long long int ll;
void swap(ll *a, ll *b) {
  ll tmp = *a;
  *a = *b;
  *b = tmp;
}
typedef struct Vector {
  void **ptrs;
  ll *vals;
  ll cap;
  ll sz;
} Vector;

void init_Vector(Vector *vec) {
  vec->ptrs = malloc(sizeof(void *));
  vec->vals = malloc(sizeof(ll));
  vec->cap = 1;
  vec->sz = 0;
}

void Vector_push_back(Vector *vec, void *ptr, ll val) {
  if (vec->sz == vec->cap) {
    vec->cap <<= 1;
    vec->ptrs = realloc(vec->ptrs, sizeof(void *) * vec->cap);
    vec->vals = realloc(vec->vals, sizeof(ll) * vec->cap);
  }
  vec->ptrs[vec->sz] = ptr;
  vec->vals[vec->sz] = val;
  ++vec->sz;
}

void Vector_pop_back(Vector *vec) {
  if (vec->sz == 0)
    return;
  --vec->sz;
}

void del_Vector(Vector *vec) {
  free(vec->ptrs);
  free(vec->vals);
}

typedef struct Stack {
  Vector *vec;
  ll cap;
  ll sz;
} Stack;

void init_Stack(Stack *st) {
  st->vec = malloc(sizeof(Vector));
  st->cap = 1;
  st->sz = 0;
}

void Stack_push(Stack *st, Vector vec) {
  if (st->sz == st->cap) {
    st->cap <<= 1;
    st->vec = realloc(st->vec, sizeof(Vector) * st->cap);
  }
  st->vec[st->sz] = vec;
  ++st->sz;
}

Vector *Stack_top(Stack *st) { return &st->vec[st->sz - 1]; }
void Stack_pop(Stack *st) { --st->sz; }
void del_Stack(Stack *st) {
  for (int i = 0; i < st->sz; ++i) {
    del_Vector(&st->vec[i]);
  }
  free(st->vec);
}

Stack modify_stk;
void checkpoint() {
  Vector vec;
  init_Vector(&vec);
  Stack_push(&modify_stk, vec);
}

void modify(void *ptr, ll val) {
  ll old_val = *(ll *)ptr;
  *(ll *)ptr = val;
  Vector *vec = Stack_top(&modify_stk);
  Vector_push_back(vec, ptr, old_val);
}

typedef struct DSU {
  ll sz; // total size
  ll c_rt[MAXN << 1], c_rk[MAXN << 1];
  ll c_del[MAXN << 1], c_dif[MAXN << 1], c_virus[MAXN << 1];
  ll v_rt[MAXN], v_rk[MAXN];
  ll v_dif[MAXN], v_count[MAXN], v_level[MAXN];
} DSU;

DSU dsu;
ll c[MAXN]; // computer in network id

void init_DSU(ll n) {
  dsu.sz = n;
  for (int i = 0; i < MAXN; ++i) {
    dsu.v_rt[i] = i;
    dsu.v_rk[i] = 1;
    dsu.v_dif[i] = 0;
    dsu.v_count[i] = 1;
    dsu.v_level[i] = 1;
  }
  for (int i = 0; i < (MAXN << 1); ++i) {
    dsu.c_rt[i] = i;
    dsu.c_rk[i] = 1;
    dsu.c_del[i] = 0;
    dsu.c_dif[i] = 0;
    dsu.c_virus[i] = i;
  }
}

ll network_findroot(ll x) {
  return dsu.c_rt[x] == x ? x : network_findroot(dsu.c_rt[x]);
}

ll virus_findroot(ll x) {
  return dsu.v_rt[x] == x ? x : virus_findroot(dsu.v_rt[x]);
}

ll virus_find_damage(ll x) {
  return dsu.v_dif[x] + (dsu.v_rt[x] == x ? 0 : virus_find_damage(dsu.v_rt[x]));
}

ll network_find_damage(ll x) {
  if (dsu.c_rt[x] != x) {
    return dsu.c_dif[x] + network_find_damage(dsu.c_rt[x]);
  }
  return dsu.c_dif[x] + virus_find_damage(dsu.c_virus[x]);
}

ll virus_find_level(ll x) { return dsu.v_level[virus_findroot(x)]; }

void _network_union(int k1, int k2) { // union by id
  assert(k1 && k2);
  ll cr1 = network_findroot(k1);
  ll cr2 = network_findroot(k2);
  if (cr1 == cr2)
    return;
  ll vr1 = virus_findroot(dsu.c_virus[cr1]);
  ll vr2 = virus_findroot(dsu.c_virus[cr2]);
  bool vr1_stronger = dsu.v_level[vr1] >= dsu.v_level[vr2];
  if (dsu.c_rk[cr1] < dsu.c_rk[cr2]) {
    swap(&cr1, &cr2);
    swap(&vr1, &vr2);
    vr1_stronger = !vr1_stronger;
  }

  ll c_sz1 = dsu.c_rk[cr1] - dsu.c_del[cr1];
  ll c_sz2 = dsu.c_rk[cr2] - dsu.c_del[cr2];

  modify(&dsu.c_rt[cr2], cr1);
  modify(&dsu.c_rk[cr1], dsu.c_rk[cr1] + dsu.c_rk[cr2]);
  modify(&dsu.c_del[cr1], dsu.c_del[cr1] + dsu.c_del[cr2]);

  ll v_d1 = virus_find_damage(dsu.c_virus[cr1]);
  ll v_d2 = virus_find_damage(dsu.c_virus[cr2]);

  modify(&dsu.c_dif[cr2], (v_d2 + dsu.c_dif[cr2]) - (v_d1 + dsu.c_dif[cr1]));
  if (vr1 == vr2)
    return;
  if (vr1_stronger) {
    modify(&dsu.v_count[vr1], dsu.v_count[vr1] + c_sz2);
    modify(&dsu.v_count[vr2], dsu.v_count[vr2] - c_sz2);
  } else {
    modify(&dsu.c_virus[cr1], vr2);
    modify(&dsu.c_dif[cr1], dsu.c_dif[cr1] + v_d1 - dsu.v_dif[vr2]);
    modify(&dsu.v_count[vr2], dsu.v_count[vr2] + c_sz1);
    modify(&dsu.v_count[vr1], dsu.v_count[vr1] - c_sz1);
  }
}
// operation 1
void network_merge(int x, int y) { // union by computer
  checkpoint();
  _network_union(c[x], c[y]);
}
// operation 2
void evolve(int t) {
  checkpoint();
  ll virus_r = virus_findroot(t);
  modify(&dsu.v_level[virus_r], dsu.v_level[virus_r] + 1);
}
// operation 3
void attack(int t) {
  checkpoint();
  ll virus_r = virus_findroot(t);
  modify(&dsu.v_dif[virus_r], dsu.v_dif[virus_r] + dsu.v_level[virus_r]);
}
// operation 4
void reinstall(int k, int s) {
  checkpoint();
  ll old_network_r = network_findroot(c[k]);
  // printf("old_network_r = %lld, virus_r = %lld\n", old_network_r, virus_r);
  // Delete the old node
  modify(&dsu.c_del[old_network_r], dsu.c_del[old_network_r] + 1);
  // Add a new node
  modify(&dsu.sz, dsu.sz + 1);
  modify(&c[k], dsu.sz);
  ll new_network_r = c[k];
  ll old_virus_r = virus_findroot(dsu.c_virus[old_network_r]);
  ll new_virus_r = virus_findroot(s);
  if (old_virus_r != new_virus_r) {
    modify(&dsu.v_count[old_virus_r], dsu.v_count[old_virus_r] - 1);
    modify(&dsu.v_count[new_virus_r], dsu.v_count[new_virus_r] + 1);
  }
  modify(&dsu.c_virus[new_network_r], s);
  // So it will become 0
  modify(&dsu.c_dif[new_network_r], -virus_find_damage(s));
}
// operation 5
void fuse(int a, int b) {
  checkpoint();
  ll r1 = virus_findroot(a);
  ll r2 = virus_findroot(b);
  if (r1 == r2)
    return;
  if (dsu.v_rk[r1] < dsu.v_rk[r2])
    swap(&r1, &r2);
  modify(&dsu.v_rt[r2], r1);
  modify(&dsu.v_rk[r1], dsu.v_rk[r1] + dsu.v_rk[r2]);
  modify(&dsu.v_count[r1], dsu.v_count[r1] + dsu.v_count[r2]);
  modify(&dsu.v_count[r2], 0);
  modify(&dsu.v_level[r1], dsu.v_level[r1] + dsu.v_level[r2]);
  modify(&dsu.v_dif[r2], dsu.v_dif[r2] - dsu.v_dif[r1]);
}
// operation 6
void status(int k) {
  ll network_r = network_findroot(c[k]);
  ll virus_r = virus_findroot(dsu.c_virus[network_r]);
  // printf("network_r = %lld, virus_r = %lld\n", network_r, virus_r);
  printf("%lld %lld %lld\n", network_find_damage(c[k]),
         virus_find_level(virus_r), dsu.v_count[virus_r]);
}
// operation 7
void revert() {
  if (modify_stk.sz <= 1)
    return;
  Vector *vec = Stack_top(&modify_stk);
  for (int j = vec->sz - 1; j >= 0; --j) {
    void *ptr = vec->ptrs[j];
    ll val = vec->vals[j];
    *((ll *)ptr) = val;
  }
  Stack_pop(&modify_stk);
}

int main(int argc, char *argv[]) {
  ll n, q;
  scanf("%lld%lld", &n, &q);
  init_Stack(&modify_stk);
  init_DSU(n);
  for (int i = 1; i <= n; ++i) {
    c[i] = i;
  }
  checkpoint();
  while (q--) {
    int op;
    scanf("%d", &op);
    switch (op) {
    case 1: {
      int x, y;
      scanf("%d%d", &x, &y);
      network_merge(x, y);
      break;
    }
    case 2: {
      int t;
      scanf("%d", &t);
      evolve(t);
      break;
    }
    case 3: {
      int t;
      scanf("%d", &t);
      attack(t);
      break;
    }
    case 4: {
      int k, s;
      scanf("%d%d", &k, &s);
      reinstall(k, s);
      break;
    }
    case 5: {
      int a, b;
      scanf("%d%d", &a, &b);
      fuse(a, b);
      break;
    }
    case 6: {
      int k;
      scanf("%d", &k);
      status(k);
      break;
    }
    case 7: {
      revert();
      break;
    }
    }
    // // Network
    // for (int i = 1; i <= n; ++i) {
    //   int r_inf = network_findroot(c[i]);
    //   if (r_inf == c[i]) {
    //     printf("Computer %d: root %d, d_dif %lld, damage %lld, rk %lld, virus
    //     "
    //            "%lld\n",
    //            i, r_inf, dsu.c_dif[c[i]], network_find_damage(c[i]),
    //            dsu.c_rk[c[i]], dsu.c_virus[c[i]]);
    //   } else {
    //     printf("Computer %d: root %d, d_dif %lld, damage %lld\n", i, r_inf,
    //            dsu.c_dif[c[i]], network_find_damage(c[i]));
    //   }
    // }
    // // Virus
    // for (int i = 1; i <= n; ++i) {
    //   int r = virus_findroot(i);
    //   if (r == i) {
    //     printf("Virus %d: v_dif %lld, damage %lld, level %lld\n", i,
    //            dsu.v_dif[i], virus_find_damage(i), dsu.v_level[i]);
    //   } else {
    //     printf("Virus %d: v_dif %lld, damage %lld, Virus %d\n", i,
    //     dsu.v_dif[i],
    //            virus_find_damage(i), r);
    //   }
    // }
  }
}
