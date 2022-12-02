#include<stdio.h>
#include<stdlib.h>
#include<string.h>
int table_size;

typedef struct{
    int acc_num;
    int read_num;
    int write_num;
    int hit_num;
    int fault_num;
    double fault_rate;
}Data;
Data data;

typedef struct{
    int PFN;
    char op[6];
    int counter;
}Entry;

typedef struct{
    Entry* stack;
    int top;
}Stack;

void init(Stack *s);
int is_full(Stack *s);
int is_empty(Stack*s);
void push(Stack* s,int PFN,char * operation);
int search(Stack*s,int page_num,char*operation);
void print_data();
int search_for_evict(Stack*s);

void init(Stack *s)
{
    s->stack = (Entry*)malloc(sizeof(Entry)*table_size);
    s->top = -1;
}
int is_full(Stack *s)
{
    return(s->top == (table_size-1));
}

int is_empty(Stack*s)
{
    return(s->top == -1);
}
void push(Stack* s,int PFN,char * operation) // when it faults
{
    data.fault_num++;
    if(!is_empty(s)){  // counter++
        for(int i = 0;i<=s->top;i++){
            s->stack[i].counter++;
        }
    }

    if(is_full(s)){
        
        int index = search_for_evict(s);
        strcpy(s->stack[index].op,operation);
        s->stack[index].PFN = PFN;
        s->stack[index].counter = 0;
        return;
    }
    strcpy(s->stack[s->top++].op,operation);
    s->stack[s->top].PFN = PFN;
    s->stack[s->top].counter = 0;
}

int search_for_evict(Stack*s) // return an index to evict.
{
    int max = 0;
    for(int i = 1;i<=s->top;i++){
        if(s->stack[i].counter>s->stack[max].counter){
            max = i;
        }
    }
    return max;
}

int search(Stack*s,int page_num,char *op)
{
    if(!strcmp("read",op)) // increase read count
        data.read_num++;
        
    else if(!strcmp("write",op)) //increase write count
        data.write_num++;
    

    for(int i = 0;i<=s->top;i++){ // increase counter
        s->stack[i].counter++;
    }

    for(int i = 0;i<=s->top;i++){ // searching
        if(s->stack[i].PFN == page_num){
            data.hit_num++;
            s->stack[i].counter = 0;
            return 1;
        }
    }
    push(s,page_num,op);
    return 0;
}

void print_data()
{
    printf("Total number of access: %d\n",data.acc_num);
    printf("Total number of read: %d\n",data.read_num);
    printf("Total number of write: %d\n",data.write_num);
    printf("Number of page hits: %d\n",data.hit_num);
    printf("Number of page faults: %d\n",data.fault_num);
    printf("Page fault rate: %d/%d = %0.3f %%\n",data.fault_num,data.acc_num,data.fault_rate);

}

int main()
{
    Stack s;
    FILE *fp = fopen("access.list","r");

    int top = 0;
    scanf("%d",&table_size);
    init(&s);
    int page_num;
    char op[6];
    while(EOF != fscanf(fp, "%d %s", &page_num, op)){
        data.acc_num++;
        search(&s,page_num,op);
    }
    data.fault_rate = ((double)data.fault_num/(double)data.acc_num)*100;
    print_data();
    free(s.stack);
    fclose(fp);
    return 0;
}