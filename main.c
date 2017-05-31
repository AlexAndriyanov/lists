#define _CRT_SECURE_NO_WARNINGS
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define MAXVAL 256
#define SHIFT_TEST 3
#define SHIFT_FILE 0

typedef struct list {
    int data;
    int symbol;
    struct list *next;
    struct list *left;
    struct list *right;
} list;


/*
* делим длинну файла на 256
* b-остаток от деления
* return-целый часть
*/
long separate(int a,int *b) {
    *b = a % MAXVAL;
    return (a / MAXVAL);
}

/*
* создаём узел и кладём его в стек
* tmp->data-частота символа
* tmp->symbol-символ
*/
void push(list **head, int data, int symbol) {
    list *tmp = (list *)malloc(sizeof(list));
    tmp->data = data;
    tmp->symbol = symbol;
    tmp->right = NULL;
    tmp->left = NULL;
    tmp->next = (*head);
    (*head) = tmp;

}

/*
* cоздаём стек из встречающихся символов
*/
void create_list(list **head, int *array) {
    int i;
    for (i = 0; i < MAXVAL; i++) {
        if (array[i] != 0)
            push(head, array[i], i);
    }
}

/*
* создаём таблицу частот символов
*/
void create_table(unsigned char *array, int *table, int size) {
    for (int i = 0; i < size; i++) {
        table[(int)array[i]]++;
    }
}

/*
* функиция возвращающая необходимый узел
*/
list *getNth(list *head, int n) {
    int counter = 0;
    while (counter < n && head) {
        head = head->next;
        counter++;
    }
    return head;
}

/*
* удаляет элемент из стека
*/
void deleteNth(list **head, int n) {
    if (n != 0) {
        list *prev = getNth(*head, n - 1);
        list *elm = prev->next;
        prev->next = elm->next;
    }
    else {
        (*head) = (*head)->next;
    }
}

/*
* ищем сивол с минимальной частотой
* position-хранит позицию минимального элемента
* count-хранит частоту символа
* return-символ с минимальной частотой
*/
list *find_min(list *head, int *count, int *position) {
    list *tmp = malloc(sizeof(list));
    if (tmp == NULL) {
        exit(0);
    }
    int i = 0;
    tmp = head;
    int min = head->data;
    *position = 0;
    head = head->next;
    while (head != NULL) {
        i++;
        if (head->data < min) {
            min = head->data;
            tmp = head;
            (*position) = i;
        }
        head = head->next;
    }
    *count = min;
    return tmp;
}

unsigned  char code[MAXVAL];
int count_stack = 0;
unsigned char alph[MAXVAL][MAXVAL];


void add_code(int symbol){
    for(int i=0;i<count_stack;i++){
        alph[symbol][i]=code[i];
    }
}


/*
* функция,создающая коды для сиволов
* путём рекурсивного обхода дерева.
* Каждый спуск влево добавляет коду "0"
*  Каждый спуск вправ добавляет коду "1"
*/
void preOrderTravers(list *root) {
    if (root->left != NULL) {
        code[count_stack++]='0';
        preOrderTravers(root->left);
    }
    if (root->right != NULL) {
        code[count_stack++]= '1';
        preOrderTravers(root->right);
    }
    if (root->symbol >= 0) {
        add_code(root->symbol);
    }
    count_stack = (count_stack == 0) ? 1 : count_stack;
    strcpy((char *) &code[--count_stack], "\0");
}

/*
* находим два минимальных элемента в стеке,
* убираем их из стека,
* создаём из них новый узел,
* кладём его обратно в стек
*/
void create_tree(list **head) {
    int q, e;
    int position;
    while ((*head)->next != NULL) {
        list *tmp = malloc(sizeof(list));
        q = 0;
        e = 0;

        list *first = malloc(sizeof(list));
        if (first == NULL) {
            exit(0);
        }
        first = find_min(*head, &q, &position);
        deleteNth(head, position);
        first->next = NULL;

        list *second = malloc(sizeof(list));
        if (second == NULL) {
            exit(0);
        }
        second = find_min(*head, &e, &position);
        deleteNth(head, position);
        second->next = NULL;

        tmp->data = q + e;
        tmp->left = first;
        tmp->symbol = -1;
        tmp->right = second;
        tmp->next = (*head);
        (*head) = tmp;
    }
}


