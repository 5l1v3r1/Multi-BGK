// This builds a 1D mesh, across multiple MPI ranks if needed. 

#include "mesh.h"
#include <mpi.h>

int get_Rank_Nx(int,int,int);
double get_x_left(int,double,double,int,int);

/*
This sets up the spatial discretization by splitting up the physical mesh
across MPI ranks. This also allocates the x and dx arrays for each rank.

Inputs
Nx    - total number of physical zones in the problem
Lx    - width of physical domain [cm]
order - order of discretization. There are <order> ghost cells on either side
        of the physical grid points in each rank's x array

Outputs
Nx_rank             - number of physical grid zones on this rank
x[Nx_rank+2*order]  - the location of the zone centers on this rank, including ghost cells
dx[Nx_rank+2*order] - the width of the zones. For now this code sets every zone to be the same width
                      dx = Lx/ Nx, but this is left as an array to make future changes easier

*/
void make_mesh(int Nx, double Lx, int order, int *Nx_rank, double **x, double **dx) {

  int rank;
  int numRanks;
  int numZones;
  int zone;
  double dx_zone;
  double x_start;
  double x_count;

  MPI_COMM_size(MPI_COMM_WORLD,&numRanks);
  MPI_COMM_rank(MPI_COMM_WORLD,&rank);

  dx_zone = Lx / Nx;

  if(numRanks > 1) {
    *Nx_rank = getRankNx(Nx, rank, numRanks);
  }
  else {
    *Nx_rank = Nx;
  }

  numZones = *Nx_rank + 2*order;
  x_start = get_x_left(Nx, Lx, dx_zone, rank, numRanks);
  x_count = x_start - 0.5*dx_zone;

  *x = malloc(numZones * sizeof(double));
  *dx = malloc(numZones * sizeof(double));
  
  for(zone=order; zone < numZones-order; zone++) {
    (*dx)[zone] = dx_zone;
    (*x)[zone] = x_count + dx_zone; 
  }

  if(order == 1) {
    (*dx)[0] = dx_zone;
    (*x)[0] = 
    

}
  

/*
This calculates the number of physical zones on each rank, based on the total number of zones and ranks available
*/
double get_Rank_Nx(int Nx, int rank, int numRanks)
{
  int remainder; 
  int Nx_rank_base;

  //Find the leftover stuff that can't divide things equally
  Nx_rank_base = Nx / numRanks;

  remainder = Nx % numRanks;
  
  //If we are below remainder, add an extra zone
  if (rank < remainder)
    return Nx_rank_base + 1;
  else 
    return Nx_rank_base;
}


double get_x_left(int Nx, double Lx, double dx, int rank, int numRanks) {
  int remainder;
  int Nx_rank_base;

  double Lx_rank_L;
  double Lx_rank_R; 

  Nx_rank_base = Nx / numRanks;
  remainder = Nx % numRanks;

  Lx_rank_R = Nx_rank_base * dx;
  Lx_rank_L = (Nx_rank_base+1) * dx;

  if (rank < remainder) // it is on the left side
    return rank * Lx_rank_R;
  else
    return remainder * Lx_rank_R + (rank - remainder) * Lx_rank_L;
}
