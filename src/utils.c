int is_int(char *str){
    for (int i = str[0] == '-' ? 1 : 0; str[i] != '\0'; i++) if (str[i] < '0' || str[i] > '9') return 0;
    return 1;
}

int str2int(char *str){
    int out = 0;
    int neg = str[0] == '-';
    int i = 0;
    if (neg) i++;
    while (str[i] != '\0'){
        out*=10;
        out+=(int)(str[i] - '0');
        i++;
    }
    if (neg) out = -out;
    return out;
}

int is(char *a, char *b){
    int i = 0;
    while (a[i] != '\0' && b[i] != '\0'){
        if (a[i] != b[i]) return 0;
        i++;
    }
    if (a[i] != b[i]) return 0;
    return 1;
}

//Returns if a has b in it
int includes(char *a, char *b){
    int blen = 0;
    while (b[blen] != '\0') blen++;
    if (blen == 0) return 1;
    int i = 0;
    int j = 0;
    while (a[j] != '\0'){
        if (a[j] == b[i]) {
            i++;
            if (i == blen) return 1;
        }else {
            i = 0;
        }
    }
    return 0;
}