/*
* функция чтения входного файла
* check-сдвиг во входном файле(для "d\n" и "c\n")
* */
int read_input(FILE *INPUT, int *array, FILE *OUTPUT, int check) {
    unsigned char buffer[MAXVAL];
    int b;
    int i = 0;
    fseek(INPUT, 0, SEEK_END);
    int size = (int)(ftell(INPUT) - check);
    if (size == 0)
        return 1;
    fseek(INPUT, check, SEEK_SET);
    int count = separate(size, &b);
    if (count == 0) {
        fread(buffer, sizeof(unsigned char), (size_t)b, INPUT);
        create_table(buffer, array, b);
    }
    else {
        for (i = 0; i < count; i++) {
            fread(buffer, sizeof(unsigned char), MAXVAL, INPUT);
            create_table(buffer, array, MAXVAL);
        }
        fread(buffer, sizeof(unsigned char), (size_t)b, INPUT);
        create_table(buffer, array, b);
    }
    fseek(INPUT, check, SEEK_SET);
    fwrite(&size, sizeof(int), 1, OUTPUT);
    return 0;
}

/*
* функция перевода двоичной строки в десятичное число
*/
int to_dec(unsigned char *array, int size) {
    int a = 0,i;
    for (i = 0; i < 8; i++) {
        a = a + (array[i] - '0') *(int)pow(2, 8 - i - 1);
    }
    for(i=0;i<size-8;i++){
        array[i]=array[i+8];
    }
    return a;
}

/*
* функия соеденяющая две строки a и b в строку a
*/
void together(unsigned char *a, unsigned char *b, size_t size_a, int size_b) {
    int j = 0;
    for (size_t i = size_a; i < (size_a + size_b); i++,j++) {
        a[i] = b[j];
    }
}

/*
* функция считавает один байт из файла
* получает код этого символа
* складывает его с предыдущем кодом
* если их длинна больше или равна 8 записываем сивол в файл
*/
int encryption(FILE *INPUT, FILE *OUTPUT, unsigned char *CodeOfSymbol, size_t *size_source) {

    unsigned char tmp[MAXVAL];

    unsigned char buffer[1];
    int i=0;
    fread(buffer, sizeof(unsigned char), 1, INPUT);
    while (alph[(int)buffer[0]][i]!='\0'){
        tmp[i]=alph[(int)buffer[0]][i];
        i++;
    }
    int size_tmp=i;
    together(CodeOfSymbol, tmp, *size_source,size_tmp);
    *size_source = (size_t)(*size_source + size_tmp);
    while (*size_source >= 8) {
        buffer[0] = (unsigned char)to_dec(CodeOfSymbol, (int) *size_source);
        *size_source = *size_source - 8;
        fwrite(buffer, sizeof(unsigned char), 1, OUTPUT);
    }
    return 0;
}

/*
* считываем исходный файл и записывает шифрованый текст в выходной файл
*/
int create_out(FILE *INPUT, FILE *OUTPUT, int check) {
    int i = 0;
    unsigned char CodeOfSymbol[MAXVAL]={8};
    size_t size_source = 0;
    fseek(INPUT, 0, SEEK_END);
    int size = ftell(INPUT) - check;
    fseek(INPUT, check, SEEK_SET);
    for (int j = 0; j < size; j++) {
        encryption(INPUT, OUTPUT,CodeOfSymbol, &size_source);
    }
    if (size_source > 0) {
        for (i = size_source; i < 8; i++) {
            CodeOfSymbol[i] = '0';
        }
        unsigned char buffer[1];
        buffer[0] = (unsigned char)to_dec(CodeOfSymbol,8);
        fwrite(buffer, sizeof(unsigned char), 1, OUTPUT);
    }
    return 0;
}


char description[MAXVAL];
int count_des = 0;

