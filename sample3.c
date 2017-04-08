
int returner(void) {
    int a; int b;
    a = 1;
    return a;
}

void main(void) {
    int key;
    output(returner());
    key = 0;
    output(returner());
    key = returner();
    output(key);
}
