#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#define int long long
#define MN 1000005
#define MQ 1000005
#define swap(a, b)    \
    {                 \
        int temp = a; \
        a = b;        \
        b = temp;     \
    }

// === Global variables ===

int n, q;

// === History ===

typedef struct {
    int *ptr;
    int original_value;
} Modify;

typedef struct {
    int size;
    Modify m[10];
} History;

int top = 0;
History history[MQ];

// === Functions ===

void modify(int *ptr, int value) {
    History *h = &history[top];
    h->m[h->size].ptr = ptr;
    h->m[h->size].original_value = *ptr;
    h->size++;
    *ptr = value;
}

// === Operations ===

// operation 1
void connect(int c1, int c2) {}

// operation 2
void evolve(int t) {}

// operation 3
void attack(int t) {}

// operation 4
void reinstall(int k, int s) {}

// operation 5
void fusion(int v1, int v2) {}

// operation 6
void status(int k) {}

// operation 7
void revert() {
    if (top == 0) return;

    top--;
    History *h = &history[top];
    for (int i = h->size - 1; i >= 0; i--) {
        Modify *m = &h->m[i];
        *(m->ptr) = m->original_value;
    }
    h->size = 0;
}

signed main() {
    assert(scanf("%lld %lld", &n, &q) == 2);

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
        if (t <= 5) top++;
    }

    return 0;
}