/*
* записываем в массив description сивол
*/
void push_symbol(int a) {
    char array[8];
    int i = 0;
    int j = 0;
    while (a > 0) {
        array[i++] = (char)(a % 2 + '0');
        a = a / 2;
    }
    for (j = 0; j < 8 - i; j++) {
        description[count_des++] = '0';
    }
    i--;
    for (j = 0; j < i + 1; j++) {
        description[count_des++] = array[i - j];
    }
}

/*
* если в массиве description больше 8 элементов, записываем
* символ в выходной файл и сдвигаем элементы в массиве description влево на 8 элементов
*/
void shift(FILE *OUTPUT) {
    int i;
    unsigned char buffer[MAXVAL];
    unsigned char symbol[1];
    while (count_des >= 8) {
        strncpy((char *)buffer, description, 8);
        for (i = 0; i < count_des - 8; i++) {
            description[i] = description[i + 8];
        }
        count_des = count_des - 8;
        symbol[0] = (unsigned char)to_dec(buffer,8);
        fwrite(symbol, sizeof(unsigned char), 1, OUTPUT);
    }
}

/*
* функция записывающая дерево в выходной файл
* каждый вспуск вниз по дереву добавляет "1" в массив description
* каждый раз встречая сивол добавляем "0" в массив description и сам символ
*/
int create_descript(FILE *OUTPUT, list *root) {
    if (root->left == NULL && root->right == NULL){
        description[count_des++]='0';
        push_symbol(root->symbol);
        if (count_des >= 8) {
            shift(OUTPUT);
        }
        return 0;
    }
    if (root->left != NULL && (root->left)->symbol<0) {
        description[count_des++]='1';
        create_descript(OUTPUT, root->left);
    }
    else{
        description[count_des++]='0';
        push_symbol((root->left)->symbol);
        if (count_des >= 8) {
            shift(OUTPUT);
        }
    }
    if (root->right != NULL && (root->right)->symbol<0) {
        description[count_des++]='1';
        create_descript(OUTPUT, root->right);
    }
    else{
        description[count_des++]='0';
        push_symbol((root->right)->symbol);
        if (count_des >= 8) {
            shift(OUTPUT);
        }
    }
    return 0;
}

/*
* если после записи дерева в массиве остались элементы,
* то дописываем в конце 0 до 8 сиволов и записываем в файл
*/
void tail(FILE *OUTPUT) {
    unsigned char buffer[8];
    unsigned char symbol[1];
    if (count_des != 0) {
        for (int i = count_des; i < 8; i++) {
            description[i] = '0';
        }
        strncpy((char *)buffer, description, 8);
        symbol[0] = (unsigned char)to_dec(buffer,8);
        fwrite(symbol, sizeof(unsigned char), 1, OUTPUT);
    }
}


//////////////////////////////////////////////////////////////////////////////////
unsigned char buffer = 0;
size_t count = 0;

/*
* функция считывающая один бит
* с последующим битовым сдвигом,
* с помощью конъюнкции 0x80 получаем значение первого бита
*/
int readBit(FILE *in) {
    int res;
    size_t readed;
    if (count == 0) {
        readed = fread(&buffer, sizeof(unsigned char), 1, in);
        count = readed * 8;
    }
    res = buffer & 0x80; // 0x80 == 1_0000000
    buffer = buffer << 1;
    count--;
    return res;
}

/*
* функция считывающая один байт из входного файла
*/
int readByte(FILE *in) {
    int i, res = 0;
    for (i = 0; i < 8; i++) {
        res = res << 1;
        if (readBit(in)) {
            res |= 1;
        }
    }
    return res;
}


/*
 * функция добавляющая в дерево символ
 */
list* read_symbol(FILE *file){
    list *node= malloc(sizeof(list));
    node->symbol = readByte(file);
    node->data = 0;
    node->left = NULL;
    node->right = NULL;
    return node;
}

