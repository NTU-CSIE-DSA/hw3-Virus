## Problem Description

Please refer to [NTU COOL](https://cool.ntu.edu.tw/courses/47166/assignments/306460).

## Hints

```c
typedef struct {
    long long *ptr;
    long long original_value;
} Modify;

typedef struct {
    Modify *data;
    int size, capacity;
} ModifyStack;

ModifyStack st;
int op_count = 0;
int hist_size[MQ];  // number of modifications for each operation

void modify(long long *ptr, long long value) {
    if (st.size == st.capacity) {
        st.capacity *= 2;
        st.data = (Modify *)realloc(st.data, sizeof(Modify) * st.capacity);
    }
    st.data[st.size].ptr = ptr;
    st.data[st.size].original_value = *ptr;
    st.size++;
    hist_size[op_count]++;
    *ptr = value;
}
```

## Samples

### Sample Input 1

```
5 15
1 1 2
3 1
3 4
6 2
2 4
1 3 4
3 4
6 4
1 5 2
3 5
2 4
3 4
6 1
6 3
6 5
```

### Sample Output 1

```
1 1 2
3 2 2
2 1 3
5 3 2
1 1 3
```

### Sample Input 2

```
5 12
1 1 2
3 1
3 4
2 2
2 4
1 3 4
3 4
5 4 5
3 4
4 4 2
6 1
6 4
```

### Sample Output 2

```
1 1 2
0 2 1
```

### Sample Input 3

```
5 37
1 1 2
3 1
3 4
6 2
2 4
1 3 4
3 4
6 4
1 5 2
3 5
2 4
3 4
6 1
6 3
6 5
7
7
7
7
5 4 5
3 4
6 1
6 3
6 5
4 4 1
6 1
2 1
3 1
6 2
6 3
6 4
7
7
7
4 4 2
6 1
6 4
```

### Sample Output 3

```
1 1 2
3 2 2
2 1 3
5 3 2
1 1 3
1 1 2
5 3 3
3 3 3
1 1 3
3 2 3
5 3 2
2 2 3
1 1 2
0 1 1
```
