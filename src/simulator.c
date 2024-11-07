#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

typedef struct Queue {
    long long* items; //Tags
    int maxSize; //W
    int front;
    int end;
} Queue;

typedef struct CacheLine {
    long long tag;
    bool valid;
} CacheLine;

typedef struct Set {
    Queue* q; //LRU Queue
    CacheLine** lines;
    int n_lines; //W
} Set;

typedef struct Cache {
    int B;
    int S;
    int W;
    Set** sets;
    int misses;
} Cache;

Queue* createQ(int maxSize){
    Queue* q = (Queue*)malloc(sizeof(Queue));

    q->front = -1;
    q->end = -1;
    q->maxSize = maxSize;
    q->items = (long long *)malloc(maxSize * sizeof(long long));

    return q;
}

long long dequeue(Queue* q) {
    int item;
    item = q->items[q->front];

    for (int i = q->front; i < q->end; i++) {
        q->items[i] = q->items[i + 1];
    }
    q->end--;

    return item;
}

void enqueue(Queue* q, long long value){
    
    //If queue is empty
    if(q->front == -1){
        q->front++;
        q->end++;

        q->items[q->front] = value;
        return;
    }

    //If already in queue, remove it
    for(int i = q->front; i <= q->end; i++){
        if (q->items[i] == value) {
            if(i == q->end){
                q->end--;
                break;
            }
            for (int j = i; j < q->end; j++) {
                q->items[j] = q->items[j + 1];
            }
            q->end--;
        }
    }

    //If queue is full
    if(q->end+1 > q->maxSize){
        dequeue(q);
    }
    
    q->end++;
    q->items[q->end] = value;
}

Cache* cache;

// initialize simulation
void sim_start(int B, int S, int W) {
    cache = (Cache*)malloc(sizeof(Cache));

    cache->B = B; //Number of bytes of content in each cachelines; offset
    cache->S = S; //Number of sets
    cache->W = W; //Number of cache lines; index
    cache->misses = 0;

   cache->sets = (Set**)malloc((1 << S) * sizeof(Set*));

    for(int i = 0; i < (1 << S); i++){
        cache->sets[i] = (Set*)malloc(sizeof(Set));
        cache->sets[i]->q = createQ(1 << W); //Init queue for each set
        cache->sets[i]->n_lines = (1 << W);

        cache->sets[i]->lines = (CacheLine**)malloc((1 << W) * sizeof(CacheLine*)); //Init each cache line

        for(int j = 0; j < (1 << W); j++){
            cache->sets[i]->lines[j] = (CacheLine*)malloc(sizeof(CacheLine));
            cache->sets[i]->lines[j]->tag = -1;
            cache->sets[i]->lines[j]->valid = false;
        }
    }
}

// access one memory black
void sim_access(long long acc) {
    int set = (acc >> cache->B) & ((1 << cache->S) - 1);
    long long tag = acc >> (cache->B + cache->S);

    Set* s = cache->sets[set];

    //Memory in cache
    for(int i = 0; i < s->n_lines; i++){
        if(s->lines[i]->valid && s->lines[i]->tag == tag){
            enqueue(s->q, tag);
            return;
        }
    }

    //Memory not in cache; but set has space
    for(int i = 0; i < s->n_lines; i++){
        if(!s->lines[i]->valid){
            s->lines[i]->tag = tag;
            s->lines[i]->valid = true;
            cache->misses++;
            enqueue(s->q, tag);
            return;
        }
    }

    //No space in set; replace with LRU
    long long lru = dequeue(s->q);

    for(int i = 0; i < s->n_lines; i++){ 
        if(s->lines[i]->tag == lru){
            s->lines[i]->tag = tag;
            enqueue(s->q, tag);
            s->lines[i]->valid = true;
            break;
        }
    }
    cache->misses++;

    return;
}

// return the number of misses
int sim_finish(void) {
    int m = cache->misses;

    for (int i = 0; i < (1 << cache->S); i++) {
        if (cache->sets[i] != NULL) {
            for (int j = 0; j < (1 << cache->W); j++) {
                if (cache->sets[i]->lines[j] != NULL) {
                    free(cache->sets[i]->lines[j]);
                }
            }
            free(cache->sets[i]->lines);
            free(cache->sets[i]->q->items);
            free(cache->sets[i]->q);
            free(cache->sets[i]);
        }
    }
    free(cache->sets);

    return m;
}