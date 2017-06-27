#include <QCoreApplication>
#include <QTime>
#include <QDebug>
#include <iostream>
#include <cstdio>

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
}
;

set_t set[4];
set_t *A = &set[0];
set_t *B = &set[1];
set_t *C = &set[2];
set_t *S = &set[3];

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

void set_intersection3(set_t *result, char *name, set_t *set1, set_t *set2, set_t *set3)
{
    int i;
    int e;

    strncpy(result->name, name, SET_NAME_LEN);
    result->size = 0;

    for ( i = 0; i < set1->size; i++ )
    {
        e = set1->element[i];
        if ( set_is_belonged(set2, e) && set_is_belonged(set3, e) )
        {
            result->element[result->size] = e;
            result->size++;
        }
    }
}

int main(int argc, char *argv[])
{

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
