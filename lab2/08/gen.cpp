#include <bits/stdc++.h>
using namespace std;

int main() {
    ios_base::sync_with_stdio(false);
    cout.tie(nullptr);
    freopen("input.txt", "w", stdout);
    for (int i = 0; i < 2000; i++) {
        cout << char('0' + (i % 10)); 
    }
    cout << endl;
}
