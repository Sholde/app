#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

double reduction_somme(double in)
{
  /* A COMPLETER */
  int rank = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  int size = 0;
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  // Define root
  int root = 0;

  // Stock result
  double res = in;
  
  if (rank != root)
    {
      // Send to 0 res_loc
      MPI_Send(&res, 1, MPI_DOUBLE, root, 0, MPI_COMM_WORLD);

      // Recv from 0 res_glob
      MPI_Recv(&res, 1, MPI_DOUBLE, root, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
  else /* rank 0 */
    {
      // Recv all res_loc
      for (int i = 1; i < size; i++)
        {
          double tmp = 0;
          MPI_Recv(&tmp, 1, MPI_DOUBLE, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); /* sta.MPI_SOURCE */
          res += tmp;
        }
      
      // Send res_glob to all
      for (int i = 1; i < size; i++)
        {
          MPI_Send(&res, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
        }
    }

  return res;
}

struct info_t {
    int nproc;
    int rang;

    int ntot;
    int nloc;

    int ideb;
    int ifin;
};

void init_info(int n, struct info_t *info) {

    int Q, R;

    MPI_Comm_rank(MPI_COMM_WORLD, &(info->rang));
    MPI_Comm_size(MPI_COMM_WORLD, &(info->nproc));

    Q = n / info->nproc;
    R = n % info->nproc;

    info->ntot = n;

    if (info->rang < R) {

        info->nloc = Q+1;
        info->ideb = info->rang * (Q+1);
        info->ifin = info->ideb + info->nloc;

    } else {

        info->nloc = Q;
        info->ideb = R * (Q+1) + (info->rang - R) * Q;
        info->ifin = info->ideb + info->nloc;
    }

    printf("%d %d %d %d %d %d\n", info->nproc, info->rang, info->ntot, info->nloc, info->ideb, info->ifin);
}

void lire_fichier(const char *nom, double *x, struct info_t *info)  {

    FILE *fd;
    int i;
    double val;

    fd = fopen(nom, "r");

    for( i = 0 ; i < info->ideb ; i++ )
        fscanf(fd, "%lf\n", &val);
    for( i = info->ideb ; i < info->ifin ; i++ )
        fscanf(fd, "%lf\n", &(x[i - info->ideb]));

    fclose(fd);
}

double produit_scalaire(int N, double *a, double *b) {

    double res_loc, res_glob;
    int i;

    res_loc = 0;

    for( i = 0 ; i < N ; i++ )
        res_loc += a[i]*b[i];

    res_glob = reduction_somme(res_loc);

    return res_glob;
}

int main(int argc, char **argv) {

    int Nglob, Nloc;
    double *x, *y;
    double res, err;
    struct info_t info;

    MPI_Init(&argc, &argv);

    Nglob = 100;

    init_info(Nglob, &info);
    Nloc = info.nloc;

    x = (double*)malloc(Nloc*sizeof(double));
    y = (double*)malloc(Nloc*sizeof(double));

    lire_fichier("vec_x", x, &info);
    lire_fichier("vec_y", y, &info);

    res = produit_scalaire(Nloc, x, y);

    printf("Produit scalaire = %.6e\n", res);

    err = fabs(res - Nglob)/(double)Nglob;
    if (err < 1.e-6)
    {
        printf("PASSED\n");
    }
    else
    {
        printf("Solution exacte : %g\n", (double)Nglob);
        printf("FAILED\n");
    }

    free(x);
    free(y);

    MPI_Finalize();

    return 0;
}

