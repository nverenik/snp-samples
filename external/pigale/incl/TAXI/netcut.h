/****************************************************************************
**
** Copyright (C) 2001 Hubert de Fraysseix, Patrice Ossona de Mendez.
** All rights reserved.
** This file is part of the PIGALE Toolkit.
**
** This file may be distributed under the terms of the GNU Public License
** appearing in the file LICENSE.HTML included in the packaging of this file.
**
*****************************************************************************/
#ifndef NETCUT_H
#define NETCUT_H

/*! 
\defgroup core Pigale core library
*/
/*!
\file netcut.h
 \ingroup core
\brief Embedder and Partitioner class definitions
\todo  
\b Eigenspaces \b dimensions \b optimization.
Optimize eigenspaces dimensions over the distances defined on the cellular algebra \f$ \celalg{A} \f$ generated by the adjacency matrix \f$ A \f$
\todo 
\b Q \b distance.
Test the distance
\f[ d^2(i,j)=\begin{cases}
0,&\text{if $i=j$}\\
1,&\text{if $i$ and $j$ are non adjacent}\\
1-\frac{1}{\sqrt{d(i)d(j)}},&\text{otherwise}
\end{cases}\f]
Remark that the matrix \f$ \left(\frac{A_{i,j}}{\sqrt{d(i)d(j)}}\right)_{i,j} \f$ is symmetric,
stochastic and hence have real eigenvalues, the largest one being equal to \f$ 1\f$.
Nevertheless, a multigraph is not reconstructible from this distance as replacing every edge by 
\f$ k \f$ parallel edges does not affect the distances between the vertices.
*/
#include <TAXI/graphs.h>
#include <TAXI/Tbase.h>
#include <TAXI/Tmessage.h>
#include <TAXI/Tpoint.h>
#include <TAXI/embedrn.h>

//! Threshold for clustering optimization termination
/*!
  Clustering optimization process is repeated until the inertia is
  modified by less than \a SEUIL
*/
#define SEUIL 0.00001

int & useDistance();

//! Triangular matrix handling as a symmetric square matrix.
template <class T> class TriangularMatrix 
{private:
  svector<T> buffer;
  int Index(int i,int j) 
      {int a,b;
      if(i == j)Twait("ERROR ACCESS  TriangularMatrix ");
      if (i>j) {a=i; b=j;}
      else {a=j; b=i;}
      return a*(a-1)/2+b;
      }

public:
  //! Constructor
  /*!
    \param n Size of the matrix
  */
  TriangularMatrix(int n): buffer(n*(n-1)/2) {}
  //! Resizes the matrix
  void resize(int n) {buffer.resize(0,n*(n-1)/2-1);}
  //! Clears the matrix entries
  void clear() {buffer.clear();}
  //! Returns a reference to an element
  T & operator()(int i, int j) {return buffer[Index(i,j)];}
  //! Returns a constant reference to an element
  const T & operator()(int i, int j) const {return buffer[Index(i,j)];}
};
//! Rectangular matrix
template <class T> class RectangularMatrix 
{
  svector<T> buffer;
  int ncols;
  int Index(int i,int j)
      {return i*ncols+j;
      }

public:
  //! Constructor
  /*!
    \param nr Number of rows
    \param nc Number of columns
  */
  RectangularMatrix(int nr,int nc): buffer(nr*nc), ncols(nc) {}
  //! Clears the matrix entries
  void clear() {buffer.clear();}
  //! Resizes the matrix
  void resize(int nr, int nc) {buffer.resize(0,nr*nc-1); ncols=nc;}
  //! Returns a reference to an element
  T & operator()(int i, int j) {return buffer[Index(i,j)];}
  //! Returns a constant reference to an element
  const T & operator()(int i, int j) const {return buffer[Index(i,j)];}
};

//! Class used to spread vertices in classes
struct Classifier {
  svector<int> Class; //!< Classes (\f$ V\rightarrow [1,{\rm noc}]\f$ mapping)
  svector<int> Cardinal; //!< Cardinal of the classes

  //! Constructor
  /*!
    \param n Number of vertices
    \param noc Number of classes
  */
  Classifier(int n, int noc): Class(n), Cardinal(noc) {}
  //! Clear the partition
  void clear() {Class.clear(); Cardinal.clear();}
  //! Affects a vertex to a class
  /*!
    \param i Vertex index
    \param c Class index
  */
  void affect(int i,int c) {Class[i]=c; Cardinal[c]++;}
};

