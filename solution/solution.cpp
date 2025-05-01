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
int id[MN], node_cnt;
int c_sz[MN + MQ], c_del[MN + MQ], c_par[MN + MQ], c_dmg[MN + MQ], c_vrs[MN + MQ];
int v_sz[MN], v_par[MN], v_cnt[MN], v_lv[MN], v_dmg[MN];

typedef struct {
    int *ptr;
    int orig_val;
} Modify;

typedef struct {
    Modify *data;
    int size;
    int cap;
} ModifyStack;

ModifyStack st;
int op_cnt = 0;
int h_sz[MQ];

void mod(int *ptr, int value) {
    if (st.size == st.cap) {
        st.cap *= 2;
        st.data = (Modify *)realloc(st.data, sizeof(Modify) * st.cap);
    }
    st.data[st.size].ptr = ptr;
    st.data[st.size].orig_val = *ptr;
    st.size++;
    h_sz[op_cnt]++;
    *ptr = value;
}

int find_c(int x) { return c_par[x] == x ? x : find_c(c_par[x]); }
int find_v(int x) { return v_par[x] == x ? x : find_v(v_par[x]); }
int get_v(int k) { return k == v_par[k] ? v_dmg[k] : v_dmg[k] + get_v(v_par[k]); }
int get_c(int k) { return k == c_par[k] ? c_dmg[k] : c_dmg[k] + get_c(c_par[k]); }

void connect(int x, int y) {
    x = find_c(id[x]), y = find_c(id[y]);
    if (x == y) return;

    bool has_swap = (c_sz[x] < c_sz[y]);
    if (has_swap) swap(x, y);
    int size_x = c_sz[x] - c_del[x], size_y = c_sz[y] - c_del[y];
    int damage_x = get_v(c_vrs[x]), damage_y = get_v(c_vrs[y]);

    mod(&c_par[y], x);
    mod(&c_sz[x], c_sz[x] + c_sz[y]);
    mod(&c_del[x], c_del[x] + c_del[y]);
    mod(&c_dmg[y], c_dmg[y] + damage_y - damage_x - c_dmg[x]);

    int vx = find_v(c_vrs[x]), vy = find_v(c_vrs[y]);
    if (vx == vy) return;

    if (v_lv[vx] > v_lv[vy] || (v_lv[vx] == v_lv[vy] && !has_swap)) {
        mod(&v_cnt[vx], v_cnt[vx] + size_y);
        mod(&v_cnt[vy], v_cnt[vy] - size_y);
    } else {
        mod(&c_vrs[x], vy);
        mod(&c_dmg[x], c_dmg[x] + damage_x - v_dmg[vy]);
        mod(&v_cnt[vy], v_cnt[vy] + size_x);
        mod(&v_cnt[vx], v_cnt[vx] - size_x);
    }
}

void evolve(int t) {
    t = find_v(t);
    mod(&v_lv[t], v_lv[t] + 1);
}

void attack(int t) {
    t = find_v(t);
    mod(&v_dmg[t], v_dmg[t] + v_lv[t]);
}

void reinstall(int k, int s) {
    int rk = find_c(id[k]);
    mod(&c_del[rk], c_del[rk] + 1);
    mod(&node_cnt, node_cnt + 1);
    mod(&id[k], node_cnt);

    int vk = find_v(c_vrs[rk]), vs = find_v(s);
    if (vk != vs) {
        mod(&v_cnt[vk], v_cnt[vk] - 1);
        mod(&v_cnt[vs], v_cnt[vs] + 1);
    }

    k = id[k];
    c_vrs[k] = s;
    c_dmg[k] = -get_v(s);
    c_sz[k] = 1;
    c_par[k] = k;
    c_del[k] = 0;
}

void fusion(int a, int b) {
    a = find_v(a), b = find_v(b);
    if (a == b) return;
    if (v_sz[a] < v_sz[b]) swap(a, b);
    mod(&v_par[b], a);
    mod(&v_sz[a], v_sz[a] + v_sz[b]);
    mod(&v_cnt[a], v_cnt[a] + v_cnt[b]);
    mod(&v_cnt[b], 0);
    mod(&v_lv[a], v_lv[a] + v_lv[b]);
    mod(&v_dmg[b], v_dmg[b] - v_dmg[a]);
}

void status(int k) {
    int rk = find_c(id[k]);
    int v = find_v(c_vrs[rk]);
    int damage = get_c(id[k]) + get_v(c_vrs[rk]);
    printf("%lld %lld %lld\n", damage, v_lv[v], v_cnt[v]);
}

void revert() {
    if (op_cnt == 0) return;
    op_cnt--;
    int size = h_sz[op_cnt];
    for (int i = 0; i < size; i++) {
        Modify *m = &st.data[st.size - 1 - i];
        *(m->ptr) = m->orig_val;
    }
    st.size -= size;
    h_sz[op_cnt] = 0;
}

void init() {
    node_cnt = n;
    for (int i = 1; i <= n; i++) {
        c_sz[i] = v_sz[i] = v_cnt[i] = v_lv[i] = 1;
        c_par[i] = v_par[i] = id[i] = c_vrs[i] = i;
        c_del[i] = c_dmg[i] = v_dmg[i] = 0;
    }
    st.cap = 16;
    st.size = 0;
    st.data = (Modify *)malloc(sizeof(Modify) * st.cap);
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
        if (t <= 5) op_cnt++;
    }
    free(st.data);
}
