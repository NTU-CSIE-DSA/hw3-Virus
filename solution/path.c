#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#define int long long
#define MN 500005
#define MQ 500005
#define swap(a, b)    \
    {                 \
        int temp = a; \
        a = b;        \
        b = temp;     \
    }

int n, q;
int id[MN], node_count;
int c_size[MN + MQ], c_delete[MN + MQ], c_parent[MN + MQ];
int v_size[MN], v_parent[MN];
int c_virus[MN + MQ], c_damage[MN + MQ];
int v_count[MN], v_level[MN], v_damage[MN];

typedef struct {
    int *ptr;
    int original_value;
} Modify;

typedef struct {
    Modify *data;
    int size;
    int capacity;
} ModifyStack;

ModifyStack mod_stack;
int op_count = 0;
int hist_size[MQ];

void modify(int *ptr, int value) {
    if (mod_stack.size == mod_stack.capacity) {
        mod_stack.capacity *= 2;
        mod_stack.data = (Modify *)realloc(mod_stack.data, sizeof(Modify) * mod_stack.capacity);
    }
    mod_stack.data[mod_stack.size].ptr = ptr;
    mod_stack.data[mod_stack.size].original_value = *ptr;
    mod_stack.size++;
    hist_size[op_count]++;
    *ptr = value;
}

int find_root(int *p, int *d, int x) {
    if (p[x] == x) return x;
    int r = find_root(p, d, p[x]);
    if (p[x] != r) {
        modify(&d[x], d[x] + d[p[x]]);
        modify(&p[x], r);
    }
    return r;
}

int find_root_no(int *p, int x) {
    while (p[x] != x) x = p[x];
    return x;
}

int get_damage_virus(int k) {
    int damage = 0;
    while (v_parent[k] != k) {
        damage += v_damage[k];
        k = v_parent[k];
    }
    damage += v_damage[k];
    return damage;
}

int get_damage(int k) {
    int damage = 0;
    k = id[k];
    while (c_parent[k] != k) {
        damage += c_damage[k];
        k = c_parent[k];
    }
    return damage + c_damage[k] + get_damage_virus(c_virus[k]);
}

void connect(int c1, int c2) {
    int rc1 = find_root(c_parent, c_damage, id[c1]);
    int rc2 = find_root(c_parent, c_damage, id[c2]);
    if (rc1 == rc2) return;

    int size1 = c_size[rc1] - c_delete[rc1];
    int size2 = c_size[rc2] - c_delete[rc2];
    int damage1 = get_damage_virus(c_virus[rc1]);
    int damage2 = get_damage_virus(c_virus[rc2]);

    bool is_c1 = (c_size[rc1] >= c_size[rc2]);
    if (is_c1) {
        modify(&c_parent[rc2], rc1);
        modify(&c_size[rc1], c_size[rc1] + c_size[rc2]);
        modify(&c_delete[rc1], c_delete[rc1] + c_delete[rc2]);
        modify(&c_damage[rc2], c_damage[rc2] + damage2 - damage1 - c_damage[rc1]);
    } else {
        modify(&c_parent[rc1], rc2);
        modify(&c_size[rc2], c_size[rc1] + c_size[rc2]);
        modify(&c_delete[rc2], c_delete[rc1] + c_delete[rc2]);
        modify(&c_damage[rc1], c_damage[rc1] + damage1 - damage2 - c_damage[rc2]);
    }

    int rv1 = find_root(v_parent, v_damage, c_virus[rc1]);
    int rv2 = find_root(v_parent, v_damage, c_virus[rc2]);
    if (rv1 == rv2) return;

    bool is_v1 = (v_level[rv1] >= v_level[rv2]);
    if (is_c1 && is_v1) {
        modify(&v_count[rv1], v_count[rv1] + size2);
        modify(&v_count[rv2], v_count[rv2] - size2);
    } else if (is_c1 && !is_v1) {
        modify(&c_virus[rc1], rv2);
        modify(&c_damage[rc1], c_damage[rc1] + damage1 - v_damage[rv2]);
        modify(&v_count[rv2], v_count[rv2] + size1);
        modify(&v_count[rv1], v_count[rv1] - size1);
    } else if (!is_c1 && is_v1) {
        modify(&c_virus[rc2], rv1);
        modify(&c_damage[rc2], c_damage[rc2] + damage2 - v_damage[rv1]);
        modify(&v_count[rv1], v_count[rv1] + size2);
        modify(&v_count[rv2], v_count[rv2] - size2);
    } else {
        modify(&v_count[rv2], v_count[rv2] + size1);
        modify(&v_count[rv1], v_count[rv1] - size1);
    }
}

