#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_VAL 1000

typedef struct info_s
{
  int *tab;
  int deb;
  int fin;
  int max_local;
} info_t;

int max_seq(int *tab, int nelt)
{
    int i, maxv;

    maxv = 0;
    for( i = 0 ; i < nelt ; i++ )
    {
        if (tab[i] > maxv)
        {
            maxv = tab[i];
        }
    }

    return maxv;
}

void *max_parallel(void *arg)
{
  info_t *info = (info_t *)arg;

  for (int i = info->deb; i < info->fin; i++)
    {
      if (info->tab[i] > info->max_local)
        {
          info->max_local = info->tab[i];
        }
    }

  return NULL;
}

int main(int argc, char **argv)
{
    int nthreads, nelt, i, t, maxv_seq, maxv_mt;
    int *tab;

    nelt     = atoi(argv[1]); /* Taille du tableau */
    nthreads = atoi(argv[2]); /* Nombre de threads a creer */

    /* Creation du tableau et remplissage aleatoire */
    tab = (int*)malloc(nelt*sizeof(int));

    srand(nelt);
    for( i = 0 ; i < nelt ; i++)
    {
        tab[i] = 1 + (rand() % MAX_VAL);
    }

    /* Recherche du max de tab en contexte multithread => maxv_mt*/
    maxv_mt = 0;
    /* A ECRIRE */
    pthread_t *tid = malloc(sizeof(pthread_t) * nthreads);
    info_t *info = malloc(sizeof(info_t) * nthreads);

    int truc = nelt / nthreads;
    
    for (int i = 0; i < nthreads; i++)
      {
        info[i].tab = tab;
        info[i].deb = i * truc;
        info[i].fin = (i + 1) * truc;

        if (info[i].fin >= nelt)
          {
            info[i].fin = nelt;
          }

        info[i].max_local = 0;
        
        pthread_create(&tid[i], NULL, max_parallel, &info[i]);
      }

    for (int i = 0; i < nthreads; i++)
      {
        pthread_join(tid[i], NULL);

        if (info[i].max_local > maxv_mt)
          maxv_mt = info[i].max_local;
      }

    /* Recherche du max en sequentiel pour verification => maxv_seq */
    maxv_seq = max_seq(tab, nelt);

    if (maxv_seq == maxv_mt)
    {
        printf("PASSED\n");
    }
    else
    {
        printf("FAILED\n");
        printf("Valeur correcte : %d\n", maxv_seq);
        printf("Votre valeur    : %d\n", maxv_mt);
    }

    free(tab);

    return 0;
}