/*
* функция рекурсивно создающая дерево частот хаффмана
* получив на вход 1 функция переход вниз по дереву
* получив на вход 0 функция считывает символ и записывает его в узел
*/
list *readHuffmansTree(FILE *file) {
    list *node = malloc(sizeof(list));
    node->left=NULL;
    node->right=NULL;
    node->symbol=-1;

    if (readBit(file) == 0) {
        if (node->left == NULL){
            node->left=read_symbol(file);
        }
    }
    else {
        node->left = readHuffmansTree(file);
        if(readBit(file)==0){
            node->right=read_symbol(file);
        }else{
            node->right = readHuffmansTree(file);
        }
        return node;
    }
    if(readBit(file)==0){
        node->right=read_symbol(file);
    }
    else {
        node->right = readHuffmansTree(file);
    }
    return node;
}

/*
* функция считывающая размер файла до сжатия,
* считывает и создаёт дерево
*/
void readHeader(FILE *file, list **ppTree, int *length) {
    fread(length, sizeof(int), 1, file);
    *ppTree = readHuffmansTree(file);
    preOrderTravers(*ppTree);
}

/*
* функция раскодирующая один считаный байт
*/
int decodeByte(FILE *in, list *tree) {
    if (tree->symbol != -1) {
        return tree->symbol;
    }
    else if (readBit(in)) {
        return decodeByte(in, tree->right);
    }
    else {
        return decodeByte(in, tree->left);
    }
}

void delete_tree(list* head){
    if (head->left!=NULL){
        delete_tree(head->left);
    }
    if (head->right!=NULL){
        delete_tree(head->right);
    }
    free(head);
}

/*
* функция декодирования
*/
void decode(FILE *infile, FILE *outfile) {
    int  ch;
    int i, length = 0;
    list *pTree;

    readHeader(infile, &pTree, &length);
    count = 0;
    for (i = 0; i<length; i++) {
        ch = decodeByte(infile, pTree);
        fputc(ch, outfile);
    }
    delete_tree(pTree);
}

/*
* фунция кодирующая исходный файл и записывает в выходной файл
*/
int coding(FILE *IN, FILE *OUT, int check){
    list *head;
    head = NULL;
    int alphavite[MAXVAL] = { 0 };
    if (read_input(IN, alphavite, OUT, check)) {        //подсчитываем количество вхождений символов в файле
        return 0;
    }
    create_list(&head, alphavite);            //создаём список встречающихся символов
    create_tree(&head);                        //создаём бинарное дерево
    preOrderTravers(head);        //обходим дерево

    create_descript(OUT, head);        //записываем дерево
    tail(OUT);                        //
    create_out(IN, OUT,check);        //создание шифрованного файла
    delete_tree(head);
    fclose(OUT);
    fclose(IN);
    return 0;
}

/*
* функция разархивирования файла
*/
void decoding(FILE *IN, FILE *OUT){
    decode(IN, OUT);
    fclose(OUT);
    fclose(IN);
}

/*
* функция для работы с тестами
*/
int for_test(FILE *IN, FILE *OUT){
    char buffer[3];
    fread(buffer, sizeof(unsigned char), 3, IN);
    if (buffer[0] == 'c') {
        coding(IN, OUT, SHIFT_TEST);
    }
    else{
        if (buffer[0] == 'd')
            decoding(IN, OUT);
    }
    return 0;
}


int main(int argc, char **argv) {
    FILE *INPUT;
    FILE *OUTPUT;
    if (argc>1){
        INPUT = fopen(argv[2], "rb");
        if (!INPUT)
            return 0;

        OUTPUT = fopen(argv[3], "wb");
        if (!OUTPUT)
            return 0;

        if (!strcmp(argv[1], "-c")){
            coding(INPUT, OUTPUT, SHIFT_FILE);
            return 0;
        }
        else{
            if (!strcmp(argv[1], "-d")){
                decoding(INPUT, OUTPUT);
                return 0;
            }
        }
        printf(">neverniy kluch\n");
        return 0;
    }
    INPUT = fopen("in.txt", "rb");
    if (!INPUT)
        return 0;

    OUTPUT = fopen("out.txt", "wb");
    if (!OUTPUT) {
        return 0;
    }
    for_test(INPUT, OUTPUT);
    return 0;
}