void evolve(int t) {
    t = find_root(v_parent, v_damage, t);
    modify(&v_level[t], v_level[t] + 1);
}

void attack(int t) {
    t = find_root(v_parent, v_damage, t);
    modify(&v_damage[t], v_damage[t] + v_level[t]);
}

void reinstall(int k, int s) {
    int rck = find_root(c_parent, c_damage, id[k]);
    modify(&c_delete[rck], c_delete[rck] + 1);
    modify(&node_count, node_count + 1);
    modify(&id[k], node_count);

    int rvk = find_root(v_parent, v_damage, c_virus[rck]);
    int rvs = find_root(v_parent, v_damage, s);
    if (rvk != rvs) {
        modify(&v_count[rvk], v_count[rvk] - 1);
        modify(&v_count[rvs], v_count[rvs] + 1);
    }

    k = id[k];
    modify(&c_virus[k], s);
    modify(&c_damage[k], -get_damage_virus(s));
}

void fusion(int v1, int v2) {
    int rv1 = find_root(v_parent, v_damage, v1);
    int rv2 = find_root(v_parent, v_damage, v2);
    if (rv1 == rv2) return;

    if (v_size[rv1] < v_size[rv2]) swap(rv1, rv2);
    modify(&v_parent[rv2], rv1);
    modify(&v_size[rv1], v_size[rv1] + v_size[rv2]);
    modify(&v_count[rv1], v_count[rv1] + v_count[rv2]);
    modify(&v_count[rv2], 0);
    modify(&v_level[rv1], v_level[rv1] + v_level[rv2]);
    modify(&v_damage[rv2], v_damage[rv2] - v_damage[rv1]);
}

void status(int k) {
    int rck = find_root_no(c_parent, id[k]);
    int rvk = find_root_no(v_parent, c_virus[rck]);
    int damage = get_damage(k);
    printf("%lld %lld %lld\n", damage, v_level[rvk], v_count[rvk]);
}

void revert() {
    if (op_count == 0) return;
    op_count--;
    int size = hist_size[op_count];
    for (int i = 0; i < size; i++) {
        Modify *m = &mod_stack.data[mod_stack.size - 1 - i];
        *(m->ptr) = m->original_value;
    }
    mod_stack.size -= size;
    hist_size[op_count] = 0;
}

void init() {
    node_count = n;
    for (int i = 1; i <= n + q; i++) {
        c_size[i] = 1;
        c_parent[i] = i;
        c_delete[i] = 0;
    }
    for (int i = 1; i <= n; i++) {
        v_size[i] = 1;
        v_parent[i] = i;
        id[i] = i;
        c_virus[i] = i;
        c_damage[i] = 0;
        v_count[i] = 1;
        v_level[i] = 1;
        v_damage[i] = 0;
    }
    mod_stack.capacity = 16;
    mod_stack.size = 0;
    mod_stack.data = (Modify *)malloc(sizeof(Modify) * mod_stack.capacity);
}

signed main() {
    assert(scanf("%lld %lld", &n, &q) == 2);
    init();
    for (int i = 1; i <= q; i++) {
        int t, a, b;
        assert(scanf("%lld", &t) == 1);
        if (t == 1) {
            assert(scanf("%lld %lld", &a, &b) == 2);
            connect(a, b);
        } else if (t == 2) {
            assert(scanf("%lld", &a) == 1);
            evolve(a);
        } else if (t == 3) {
            assert(scanf("%lld", &a) == 1);
            attack(a);
        } else if (t == 4) {
            assert(scanf("%lld %lld", &a, &b) == 2);
            reinstall(a, b);
        } else if (t == 5) {
            assert(scanf("%lld %lld", &a, &b) == 2);
            fusion(a, b);
        } else if (t == 6) {
            assert(scanf("%lld", &a) == 1);
            status(a);
        } else if (t == 7) {
            revert();
        }
        if (t <= 5) op_count++;
    }
    free(mod_stack.data);
}