//! Locals of the embedder
struct Locals 
{
  TriangularMatrix<double> ProjectDist; //!< Distances in some eigenspace
  RectangularMatrix<double> BaryCoord; //!< Barycenters coordinates
  Classifier Part; //!< Class affectation
  //! Constructor
  /*!
    \param n Number of vertices
    \param ms Maximum number of barycenters
    \param noc Number of classes
  */
  Locals(int n, int ms, int noc): ProjectDist(n),BaryCoord(ms,n-1),Part(n,noc)
  {ProjectDist.clear();}
};

//! Topological graph embedded in \f$ \mathbb{R}^n\f$ 
class EmbedRnGraph : public TopologicalGraph
{public :
  Prop<short> vcolor;  //!< colors of the vertices
 Prop<long> vlabel; //!< labels of the vertices
 Prop<int> ewidth; //!< width of the edges 
 Prop<short> ecolor; //!< colors of the edges 
 Prop<long> elabel;  //!< labels of the edges 
 svector<int> degree; //!< degrees of the vertices 
 int **vvadj;                      //!< vertex/vertex sorted adjacency 
 int **inList;  //!< incoming adjacency lists 
 int **outList; //!< outgoing adjacency lists 
 double **Distances;               //!< squared Euclidean distances 
 double **Coords;                  //!< coordinates in \f$ \mathbb{R}^{n-1}\f$ 
 bool ok; //!< computation status 
 svector<int> indegree; //!< indegrees of the vertices 
 svector<int> outdegree; //!< outdegrees of the vertices 
 svector<double> EigenValues; //!< eigenvalues 
/*!
   \f[ d^2(i,j)=1-\frac{|N(i)\cap N(j)|}{|N(i)|+|N(j)|},\f]
   where \f$ N(i) \f$ is the set including \f$ i \f$ and its neighbors. 
 */
 int ComputeCzekanovskiDistances(); //!< Computes Czekanovski-Dice distances 
 int ComputeAdjacenceMatrix(); //!< Computes the vertex/vertex adjacency matrix 
/*!
   \f[ d^2(i,j)=1-\frac{|N(i)\cap N(j)|}{|N(i)|+|N(j)|},\f]
   where \f$ N(i) \f$ is the set including \f$ i \f$ and its neighbors. 
 */
 double ComputeCzekanovskiDistance(int vertex1,int vertex2); //!< Computes Czekanovski-Dice distances between two vertices 
/*!
   \f[ d^2(i,j)=1-\frac{|N^-(i)\cap N^-(j)|}{|N^-(i)|+|N^-(j)|}-\frac{|N^+(i)\cap N^+(j)|}{|N^+(i)|+|N^+(j)|},\f]
   where \f$ N^-(i) \f$ is the set including \f$ i \f$ and its incoming neighbors, and 
   \f$ N^+(i) \f$ is the set including \f$ i \f$ and its outgoing neighbors.
 */
 int ComputeOrientDistances(); //!< Computes oriented distances 
 int ComputeInOutList(); //!< Computes incoming and outgoing neighbor sets 
/*!
   \f[ d^2(i,j)=1-\frac{|N^-(i)\cap N^-(j)|}{|N^-(i)|+|N^-(j)|},\f]
   where \f$ N^-(i) \f$ is the set including \f$ i \f$ and its incoming neighbors.
 */
 double ComputeInDist(int vertex1,int vertex2); //!< Computes the part of the oriented distance due to incoming edges
/*!
   \f[ d^2(i,j)=1-\frac{|N^+(i)\cap N^+(j)|}{|N^+(i)|+|N^+(j)|},\f]
   where \f$ N^+(i) \f$ is the set including \f$ i \f$ and its outgoing neighbors.
 */
 double ComputeOutDist(int vertex1,int vertex2); //!< Computes the part of the oriented distance due to outgoing edges
/*!
   \f[ d^2(i,j)=\begin{cases}
   0,&\text{if $i=j$ or $i$ and $j$ are adjacent}\\
   1,&\text{otherwise}
   \end{cases}
   \f]
 */
 int ComputeAdjacenceDistances(); //!< Computes adjacency distances 
 /*!
   Actually computes the bilinear form corresponding to the Laplacian distance on the complement graph:
   \f[ b(i,j)=\begin{cases}
   n-d(i),&\text{if $i=j$}\\
   0,&\text{if $i$ and $j$ are adjacent}\\
   -1,&\text{otherwise}
   \end{cases}
   \f]
   It is semi-definite positive, as \f$ B=\bar{D}\bar{D}^{\rm t}\f$ if
   \f$ \bar{D} \f$ is the oriented adjacency matrix of the complement graph for any
   arbitrary orientation.

   The distance corresponding to this bilinear form is:
   \f[ d^2(i,j)=\begin{cases}
    0,&\text{if $i=j$}\\
    2n-d(i)-d(j),&\text{if $i$ and $j$ are adjacent}\\
    2n-d(i)-d(j)+2,&\text{otherwise}
    \end{cases}
   \f]
 */   
 int ComputeLaplacianDistances(); //!< Computes Laplacian distances on the complement graph
 /*!
   \f[ d^2(i,j)=\begin{cases}
   0,&\text{if $i=j$}\\
   1-\frac{2}{n},&\text{if $i$ and $j$ are adjacent}\\
   1,&\text{otherwise}
   \end{cases}
   \f]
 */
 int ComputeAdjacenceMDistances(); //!< Computes translated adjacency distances 
 /*!
   \f[ d^2(i,j)=\begin{cases}
   0,&\text{if $i=j$}\\
   1-\frac{2}{d(i)+d(j)+2},&\text{if $i$ and $j$ are adjacent}\\
   1,&\text{otherwise}
   \end{cases}
   \f]
   where \f$ d(i) \f$ is the degree of \f$ i \f$
 */
 int ComputeBisectDistances(); //!< Computes bisection distances 
 /*!
   \f[ d^2(i,j)=x^2(i)+y^2(i),\f]
   where \f$ x(i) \f$ and \f$ y(i) \f$ are the coordinates of \f$ i \f$ in the plane.
 */
  int ComputeR2Distances(); //!< Computes distances in \f$ \mathbb{R}^2\f$ 
  int ComputeQDistances();
 //! Constructor
 /*!
   \param G Graph to be embedded
 */
  EmbedRnGraph(Graph &G,int usedDistance) :
    TopologicalGraph(G),
    vcolor(G.PV(),PROP_COLOR,5),vlabel(G.PV(),PROP_LABEL),
    ewidth(G.PE(),PROP_WIDTH,1), ecolor(G.PE(),PROP_COLOR,1),
    elabel(G.PE(),PROP_LABEL),
    vvadj(NULL),inList(NULL),outList(NULL),
    Distances(NULL),Coords(NULL),ok(true)
    {init(usedDistance);}
  
