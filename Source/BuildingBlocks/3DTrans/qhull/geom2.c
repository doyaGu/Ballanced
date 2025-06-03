/*<html><pre>  -<a                             href="qh-geom.htm"
  >-------------------------------</a><a name="TOP">-</a>


   geom2.c 
   infrequently used geometric routines of qhull

   see qh-geom.htm and geom.h

   copyright (c) 1993-2001 The Geometry Center        

   frequently used code goes into geom.c
*/
   
#include "qhull_a.h"
#include <assert.h>  

#if defined(macintosh)
	namespace QHULL
	{
#endif

/*================== functions in alphabetic order ============*/

coordT *qh_copypoints (coordT *points, int numpoints, int dimension) {
	assert(0);
	return NULL;
} /* copypoints */

void qh_crossproduct (int dim, realT vecA[3], realT vecB[3], realT vecC[3]){
	assert(0);
} /* vcross */

/*-<a                             href="qh-geom.htm#TOC"
  >-------------------------------</a><a name="determinant">-</a>
  
  qh_determinant( rows, dim, nearzero )
    compute signed determinant of a square matrix
    uses qh.NEARzero to test for degenerate matrices

  returns:
    determinant
    overwrites rows and the matrix
    if dim == 2 or 3
      nearzero iff determinant < qh NEARzero[dim-1]
      (not quite correct, not critical)
    if dim >= 4
      nearzero iff diagonal[k] < qh NEARzero[k]
*/
realT qh_determinant (realT **rows, int dim, boolT *nearzero) {
  realT det=0;
  int i;
  boolT sign= False;

  *nearzero= False;
  if (dim < 2) {
    fprintf (qh ferr, "qhull internal error (qh_determinate): only implemented for dimension >= 2\n");
    qh_errexit (qh_ERRqhull, NULL, NULL);
  }else if (dim == 2) {
    det= det2_(rows[0][0], rows[0][1],
		 rows[1][0], rows[1][1]);
    if (fabs_(det) < qh NEARzero[1])  /* not really correct, what should this be? */
      *nearzero= True;
  }else if (dim == 3) {
    det= det3_(rows[0][0], rows[0][1], rows[0][2],
		 rows[1][0], rows[1][1], rows[1][2],
		 rows[2][0], rows[2][1], rows[2][2]);
    if (fabs_(det) < qh NEARzero[2])  /* not really correct, what should this be? */
      *nearzero= True;
  }else {	
    qh_gausselim(rows, dim, dim, &sign, nearzero);  /* if nearzero, diagonal still ok*/
    det= 1.0;
    for (i= dim; i--; )
      det *= (rows[i])[i];
    if (sign)
      det= -det;
  }
  return det;
} /* determinant */

realT qh_detjoggle (pointT *points, int numpoints, int dimension) {
	assert(0);
	return 0;
} /* detjoggle */

/*-<a                             href="qh-geom.htm#TOC"
  >-------------------------------</a><a name="detroundoff">-</a>
  
  qh_detroundoff()
    determine maximum roundoff errors from
      REALepsilon, REALmax, REALmin, qh.hull_dim, qh.MAXabs_coord, 
      qh.MAXsumcoord, qh.MAXwidth, qh.MINdenom_1

    accounts for qh.SETroundoff, qh.RANDOMdist, qh MERGEexact
      qh.premerge_cos, qh.postmerge_cos, qh.premerge_centrum,
      qh.postmerge_centrum, qh.MINoutside,
      qh_RATIOnearinside, qh_COPLANARratio, qh_WIDEcoplanar

  returns:
    sets qh.DISTround, etc. (see below)
    appends precision constants to qh.qhull_options

  see:
    qh_maxmin() for qh.NEARzero

  design:
    determine qh.DISTround for distance computations
    determine minimum denominators for qh_divzero
    determine qh.ANGLEround for angle computations
    adjust qh.premerge_cos,... for roundoff error
    determine qh.ONEmerge for maximum error due to a single merge
    determine qh.NEARinside, qh.MAXcoplanar, qh.MINvisible,
      qh.MINoutside, qh.WIDEfacet
    initialize qh.max_vertex and qh.minvertex
*/
void qh_detroundoff (void) {

  qh_option ("_max-width", NULL, &qh MAXwidth);
  if (!qh SETroundoff) {
    qh DISTround= qh_distround (qh hull_dim, qh MAXabs_coord, qh MAXsumcoord);
    if (qh RANDOMdist)
      qh DISTround += qh RANDOMfactor * qh MAXabs_coord;
    qh_option ("Error-roundoff", NULL, &qh DISTround);
  }
  qh MINdenom= qh MINdenom_1 * qh MAXabs_coord;
  qh MINdenom_1_2= sqrt (qh MINdenom_1 * qh hull_dim) ;  /* if will be normalized */
  qh MINdenom_2= qh MINdenom_1_2 * qh MAXabs_coord;
                                              /* for inner product */
  qh ANGLEround= 1.01 * qh hull_dim * REALepsilon;
  if (qh RANDOMdist)
    qh ANGLEround += qh RANDOMfactor;
  if (qh premerge_cos < REALmax/2) {
    qh premerge_cos -= qh ANGLEround;
    if (qh RANDOMdist) 
      qh_option ("Angle-premerge-with-random", NULL, &qh premerge_cos);
  }
  if (qh postmerge_cos < REALmax/2) {
    qh postmerge_cos -= qh ANGLEround;
    if (qh RANDOMdist)
      qh_option ("Angle-postmerge-with-random", NULL, &qh postmerge_cos);
  }
  qh premerge_centrum += 2 * qh DISTround;    /*2 for centrum and distplane()*/
  qh postmerge_centrum += 2 * qh DISTround;
  if (qh RANDOMdist && (qh MERGEexact || qh PREmerge))
    qh_option ("Centrum-premerge-with-random", NULL, &qh premerge_centrum);
  if (qh RANDOMdist && qh POSTmerge)
    qh_option ("Centrum-postmerge-with-random", NULL, &qh postmerge_centrum);
  { /* compute ONEmerge, max vertex offset for merging simplicial facets */
    realT maxangle= 1.0, maxrho;
    
    minimize_(maxangle, qh premerge_cos);
    minimize_(maxangle, qh postmerge_cos);
    /* max diameter * sin theta + DISTround for vertex to its hyperplane */
    qh ONEmerge= sqrt (qh hull_dim) * qh MAXwidth *
      sqrt (1.0 - maxangle * maxangle) + qh DISTround;  
    maxrho= qh hull_dim * qh premerge_centrum + qh DISTround;
    maximize_(qh ONEmerge, maxrho);
    maxrho= qh hull_dim * qh postmerge_centrum + qh DISTround;
    maximize_(qh ONEmerge, maxrho);
    if (qh MERGING)
      qh_option ("_one-merge", NULL, &qh ONEmerge);
  }
  qh NEARinside= qh ONEmerge * qh_RATIOnearinside; /* only used if qh KEEPnearinside */
  if (qh JOGGLEmax < REALmax/2 && (qh KEEPcoplanar || qh KEEPinside)) {
    realT maxdist;	       /* adjust qh.NEARinside for joggle */
    qh KEEPnearinside= True;   
    maxdist= sqrt (qh hull_dim) * qh JOGGLEmax + qh DISTround;
    maxdist= 2*maxdist;        /* vertex and coplanar point can joggle in opposite directions */
    maximize_(qh NEARinside, maxdist);  /* must agree with qh_nearcoplanar() */
  }
  if (qh KEEPnearinside)
    qh_option ("_near-inside", NULL, &qh NEARinside);
  if (qh JOGGLEmax < qh DISTround) {
    fprintf (qh ferr, "qhull error: the joggle for 'QJn', %.2g, is below roundoff for distance computations, %.2g\n",
         qh JOGGLEmax, qh DISTround);
    qh_errexit (qh_ERRinput, NULL, NULL);
  }
  if (qh MINvisible > REALmax/2) {
    if (!qh MERGING)
      qh MINvisible= qh DISTround;
    else if (qh hull_dim <= 3)
      qh MINvisible= qh premerge_centrum;
    else
      qh MINvisible= qh_COPLANARratio * qh premerge_centrum;
    if (qh APPROXhull && qh MINvisible > qh MINoutside)
      qh MINvisible= qh MINoutside;
    qh_option ("Visible-distance", NULL, &qh MINvisible);
  }
  if (qh MAXcoplanar > REALmax/2) {
    qh MAXcoplanar= qh MINvisible;
    qh_option ("U-coplanar-distance", NULL, &qh MAXcoplanar);
  }
  if (!qh APPROXhull) {             /* user may specify qh MINoutside */
    qh MINoutside= 2 * qh MINvisible;
    if (qh premerge_cos < REALmax/2) 
      maximize_(qh MINoutside, (1- qh premerge_cos) * qh MAXabs_coord);
    qh_option ("Width-outside", NULL, &qh MINoutside);
  }
  qh WIDEfacet= qh MINoutside;
  maximize_(qh WIDEfacet, qh_WIDEcoplanar * qh MAXcoplanar); 
  maximize_(qh WIDEfacet, qh_WIDEcoplanar * qh MINvisible); 
  qh_option ("_wide-facet", NULL, &qh WIDEfacet);
  if (qh MINvisible > qh MINoutside + 3 * REALepsilon 
  && !qh BESToutside && !qh FORCEoutput)
    fprintf (qh ferr, "qhull input warning: minimum visibility V%.2g is greater than \nminimum outside W%.2g.  Flipped facets are likely.\n",
	     qh MINvisible, qh MINoutside);
  qh max_vertex= qh DISTround;
  qh min_vertex= -qh DISTround;
  /* numeric constants reported in printsummary */
} /* detroundoff */

/*-<a                             href="qh-geom.htm#TOC"
  >-------------------------------</a><a name="detsimplex">-</a>
  
  qh_detsimplex( apex, points, dim, nearzero )
    compute determinant of a simplex with point apex and base points

  returns:
     signed determinant and nearzero from qh_determinant

  notes:
     uses qh.gm_matrix/qh.gm_row (assumes they're big enough)

  design:
    construct qm_matrix by subtracting apex from points
    compute determinate
*/
realT qh_detsimplex(pointT *apex, setT *points, int dim, boolT *nearzero) {
  pointT *coorda, *coordp, *gmcoord, *point, **pointp;
  coordT **rows;
  int k,  i=0;
  realT det;

  zinc_(Zdetsimplex);
  gmcoord= qh gm_matrix;
  rows= qh gm_row;
  FOREACHpoint_(points) {
    if (i == dim)
      break;
    rows[i++]= gmcoord;
    coordp= point;
    coorda= apex;
    for (k= dim; k--; )
      *(gmcoord++)= *coordp++ - *coorda++;
  }
  if (i < dim) {
    fprintf (qh ferr, "qhull internal error (qh_detsimplex): #points %d < dimension %d\n", 
               i, dim);
    qh_errexit (qh_ERRqhull, NULL, NULL);
  }
  det= qh_determinant (rows, dim, nearzero);
  trace2((qh ferr, "qh_detsimplex: det=%2.2g for point p%d, dim %d, nearzero? %d\n",
	  det, qh_pointid(apex), dim, *nearzero)); 
  return det;
} /* detsimplex */

realT qh_distnorm (int dim, pointT *point, pointT *normal, realT *offsetp) {
	assert(0);
	return 0;
} /* distnorm */

realT qh_distround (int dimension, realT maxabs, realT maxsumabs) {
	assert(0);
	return 0;
} /* distround */

/*-<a                             href="qh-geom.htm#TOC"
  >-------------------------------</a><a name="divzero">-</a>
  
  qh_divzero( numer, denom, mindenom1, zerodiv )
    divide by a number that's nearly zero
    mindenom1= minimum denominator for dividing into 1.0

  returns:
    quotient
    sets zerodiv and returns 0.0 if it would overflow
  
  design:
    if numer is nearly zero and abs(numer) < abs(denom)
      return numer/denom
    else if numer is nearly zero
      return 0 and zerodiv
    else if denom/numer non-zero
      return numer/denom
    else
      return 0 and zerodiv
*/
realT qh_divzero (realT numer, realT denom, realT mindenom1, boolT *zerodiv) {
  realT temp, numerx, denomx;
  

  if (numer < mindenom1 && numer > -mindenom1) {
    numerx= fabs_(numer);
    denomx= fabs_(denom);
    if (numerx < denomx) {
      *zerodiv= False;
      return numer/denom;
    }else {
      *zerodiv= True;
      return 0.0;
    }
  }
  temp= denom/numer;
  if (temp > mindenom1 || temp < -mindenom1) {
    *zerodiv= False;
    return numer/denom;
  }else {
    *zerodiv= True;
    return 0.0;
  }
} /* divzero */
  

realT qh_facetarea (facetT *facet) {
	assert(0);
	return 0;
} /* facetarea */

realT qh_facetarea_simplex (int dim, coordT *apex, setT *vertices, 
        vertexT *notvertex,  boolT toporient, coordT *normal, realT *offset) {
	assert(0);
	return 0;
} /* facetarea_simplex */

pointT *qh_facetcenter (setT *vertices) {
	assert(0);
	return NULL;
} /* facetcenter */

facetT *qh_findgooddist (pointT *point, facetT *facetA, realT *distp, 
               facetT **facetlist) {
	assert(0);
	return NULL;
}  /* findgooddist */
    
void qh_getarea (facetT *facetlist) {
	assert(0);
} /* gram_schmidt */

boolT qh_gram_schmidt(int dim, realT **row) {
	assert(0);
  return True;
} /* gram_schmidt */

boolT qh_inthresholds (coordT *normal, realT *angle) {
  boolT within= True;
	assert(0);
  return within;
} /* inthresholds */
    

void qh_joggleinput (void) {
  int size, i, seed;
  coordT *coordp, *inputp;
  realT randr, randa, randb;

  if (!qh input_points) { /* first call */
    qh input_points= qh first_point;
    qh input_malloc= qh POINTSmalloc;
    size= qh num_points * qh hull_dim * sizeof(coordT);
    if (!(qh first_point=(coordT*)malloc(size))) {
      fprintf(qh ferr, "qhull error: insufficient memory to joggle %d points\n",
          qh num_points);
      qh_errexit(qh_ERRmem, NULL, NULL);
    }
    qh POINTSmalloc= True;
    if (qh JOGGLEmax == 0.0) {
      qh JOGGLEmax= qh_detjoggle (qh input_points, qh num_points, qh hull_dim);
      qh_option ("QJoggle", NULL, &qh JOGGLEmax);
    }
  }else {                 /* repeated call */
    if (!qh RERUN && qh build_cnt > qh_JOGGLEretry) {
      if (((qh build_cnt-qh_JOGGLEretry-1) % qh_JOGGLEagain) == 0) {
	realT maxjoggle= qh MAXwidth * qh_JOGGLEmaxincrease;
	if (qh JOGGLEmax < maxjoggle) {
	  qh JOGGLEmax *= qh_JOGGLEincrease;
	  minimize_(qh JOGGLEmax, maxjoggle); 
	}
      }
    }
    qh_option ("QJoggle", NULL, &qh JOGGLEmax);
  }
  if (qh build_cnt > 1 && qh JOGGLEmax > fmax_(qh MAXwidth/4, 0.1)) {
      fprintf (qh ferr, "qhull error: the current joggle for 'QJn', %.2g, is too large for the width\nof the input.  If possible, recompile Qhull with higher-precision reals.\n",
	        qh JOGGLEmax);
      qh_errexit (qh_ERRqhull, NULL, NULL);
  }
  /* for some reason, using qh ROTATErandom and qh_RANDOMseed does not repeat the run. Use 'TRn' instead */
  seed= qh_RANDOMint;
  qh_option ("_joggle-seed", &seed, NULL);
  trace0((qh ferr, "qh_joggleinput: joggle input by %2.2g with seed %d\n", 
    qh JOGGLEmax, seed));
  inputp= qh input_points;
  coordp= qh first_point;
  randa= 2.0 * qh JOGGLEmax/qh_RANDOMmax;
  randb= -qh JOGGLEmax;
  size= qh num_points * qh hull_dim;
  for (i= size; i--; ) {
    randr= qh_RANDOMint;
    *(coordp++)= *(inputp++) + (randr * randa + randb);
  }
  if (qh DELAUNAY) {
    qh last_low= qh last_high= qh last_newhigh= REALmax;
    qh_setdelaunay (qh hull_dim, qh num_points, qh first_point);
  }
} /* joggleinput */

realT *qh_maxabsval (realT *normal, int dim) {
	assert(0);
  return 0;
} /* maxabsval */


setT *qh_maxmin(pointT *points, int numpoints, int dimension) {
  int k;
  realT maxcoord, temp;
  pointT *minimum, *maximum, *point, *pointtemp;
  setT *set;

  qh max_outside= 0.0;
  qh MAXabs_coord= 0.0;
  qh MAXwidth= -REALmax;
  qh MAXsumcoord= 0.0;
  qh min_vertex= 0.0;
  qh WAScoplanar= False;
  if (qh ZEROcentrum)
    qh ZEROall_ok= True;
  if (REALmin < REALepsilon && REALmin < REALmax && REALmin > -REALmax
  && REALmax > 0.0 && -REALmax < 0.0)
    ; /* all ok */
  else {
    fprintf (qh ferr, "qhull error: floating point constants in user.h are wrong\n\
REALepsilon %g REALmin %g REALmax %g -REALmax %g\n",
	     REALepsilon, REALmin, REALmax, -REALmax);
    qh_errexit (qh_ERRinput, NULL, NULL);
  }
  set= qh_settemp(2*dimension);
  for(k= 0; k < dimension; k++) {
    if (points == qh GOODpointp)
      minimum= maximum= points + dimension;
    else
      minimum= maximum= points;
    FORALLpoint_(points, numpoints) {
      if (point == qh GOODpointp)
	continue;
      if (maximum[k] < point[k])
	maximum= point;
      else if (minimum[k] > point[k])
	minimum= point;
    }
    if (k == dimension-1) {
      qh MINlastcoord= minimum[k];
      qh MAXlastcoord= maximum[k];
    }
    if (qh SCALElast && k == dimension-1)
      maxcoord= qh MAXwidth;
    else {
      maxcoord= fmax_(maximum[k], -minimum[k]);
      if (qh GOODpointp) {
        temp= fmax_(qh GOODpointp[k], -qh GOODpointp[k]);
        maximize_(maxcoord, temp);
      }
      temp= maximum[k] - minimum[k];
      maximize_(qh MAXwidth, temp);
    }
    maximize_(qh MAXabs_coord, maxcoord);
    qh MAXsumcoord += maxcoord;
    qh_setappend (&set, maximum);
    qh_setappend (&set, minimum);
    /* calculation of qh NEARzero is based on error formula 4.4-13 of
       Golub & van Loan, authors say n^3 can be ignored and 10 be used in
       place of rho */
    qh NEARzero[k]= 80 * qh MAXsumcoord * REALepsilon;
  }
  if (qh IStracing >=1) {
   // qh_printpoints (qh ferr, "qh_maxmin: found the max and min points (by dim):", set); AGoTH
  }
  return(set);
} /* maxmin */

/*-<a                             href="qh-geom.htm#TOC"
  >-------------------------------</a><a name="maxouter">-</a>

  qh_maxouter()
    return maximum distance from facet to outer plane
    normally this is qh.max_outside+qh.DISTround
    does not include qh.JOGGLEmax

  see:
    qh_outerinner()
    
  notes:
    need to add another qh.DISTround if testing actual point with computation

  for joggle:
    qh_setfacetplane() updated qh.max_outer for Wnewvertexmax (max distance to vertex)
    need to use Wnewvertexmax since could have a coplanar point for a high 
      facet that is replaced by a low facet
    need to add qh.JOGGLEmax if testing input points
*/
realT qh_maxouter (void) {
  realT dist;

  dist= fmax_(qh max_outside, qh DISTround);
  dist += qh DISTround;
  trace4((qh ferr, "qh_maxouter: max distance from facet to outer plane is %2.2g max_outside is %2.2g\n", dist, qh max_outside));
  return dist;
} /* maxouter */

/*-<a                             href="qh-geom.htm#TOC"
  >-------------------------------</a><a name="maxsimplex">-</a>
  
  qh_maxsimplex( dim, maxpoints, points, numpoints, simplex )
    determines maximum simplex for a set of points 
    starts from points already in simplex
    skips qh.GOODpointp (assumes that it isn't in maxpoints)
  
  returns:
    simplex with dim+1 points

  notes:
    assumes at least pointsneeded points in points
    maximizes determinate for x,y,z,w, etc.
    uses maxpoints as long as determinate is clearly non-zero

  design:
    initialize simplex with at least two points
      (find points with max or min x coordinate)
    for each remaining dimension
      add point that maximizes the determinate
        (use points from maxpoints first)    
*/
void qh_maxsimplex (int dim, setT *maxpoints, pointT *points, int numpoints, setT **simplex) {
  pointT *point, **pointp, *pointtemp, *maxpoint, *minx=NULL, *maxx=NULL;
  boolT nearzero, maxnearzero= False;
  int k, sizinit;
  realT maxdet= -REALmax, det, mincoord= REALmax, maxcoord= -REALmax;

  sizinit= qh_setsize (*simplex);
  if (sizinit < 2) {
    if (qh_setsize (maxpoints) >= 2) {
      FOREACHpoint_(maxpoints) {
        if (maxcoord < point[0]) {
          maxcoord= point[0];
          maxx= point;
        }
	if (mincoord > point[0]) {
          mincoord= point[0];
          minx= point;
        }
      }
    }else {
      FORALLpoint_(points, numpoints) {
	if (point == qh GOODpointp)
	  continue;
        if (maxcoord < point[0]) {
	  maxcoord= point[0];
          maxx= point;
        }
	if (mincoord > point[0]) {
          mincoord= point[0];
          minx= point;
	}
      }
    }
    qh_setunique (simplex, minx);
    if (qh_setsize (*simplex) < 2)
      qh_setunique (simplex, maxx);
    sizinit= qh_setsize (*simplex);
    if (sizinit < 2) {
      qh_precision ("input has same x coordinate");
      if (zzval_(Zsetplane) > qh hull_dim+1) {
	fprintf (qh ferr, "qhull precision error (qh_maxsimplex for voronoi_center):\n%d points with the same x coordinate.\n",
		 qh_setsize(maxpoints)+numpoints);
	qh_errexit (qh_ERRprec, NULL, NULL);
      }else {
	fprintf (qh ferr, "qhull input error: input is less than %d-dimensional since it has the same x coordinate\n", qh hull_dim);
	qh_errexit (qh_ERRinput, NULL, NULL);
      }
    }
  }
  for(k= sizinit; k < dim+1; k++) {
    maxpoint= NULL;
    maxdet= -REALmax;
    FOREACHpoint_(maxpoints) {
      if (!qh_setin (*simplex, point)) {
        det= qh_detsimplex(point, *simplex, k, &nearzero);
        if ((det= fabs_(det)) > maxdet) {
	  maxdet= det;
          maxpoint= point;
	  maxnearzero= nearzero;
        }
      }
    }
    if (!maxpoint || maxnearzero) {
      zinc_(Zsearchpoints);
      if (!maxpoint) {
        trace0((qh ferr, "qh_maxsimplex: searching all points for %d-th initial vertex.\n", k+1));
      }else {
        trace0((qh ferr, "qh_maxsimplex: searching all points for %d-th initial vertex, better than p%d det %2.2g\n",
		k+1, qh_pointid(maxpoint), maxdet));
      }
      FORALLpoint_(points, numpoints) {
	if (point == qh GOODpointp)
	  continue;
        if (!qh_setin (*simplex, point)) {
          det= qh_detsimplex(point, *simplex, k, &nearzero);
          if ((det= fabs_(det)) > maxdet) {
	    maxdet= det;
            maxpoint= point;
	    maxnearzero= nearzero;
	  }
        }
      }
    } /* !maxpoint */
    if (!maxpoint) {
      fprintf (qh ferr, "qhull internal error (qh_maxsimplex): not enough points available\n");
      qh_errexit (qh_ERRqhull, NULL, NULL);
    }
    qh_setappend(simplex, maxpoint);
    trace1((qh ferr, "qh_maxsimplex: selected point p%d for %d`th initial vertex, det=%2.2g\n",
	    qh_pointid(maxpoint), k+1, maxdet));
  } /* k */ 
} /* maxsimplex */

realT qh_minabsval (realT *normal, int dim) {
	assert(0);
	return 0;
} /* minabsval */


int qh_mindiff (realT *vecA, realT *vecB, int dim) {
	assert(0);
  return 0;
} /* mindiff */



/*-<a                             href="qh-geom.htm#TOC"
  >-------------------------------</a><a name="orientoutside">-</a>
  
  qh_orientoutside( facet  )
    make facet outside oriented via qh.interior_point

  returns:
    True if facet reversed orientation.
*/
boolT qh_orientoutside (facetT *facet) {
  int k;
  realT dist;

  qh_distplane (qh interior_point, facet, &dist);
  if (dist > 0) {
    for (k= qh hull_dim; k--; )
      facet->normal[k]= -facet->normal[k];
    facet->offset= -facet->offset;
    return True;
  }
  return False;
} /* orientoutside */

/*-<a                             href="qh-geom.htm#TOC"
  >-------------------------------</a><a name="outerinner">-</a>
  
  qh_outerinner( facet, outerplane, innerplane  )
    if facet and qh.maxoutdone (i.e., qh_check_maxout)
      returns outer and inner plane for facet
    else
      returns maximum outer and inner plane
    accounts for qh.JOGGLEmax

  see:
    qh_maxouter(), qh_check_bestdist(), qh_check_points()

  notes:
    outerplaner or innerplane may be NULL
    
    includes qh.DISTround for actual points
    adds another qh.DISTround if testing with floating point arithmetic
*/
void qh_outerinner (facetT *facet, realT *outerplane, realT *innerplane) {
  realT dist, mindist;
  vertexT *vertex, **vertexp;

  if (outerplane) {
    if (!qh_MAXoutside || !facet || !qh maxoutdone) {
      *outerplane= qh_maxouter();       /* includes qh.DISTround */
    }else { /* qh_MAXoutside ... */
#if qh_MAXoutside 
      *outerplane= facet->maxoutside + qh DISTround;
#endif
      
    }
    if (qh JOGGLEmax < REALmax/2)
      *outerplane += qh JOGGLEmax * sqrt (qh hull_dim);
  }
  if (innerplane) {
    if (facet) {
      mindist= REALmax;
      FOREACHvertex_(facet->vertices) {
        zinc_(Zdistio);
        qh_distplane (vertex->point, facet, &dist);
        minimize_(mindist, dist);
      }
      *innerplane= mindist - qh DISTround;
    }else 
      *innerplane= qh min_vertex - qh DISTround;
    if (qh JOGGLEmax < REALmax/2)
      *innerplane -= qh JOGGLEmax * sqrt (qh hull_dim);
  }
} /* outerinner */

coordT qh_pointdist(pointT *point1, pointT *point2, int dim) {
	assert(0);
	return 0;
} /* pointdist */


void qh_printmatrix (FILE *fp, char *string, realT **rows, int numrow, int numcol) {
	assert(0);
} /* printmatrix */

  
void qh_printpoints (FILE *fp, char *string, setT *points) {
	assert(0);
} /* printpoints */

  
void qh_projectinput (void) {
	assert(0);
} /* projectinput */

  
void qh_projectpoints (signed char *project, int n, realT *points, 
        int numpoints, int dim, realT *newpoints, int newdim) {
	assert(0);
} /* projectpoints */
        

/*-<a                             href="qh-geom.htm#TOC"
  >-------------------------------</a><a name="rand">-</a>
  
  qh_rand() 
  qh_srand( seed )
    generate pseudo-random number between 1 and 2^31 -2

  notes:
    from Park & Miller's minimimal standard random number generator
       Communications of the ACM, 31:1192-1201, 1988.
    does not use 0 or 2^31 -1
       this is silently enforced by qh_srand()
    can make 'Rn' much faster by moving qh_rand to qh_distplane
*/
int qh_rand_seed= 1;  /* define as global variable instead of using qh */

int qh_rand( void) {
#define qh_rand_a 16807
#define qh_rand_m 2147483647
#define qh_rand_q 127773  /* m div a */
#define qh_rand_r 2836    /* m mod a */
  int lo, hi, test;
  int seed = qh_rand_seed;

  hi = seed / qh_rand_q;  /* seed div q */
  lo = seed % qh_rand_q;  /* seed mod q */
  test = qh_rand_a * lo - qh_rand_r * hi;
  if (test > 0)
    seed= test;
  else
    seed= test + qh_rand_m;
  qh_rand_seed= seed;
  /* seed = seed < qh_RANDOMmax/2 ? 0 : qh_RANDOMmax;  for testing */
  /* seed = qh_RANDOMmax;  for testing */
  return seed;
} /* rand */

void qh_srand( int seed) {
  if (seed < 1)
    qh_rand_seed= 1;
  else if (seed >= qh_rand_m)
    qh_rand_seed= qh_rand_m - 1;
  else
    qh_rand_seed= seed;
} /* qh_srand */

/*-<a                             href="qh-geom.htm#TOC"
  >-------------------------------</a><a name="randomfactor">-</a>
  
  qh_randomfactor()
    return a random factor within qh.RANDOMmax of 1.0

  notes:
    qh.RANDOMa/b are defined in global.c
*/
realT qh_randomfactor (void) {
  realT randr;

  randr= qh_RANDOMint;
  return randr * qh RANDOMa + qh RANDOMb;
} /* randomfactor */

void qh_randommatrix (realT *buffer, int dim, realT **rows) {
	assert(0);
} /* randommatrix */

        
void qh_rotateinput (realT **rows) {
	assert(0);
}  /* rotateinput */

void qh_rotatepoints (realT *points, int numpoints, int dim, realT **row) {
	assert(0);
} /* rotatepoints */  
  

void qh_scaleinput (void) {
	assert(0);
}  /* scaleinput */
  
void qh_scalelast (coordT *points, int numpoints, int dim, coordT low,
		   coordT high, coordT newhigh) {
	assert(0);
} /* scalelast */

void qh_scalepoints (pointT *points, int numpoints, int dim,
	realT *newlows, realT *newhighs) {
	assert(0);
} /* scalepoints */    

       
void qh_setdelaunay (int dim, int count, pointT *points) {
	assert(0);
} /* setdelaunay */

  
boolT qh_sethalfspace (int dim, coordT *coords, coordT **nextp, 
         coordT *normal, coordT *offset, coordT *feasible) {
	assert(0);
	return False;
} /* sethalfspace */

coordT *qh_sethalfspace_all (int dim, int count, coordT *halfspaces, pointT *feasible) {
	assert(0);
	return NULL;
} /* sethalfspace_all */

  
boolT qh_sharpnewfacets () {
  facetT *facet;
  boolT issharp = False;
  int *quadrant, k;
  
  quadrant= (int*)qh_memalloc (qh hull_dim * sizeof(int));
  FORALLfacet_(qh newfacet_list) {
    if (facet == qh newfacet_list) {
      for (k= qh hull_dim; k--; )
      	quadrant[ k]= (facet->normal[ k] > 0);
    }else {
      for (k= qh hull_dim; k--; ) {
        if (quadrant[ k] != (facet->normal[ k] > 0)) {
          issharp= True;
          break;
        }
      }
    }
    if (issharp)
      break;
  }
  qh_memfree( quadrant, qh hull_dim * sizeof(int));
  trace3((qh ferr, "qh_sharpnewfacets: %d\n", issharp));
  return issharp;
} /* sharpnewfacets */

pointT *qh_voronoi_center (int dim, setT *points) {
	assert(0);
  return NULL;
} /* voronoi_center */

#if defined(macintosh)
	} // end namespace QHULL
#endif

