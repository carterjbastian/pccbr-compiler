int argsFunc() {
    return 0;
}

int args2Func(int cats[]) {
    return cats[1];
}

int main() {
    int x = 5;
    argsFunc(x);
    argsFunc(x, x);

    return 1;
}