  //! Destructor
  ~EmbedRnGraph() {release();}

private :
  void init(int usedDistance); //!< Class initialization 
 void release(); //!< Member destructions 
};

//! Class used to partition a graph using a factorial embedding in \f$ \mathbb{R}^{n-1} \f$
class SplitGraph : public EmbedRnGraph
{public:
  svector<int> ClassNumber;             //!< Class index of a vertex
  svector<int> NumberElementsInClass;   //!< Cardinality of the classes
  int NumberOfClasses;  //!< Requested number of classes
  int MinDimension; //!< Minimum dimension to be used for optimization
   /*!< Fixed to \f$ \max (1,\#\text{classes}-2) \f$ by init() */
  int MaxDimension; //!< Maximum dimension to be used for optimization
  /*!< Actually fixed to \f$ \min (n-1,\#\text{classes}) \f$ by init() */
  int CurrentNumberOfClasses; //!< Current number of classes
    //! Constructor
    /*!
      \param G Graph to be partitioned
      \param numclasses Requested number of classes
      \param maxdim Maximum dimension  to be used for optimization
      \warning \p maxdim is overridden by init()
    */
  SplitGraph(Graph &G,int numclasses,int maxdim,int usedDistance) :
      EmbedRnGraph(G,usedDistance),
      NumberOfClasses(numclasses),MaxDimension(maxdim),CurrentNumberOfClasses(0)
      {init();}
    //! Destructor
    ~SplitGraph() {}
      
    void ComputeMaxDistance3d();
    //! Creates a new class
    /*!
      \param dimension Current space dimension
      \param worst Index of the point used as a representative for the new class
    */
    void NewClass(int dimension,int worst);
    //! Computes projective distance
    void ComputeProjectDistance(int dimension);
    //! Partitions into 2 initial classes and computes projective distance
    void SearchFarVertices(int dimension);
    //! Searches worst represented point
    /*!
      \param dimension Current space dimension
      \param worst Index of the worst represented point (returned value)
    */
    void SearchWorst(int dimension,int &worst); 
    //! Partitions into 2 classes with given representatives
    void AffectExtrems(int extrem0,int extrem1);
    //! Builds classes from the barycenters
    void BuildClasses(int dimension, double& inertie,int& worst);
    //! Representative affectation optimization
    void Optimize(int dimension,int& worst,double& inertie);
    //! Computes the barycenters of the classes
    void ComputeBarycenters(int dimension);
    //! Computes the total inertia of the partition
    double TotalInertia(double& ClassVarianceNumber);
    //! Computes the requested partition
    int Segment();
    
    private :
      //! Class initialization
      void init();
    };



// DIAG.CPP
int diag(double **Coords,int nb_vertex,double **Distances,svector<double>& EigenValues,bool Project = true);
#endif