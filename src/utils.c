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
