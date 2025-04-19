#include "testlib.h"
using namespace std;

const int MN = 1000000;
const int MQ = 1000000;

int main() {
    registerValidation();

    int n = inf.readInt(1, MN, "n");
    inf.readSpace();
    int q = inf.readInt(1, MQ, "q");
    inf.readEoln();

    for (int i = 0; i < q; ++i) {
        int t = inf.readInt(1, 7, "t");
        if (t != 7) inf.readSpace();
        switch (t) {
            case 1: {  // connect x y
                int x = inf.readInt(1, n, "x");
                inf.readSpace();
                int y = inf.readInt(1, n, "y");
                ensuref(x != y, "x and y must be different");
                break;
            }
            case 2: {  // evolve t
                inf.readInt(1, n, "t");
                break;
            }
            case 3: {  // attack t
                inf.readInt(1, n, "t");
                break;
            }
            case 4: {  // reinstall k s
                inf.readInt(1, n, "k");
                inf.readSpace();
                inf.readInt(1, n, "s");
                break;
            }
            case 5: {  // fusion a b
                int a = inf.readInt(1, n, "a");
                inf.readSpace();
                int b = inf.readInt(1, n, "b");
                ensuref(a != b, "a and b must be different");
                break;
            }
            case 6: {  // status k
                inf.readInt(1, n, "k");
                break;
            }
            case 7: {  // revert
                break;
            }
        }
        inf.readEoln();
    }

    inf.readEof();
    return 0;
}
