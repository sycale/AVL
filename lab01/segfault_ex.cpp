int main() {
    int a[20];
    for (int i = 0; i < sizeof(a) / sizeof(uint) ; i++) {
        a[i] = i;
    }
}