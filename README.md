# [HW3-P4] Device Symptom Aggregation

## Solution

Hope you are enjoying the course so far! This problem is a bit more complex than the previous ones, but it can be solved efficiently with a careful design.

### How to Revert

In this problem, the `revert` operation means undoing a previous change, restoring the system to the state before the operation. This can be implemented using a stack.

Every time a modification occurs, store a pointer to the variable along with its original value. To revert, simply pop from the stack and restore the variable using the stored pointer.

By recording the pointer directly, we eliminate the need to know which variable was modified, simplifying the implementation significantly.

For additional context, refer to the Hint section in the problem description and the sample code below.

### Naive Approach

Use an array `group` to record the network to which each computer belongs. If `group[a] == group[b]`, then computers a and b are in the same network. A similar method can be used to maintain virus fusion relationships.

For the `attack` operation, we can loop through all infected computers and update their damage directly, resulting in $O(N)$ time complexity per operation.

We'll skip the implementation details here. This results in an overall time complexity of $O((N + Q) \times N)$, which is sufficient to pass subtask 1.

### Lazy Tags

To make all operations efficient enough to pass all test cases, we aim for $O(\log N)$ per operation. This can be done using tree-based disjoint sets with optimizations like union by rank and path compression.

We will need two separate disjoint sets: one for computer networks ($\text{DSU}_\text{c}$) and another for virus fusion ($\text{DSU}_\text{v}$). The `connect` operation corresponds to a union in $\text{DSU}_\text{c}$, while the `fusion` operation corresponds to a union in $\text{DSU}_\text{v}$.

To determine which fused virus has infected a computer, first find its root in $\text{DSU}_\text{c}$ (`rc`), then get the virus `vc` associated with `rc`. Finally, find the root of `vc` in $\text{DSU}_\text{v}$ (`rvc`), which represents the current virus identity after fusion.

Instead of updating all infected computers during an `attack`, we use lazy propagation: record the damage increment on `rvc` as `rvc.dmg_tag`. When querying later, traverse up the virus tree and sum all damage tags to get the total damage. This avoids redundant updates and improves performance.

However, `fusion` requires careful handling. When merging two viruses `v1` and `v2`, with roots `rv1` and `rv2`, and attaching `rv2` under `rv1`, the subtree under `rv2` would incorrectly inherit `rv1.dmg_tag`.

To fix this, subtract `rv1.dmg_tag` from `rv2.dmg_tag` during the merge. This neutralizes the contribution from `rv1.dmg_tag` when querying descendants of `rv2`.

The `connect` operation is the most complex. Besides updating damage tags similarly, you also need to determine if the new root in $\text{DSU}_\text{c}$ must adopt a different virus. There are two cases to handle, explained in the Implementation section below.

### About Path Compression

It is well known that when a disjoint set uses both union by rank and path compression, the time complexity per query or merge can be reduced to an **average** of $O(\alpha(N))$, where $\alpha(x)$ is the inverse Ackermann function.

However, it's important to note that this complexity is **amortized**. In the worst case, it is still $O(\log N)$. Once you introduce the `revert` operation, this amortization no longer holds, because in the worst case, frequent revert/undo actions can lead to repeated operations that bring the time complexity back up to $O(\log N)$.

Therefore, in the context of this problem, path compression is not strictly necessary. You may choose to skip path compression for a slightly simpler implementation, and it will still be sufficient to pass all test cases.

## Implementation

### Variables

- `id[]`: Maps each computer id to its current node id in $\text{DSU}_\text{c}$.
- `node_cnt`: Total number of $\text{DSU}_\text{c}$ nodes, including "fake" nodes by `reinstall` operations.
- `c_rk[]`, `c_par[]`: Rank and parent arrays for $\text{DSU}_\text{c}$.
- `c_sz[]`: Number of computers in a network, stored in $\text{DSU}_\text{c}$ root.
- `c_dmg[]`: Damage tag for a node in $\text{DSU}_\text{c}$.
- `c_vrs[]`: The corresponding virus node id of a network, stored in $\text{DSU}_\text{c}$ root.
- `v_rk[]`, `v_par[]`: Rank and parent arrays for $\text{DSU}_\text{v}$.
- `v_sz[]`: Number of computers infected by the (fused) virus, stored in $\text{DSU}_\text{v}$ root.
- `v_dmg[]`: Damage tag for a node in $\text{DSU}_\text{v}$.
- `v_lv[]`: Virus level, stored in $\text{DSU}_\text{v}$ root.

