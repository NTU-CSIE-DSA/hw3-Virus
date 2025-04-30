/*
Create big networks and attack many times.

Force them to use lazy tags.
*/

#include <bits/stdc++.h>

#include "const.h"
#include "testlib.h"
using namespace std;

vector<int> pool;
int rnd_op() { return pool[rnd.next((int)pool.size())]; }

int main(int argc, char* argv[]) {
    registerGen(argc, argv, 1);

    int n = atoi(argv[1]);          // number of computers
    int q = atoi(argv[2]);          // number of queries
    int preset_id = atoi(argv[3]);  // preset id

    int connect = n * 2 / 3;
    int attack_and_evolve = q / 4;

    vector<int> distri = preset[preset_id];
    for (int t = 0; t < 7; ++t) {
        for (int i = 0; i < distri[t]; ++i) {
            pool.push_back(t + 1);
        }
    }

    cout << n << " " << q << "\n";

    vector<int> nums(n - 1, 0);
    iota(nums.begin(), nums.end(), 2);
    shuffle(nums.begin(), nums.end());

    // Generate connect queries
    for (int i = 0; i < connect; ++i) {
        cout << "1 " << 1 << " " << nums[i] << "\n";
    }
    q -= connect;

    // Generate attack queries
    for (int i = 0; i < attack_and_evolve; ++i) {
        int type = rnd.next(1, 2);
        int t = 1;
        if (type == 1) {
            cout << "2 " << t << "\n";
        } else {
            cout << "3 " << t << "\n";
        }
    }
    q -= attack_and_evolve;

    // Generate other queries
    for (int i = 0; i < q; ++i) {
        int type = rnd_op();

        // Avoid generating status queries in the first 10% of queries
        if (type == 6 && i < q / 5) {
            type = rnd_op();
        }

        switch (type) {
            case 1: {  // connect x y
                int x = rnd.next(1, n);
                int y = rnd.next(1, n);
                while (y == x) y = rnd.next(1, n);
                cout << "1 " << x << " " << y << "\n";
                break;
            }
            case 2: {  // evolve t
                int is_1 = rnd.next(1, 2);
                int t = rnd.next(1, n);
                if (is_1 == 1) t = 1;
                cout << "2 " << t << "\n";
                break;
            }
            case 3: {  // attack t
                int is_1 = rnd.next(1, 2);
                int t = rnd.next(1, n);
                if (is_1 == 1) t = 1;
                cout << "3 " << t << "\n";
                break;
            }
            case 4: {  // reinstall k s
                int k = rnd.next(1, n);
                int s = rnd.next(1, n);
                cout << "4 " << k << " " << s << "\n";
                break;
            }
            case 5: {  // fusion a b
                int a = rnd.next(1, n);
                int b = rnd.next(1, n);
                while (b == a) b = rnd.next(1, n);
                cout << "5 " << a << " " << b << "\n";
                break;
            }
            case 6: {  // status k
                int k = rnd.next(1, n);
                cout << "6 " << k << "\n";
                break;
            }
            case 7: {  // revert
                cout << "7\n";
            }
        }
    }

    return 0;
}
