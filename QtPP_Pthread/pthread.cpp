#include <QCoreApplication>
#include <QTime>
#include <QDebug>
#include <iostream>

using namespace std;

const int NUM_REPEAT = 50;
const int SET_SEED = 1;

const int MIN_ELEMENT = 1;
const int MAX_ELEMENT = 500000;

const int SET_NAME_LEN = 16;

struct set_t
{
    char name[SET_NAME_LEN];
    int size;
    int element[MAX_ELEMENT];
};

set_t set[4];
set_t *A = &set[0];
set_t *B = &set[1];
set_t *C = &set[2];
set_t *S = &set[3];

const int MIN_THREADS = 1;
const int MAX_THREADS = 8;

struct thread_data_t
{
    pthread_mutex_t iterator;
    pthread_mutex_t store;
    int i;
    set_t *set1;
    set_t *set2;
    set_t *set3;
    set_t *result;
};

int num_threads = MIN_THREADS;

void set_create(set_t *set, char *name, int nummod)
{
    int e;

    strncpy(set->name, name, SET_NAME_LEN);
    set->size = 0;

    for ( e = MIN_ELEMENT; e < MAX_ELEMENT; e++ )
    {
        if (e % nummod == 0)
        {
            set->element[set->size] = e;
            set->size++;
        }
    }
}

int set_save(set_t *set)
{
    FILE *pf;
    int i;
    char filename[256];

    snprintf(filename, 256, "simple_%s.txt", set->name);

    if ( (pf = fopen(filename, "w")) == NULL )
    {
        perror("fopen()");
        return -1;
    }

    fprintf(pf, "size = %d\n", set->size);
    for ( i = 0; i < set->size; i++ )
    {
        fprintf(pf, "%d\n", set->element[i]);
    }

    fclose(pf);

    return 0;
}

int set_is_belonged(set_t *set, int e)
{
    int i;

    for ( i = 0; i < set->size; i++ )
    {
        if ( set->element[i] == e )
            return 1;
    }

    return 0;
}

static int next_i(thread_data_t *data)
{
    int i;

    pthread_mutex_lock(&(data->iterator));
    i = data->i;
    data->i++;
    pthread_mutex_unlock(&(data->iterator));

    return i;
}

static void *thread_intersect3(void *ptr)
{
    int i, k;
    int e;
    thread_data_t *data = (thread_data_t *)ptr;

    for ( i = next_i(data); i < data->set1->size; i = next_i(data) )
    {
        e = data->set1->element[i];
        if ( set_is_belonged(data->set2, e) && set_is_belonged(data->set3, e) )
        {
            pthread_mutex_lock(&(data->store));
            data->result->element[data->result->size] = e;
            data->result->size++;
            pthread_mutex_unlock(&(data->store));
        }
    }

    return NULL;
}

void set_intersection3(set_t *result, char *name, set_t *set1, set_t *set2, set_t *set3)
{
    int i;
    pthread_t thread[MAX_THREADS];
    thread_data_t data;

    strncpy(result->name, name, SET_NAME_LEN);
    result->size = 0;

    pthread_mutex_init(&(data.iterator), NULL);
    pthread_mutex_init(&(data.store), NULL);
    data.i = 0;
    data.set1 = set1;
    data.set2 = set2;
    data.set3 = set3;
    data.result = result;

    for ( i = 1; i < num_threads; i++ )
    {
        if ( pthread_create(&thread[i], NULL, thread_intersect3, (void *)&data) < 0 )
        {
            perror("pthread_create()");
            exit(EXIT_FAILURE);
        }
    }

    thread_intersect3((void *)&data);

    for ( i = 1; i < num_threads; i++ )
    {
        pthread_join(thread[i], NULL);
    }
}

int main(int argc, char *argv[])
{
    if ( argc > 1 )
    {
        num_threads = atoi(argv[1]);
        if ( (num_threads < MIN_THREADS) || (num_threads > MAX_THREADS) )
        {
            num_threads = MIN_THREADS;
        }
    }

    set_create(A, "a", 10);
    set_create(B, "b", 25);
    set_create(C, "c", 4);

    set_save(A);
    set_save(B);
    set_save(C);

    int timeList[NUM_REPEAT];
    int timeListSum = 0;
    int mo = 0;
    int disp = 0;

    for (int i = 0; i < NUM_REPEAT; i++) {
        QTime time;
        time.start () ;
        set_intersection3(S, "s", A, B, C);
        timeList[i] = time.elapsed();
        timeListSum += time.elapsed();
        qDebug() << "Время работы функции set_intersection3(...) равно "
                 << time.elapsed()
                 << "миллисекунд"
                 << endl;
    }
    mo = timeListSum / NUM_REPEAT;
    for(int i = 0; i < NUM_REPEAT; i++) {
        disp = disp + pow((timeList[i] - mo),2);
    }
    disp = disp / (NUM_REPEAT - 1);
    int sigma = sqrt(disp);
    //float t = 2.2281;
    float t = 2.0086;
    int interHigh = mo + t * (sigma/(sqrt(NUM_REPEAT)));
    int interLow = mo - t * (sigma/(sqrt(NUM_REPEAT)));
    qDebug() << "Математическое ожидание работы функции set_intersection3(...) равно "
             << mo
             << endl;
    qDebug() << "Дисперсия работы функции функции set_intersection3(...) равно "
             << disp
             << endl;
    qDebug() << "Доверительный интервал set_intersection3(...) равно "
             << interLow
             << " - "
             << interHigh
             << endl;
    set_save(S);

    return EXIT_SUCCESS;
}