### Connect Operation

This is the most complex operation. First, check if the roots of the two computers are the same; if so, return.

```c
x = find_c(id[x]), y = find_c(id[y]);
if (x == y) return;
```

Ensure `x` is the root with higher rank by swapping. Also record the sizes and damage values before updates, since they may change later. `dmg_x` represents the total damage from the virus root to computer `x`.

```c
bool has_swap = (c_rk[x] < c_rk[y]);
if (has_swap) swap(x, y);
int sz_x = c_sz[x], sz_y = c_sz[y];
int dmg_x = get_v(c_vrs[x]), dmg_y = get_v(c_vrs[y]);
```

Next, we update the parent, rank, size, and damage tags of the computer nodes in $\text{DSU}_\text{c}$. The line `c_dmg[y] = c_dmg[y] + dmg_y - c_dmg[x] - dmg_x` is written this way because, after the union, the path from `y` will no longer pass through `dmg_y`, but will instead pass through `c_dmg[x]` and `dmg_x`. Therefore, we need to add back what was lost and subtract what was newly added to keep the total damage consistent.

```c
mod(&c_par[y], x);
mod(&c_rk[x], c_rk[x] + (c_rk[x] == c_rk[y]));
mod(&c_sz[x], c_sz[x] + c_sz[y]);
mod(&c_dmg[y], c_dmg[y] + dmg_y - c_dmg[x] - dmg_x);
```

Then check if the virus roots are the same; if so, return.

```c
int vx = find_v(c_vrs[x]), vy = find_v(c_vrs[y]);
if (vx == vy) return;
```

Finally, be cautious with the two cases:

1. If the computer root keeps its virus, update both virus sizes.
2. If the computer root changes virus, also update the virus id and damage tag. The damage tag again follows a previous logic.

```c
if (v_lv[vx] > v_lv[vy] || (v_lv[vx] == v_lv[vy] && !has_swap)) {
    mod(&v_sz[vx], v_sz[vx] + sz_y);
    mod(&v_sz[vy], v_sz[vy] - sz_y);
} else {
    mod(&c_vrs[x], c_vrs[y]);
    mod(&c_dmg[x], c_dmg[x] + dmg_x - dmg_y);
    mod(&v_sz[vy], v_sz[vy] + sz_x);
    mod(&v_sz[vx], v_sz[vx] - sz_x);
}
```

### Evolve Operation

Find the root of the virus node and increase its level by 1.

### Attack Operation

Similar to the `evolve` operation, find the root of the virus node and increment its damage tag by its level.

### Reinstall Operation

Due to disjoint set properties, removing a node is not viable since it could lead to $O(N)$ time, which is unacceptable.

Instead, keep the old node and create a new node in $\text{DSU}\text{c}$, then map the computer to the new node. This is why $\text{DSU}\text{c}$ needs `MAXN + MAXQ` space.

Also update network and virus sizes, and set the new node's damage tag to the negation of its virus’s current tag so that the damage query cancels out.

```c
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
```

### Fusion Operation

Standard union in $\text{DSU}_\text{v}$ with updates to size, level, and damage tag. The damage tag update follows the lazy strategy described above.

```c
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
```

### Status Operation

The method for calculating damage has been described above: it involves traversing from the given computer to the computer root, and then from the virus to the virus root, accumulating all the damage tags along the way.

### Revert Operation

The approach for recording modifications has already been explained above, so this operation is straightforward. Please refer directly to the `revert` function in the sample code below.

### Complexity Analysis

- All operations except `revert` run in $O(\log (N + Q))$ in the worst case.
- The `revert` operation also runs in $O(\log (N + Q))$, as it do exactly the same things as the previous operations, but in reverse order.
- Total complexity: $O(Q \log (N + Q))$ worst case, which is sufficient to pass all test cases.

## Sample Code

```c
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
```
