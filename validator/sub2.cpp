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

    bool has_op6 = false;

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
                quitf(_fail, "should not have operation 4");
            }
            case 5: {  // fusion a b
                quitf(_fail, "should not have operation 5");
            }
            case 6: {  // status k
                inf.readInt(1, n, "k");
                has_op6 = true;
                break;
            }
            case 7: {  // revert
                quitf(_fail, "should not have operation 7");
            }
        }
        inf.readEoln();
    }

    ensuref(has_op6, "There must be at least one status operation (type 6)");

    inf.readEof();
    return 0;
}
