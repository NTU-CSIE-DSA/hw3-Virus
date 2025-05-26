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
int c_rk[MN + MQ], c_par[MN + MQ], c_sz[MN + MQ], c_dmg[MN + MQ], c_vrs[MN + MQ];
int v_rk[MN], v_par[MN], v_sz[MN], v_lv[MN], v_dmg[MN];

typedef struct {
    int *ptr;
    int orig_val;
} Modify;

typedef struct {
    Modify *data;
    int cap, size;
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

    bool has_swap = (c_rk[x] < c_rk[y]);
    if (has_swap) swap(x, y);
    int sz_x = c_sz[x], sz_y = c_sz[y];
    int dmg_x = get_v(c_vrs[x]), dmg_y = get_v(c_vrs[y]);

    mod(&c_par[y], x);
    mod(&c_rk[x], c_rk[x] + (c_rk[x] == c_rk[y]));
    mod(&c_sz[x], c_sz[x] + c_sz[y]);
    mod(&c_dmg[y], c_dmg[y] + dmg_y - c_dmg[x] - dmg_x);

    int vx = find_v(c_vrs[x]), vy = find_v(c_vrs[y]);
    if (vx == vy) return;

    if (v_lv[vx] > v_lv[vy] || (v_lv[vx] == v_lv[vy] && !has_swap)) {
        mod(&v_sz[vx], v_sz[vx] + sz_y);
        mod(&v_sz[vy], v_sz[vy] - sz_y);
    } else {
        mod(&c_vrs[x], c_vrs[y]);
        mod(&c_dmg[x], c_dmg[x] + dmg_x - dmg_y);
        mod(&v_sz[vy], v_sz[vy] + sz_x);
        mod(&v_sz[vx], v_sz[vx] - sz_x);
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
    int vk = find_v(c_vrs[rk]), vs = find_v(s);
    mod(&c_sz[rk], c_sz[rk] - 1);
    mod(&node_cnt, node_cnt + 1);
    mod(&id[k], node_cnt);
    mod(&v_sz[vk], v_sz[vk] - 1);
    mod(&v_sz[vs], v_sz[vs] + 1);
    c_vrs[id[k]] = vs;
    c_dmg[id[k]] = -v_dmg[vs];
    c_rk[id[k]] = c_sz[id[k]] = 1;
    c_par[id[k]] = id[k];
}

void fusion(int a, int b) {
    a = find_v(a), b = find_v(b);
    if (a == b) return;
    if (v_rk[a] < v_rk[b]) swap(a, b);
    mod(&v_par[b], a);
    mod(&v_rk[a], v_rk[a] + (v_rk[a] == v_rk[b]));
    mod(&v_sz[a], v_sz[a] + v_sz[b]);
    mod(&v_lv[a], v_lv[a] + v_lv[b]);
    mod(&v_dmg[b], v_dmg[b] - v_dmg[a]);
}

void status(int k) {
    int rk = find_c(id[k]);
    int rv = find_v(c_vrs[rk]);
    int damage = get_c(id[k]) + get_v(c_vrs[rk]);
    printf("%lld %lld %lld\n", damage, v_lv[rv], v_sz[rv]);
}

void revert() {
    if (op_cnt == 0) return;
    int size = h_sz[--op_cnt];
    while (size--) {
        Modify *m = &st.data[--st.size];
        *(m->ptr) = m->orig_val;
    }
}

void init() {
    node_cnt = n;
    for (int i = 1; i <= n; i++) {
        c_rk[i] = v_rk[i] = c_sz[i] = v_sz[i] = v_lv[i] = 1;
        c_par[i] = v_par[i] = id[i] = c_vrs[i] = i;
        c_dmg[i] = v_dmg[i] = 0;
    }
    st.cap = 16384;
    st.size = 0;
    st.data = (Modify *)malloc(sizeof(Modify) * st.cap);
}

signed main() {
    assert(scanf("%lld %lld", &n, &q) == 2);
    init();
    for (int i = 1; i <= q; i++) {
        int t, a, b;
        h_sz[op_cnt] = 0;
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
