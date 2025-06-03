/*<html><pre>  -<a                             href="qh-io.htm"
  >-------------------------------</a><a name="TOP">-</a>

   io.c 
   Input/Output routines of qhull application

   see qh-io.htm and io.h

   see user.c for qh_errprint and qh_printfacetlist

   unix.c calls qh_readpoints and qh_produce_output

   unix.c and user.c are the only callers of io.c functions
   This allows the user to avoid loading io.o from qhull.a

   copyright (c) 1993-2001 The Geometry Center        
*/

#include "qhull_a.h"

#if defined(macintosh)
	namespace QHULL
	{
#endif

/*========= -prototypes for internal functions ========= */

static int qh_compare_facetarea(const void *p1, const void *p2);
static int qh_compare_facetmerge(const void *p1, const void *p2);
static int qh_compare_facetvisit(const void *p1, const void *p2);
int qh_compare_vertexpoint(const void *p1, const void *p2); /* not used */

/*========= -functions in alphabetical order after qh_produce_output()  =====*/


/*-<a                             href="qh-io.htm#TOC"
  >-------------------------------</a><a name="compare_vertexpoint">-</a>
  
  qh_compare_vertexpoint( p1, p2 )
    used by qsort() to order vertices by point id 
*/
int qh_compare_vertexpoint(const void *p1, const void *p2) {
  vertexT *a= *((vertexT **)p1), *b= *((vertexT **)p2);
 
  return ((qh_pointid(a->point) > qh_pointid(b->point)?1:-1));
} /* compare_vertexpoint */



/*-<a                             href="qh-io.htm#TOC"
  >-------------------------------</a><a name="facetvertices">-</a>
  
  qh_facetvertices( facetlist, facets, allfacets )
    returns temporary set of vertices in a set and/or list of facets
    if allfacets, ignores qh_skipfacet()

  returns:
    vertices with qh.vertex_visit
    
  notes:
    optimized for allfacets of facet_list

  design:
    if allfacets of facet_list
      create vertex set from vertex_list
    else
      for each selected facet in facets or facetlist
        append unvisited vertices to vertex set
*/
setT *qh_facetvertices (facetT *facetlist, setT *facets, boolT allfacets) {
  setT *vertices;
  facetT *facet, **facetp;
  vertexT *vertex, **vertexp;

  qh vertex_visit++;
  if (facetlist == qh facet_list && allfacets && !facets) {
    vertices= qh_settemp (qh num_vertices);
    FORALLvertices {
      vertex->visitid= qh vertex_visit; 
      qh_setappend (&vertices, vertex);
    }
  }else {
    vertices= qh_settemp (qh TEMPsize);
    FORALLfacet_(facetlist) {
      if (!allfacets && qh_skipfacet (facet))
        continue;
      FOREACHvertex_(facet->vertices) {
        if (vertex->visitid != qh vertex_visit) {
          vertex->visitid= qh vertex_visit;
          qh_setappend (&vertices, vertex);
        }
      }
    }
  }
  FOREACHfacet_(facets) {
    if (!allfacets && qh_skipfacet (facet))
      continue;
    FOREACHvertex_(facet->vertices) {
      if (vertex->visitid != qh vertex_visit) {
        vertex->visitid= qh vertex_visit;
        qh_setappend (&vertices, vertex);
      }
    }
  }
  return vertices;
} /* facetvertices */





/*-<a                             href="qh-io.htm#TOC"
  >-------------------------------</a><a name="countfacets">-</a>
  
  qh_countfacets( facetlist, facets, printall, 
          numfacets, numsimplicial, totneighbors, numridges, numcoplanar, numtricoplanars  )
    count good facets for printing and set visitid
    if allfacets, ignores qh_skipfacet()

  notes:
    qh_printsummary and qh_countfacets must match counts

  returns:
    numfacets, numsimplicial, total neighbors, numridges, coplanars
    each facet with ->visitid indicating 1-relative position
      ->visitid==0 indicates not good
  
  notes
    numfacets >= numsimplicial
    if qh.NEWfacets, 
      does not count visible facets (matches qh_printafacet)

  design:
    for all facets on facetlist and in facets set
      unless facet is skipped or visible (i.e., will be deleted)
        mark facet->visitid
        update counts
*/
void qh_countfacets (facetT *facetlist, setT *facets, boolT printall,
    int *numfacetsp, int *numsimplicialp, int *totneighborsp, int *numridgesp, int *numcoplanarsp, int *numtricoplanarsp) {
  facetT *facet, **facetp;
  int numfacets= 0, numsimplicial= 0, numridges= 0, totneighbors= 0, numcoplanars= 0, numtricoplanars= 0;

  FORALLfacet_(facetlist) {
    if ((facet->visible && qh NEWfacets)
    || (!printall && qh_skipfacet(facet)))
      facet->visitid= 0;
    else {
      facet->visitid= ++numfacets;
      totneighbors += qh_setsize (facet->neighbors);
      if (facet->simplicial) {
        numsimplicial++;
	if (facet->keepcentrum && facet->tricoplanar)
	  numtricoplanars++;
      }else
        numridges += qh_setsize (facet->ridges);
      if (facet->coplanarset)
        numcoplanars += qh_setsize (facet->coplanarset);
    }
  }
  FOREACHfacet_(facets) {
    if ((facet->visible && qh NEWfacets)
    || (!printall && qh_skipfacet(facet)))
      facet->visitid= 0;
    else {
      facet->visitid= ++numfacets;
      totneighbors += qh_setsize (facet->neighbors);
      if (facet->simplicial){
        numsimplicial++;
	if (facet->keepcentrum && facet->tricoplanar)
	  numtricoplanars++;
      }else
        numridges += qh_setsize (facet->ridges);
      if (facet->coplanarset)
        numcoplanars += qh_setsize (facet->coplanarset);
    }
  }
  qh visit_id += numfacets+1;
  *numfacetsp= numfacets;
  *numsimplicialp= numsimplicial;
  *totneighborsp= totneighbors;
  *numridgesp= numridges;
  *numcoplanarsp= numcoplanars;
  *numtricoplanarsp= numtricoplanars;
} /* countfacets */



void qh_printfacet(FILE *fp, facetT *facet) {}
void qh_printfacetheader(FILE *fp, facetT *facet) {}
void qh_printfacets(FILE *fp, int format, facetT *facetlist, setT *facets, boolT printall) {}
void qh_printhelp_singular(FILE *fp) {}
void qh_printpointid(FILE *fp, char *string, int dim, pointT *point, int id) {}
void qh_printvertex(FILE *fp, vertexT *vertex) {}
void qh_printvertexlist (FILE *fp, char* string, facetT *facetlist, 
                         setT *facets, boolT printall) {}
void qh_printvertices(FILE *fp, char* string, setT *vertices) {}

/*-<a                             href="qh-io.htm#TOC"
  >-------------------------------</a><a name="setfeasible">-</a>
  
  qh_setfeasible( dim )
    set qh.FEASIBLEpoint from qh.feasible_string in "n,n,n" or "n n n" format

  notes:
    "n,n,n" already checked by qh_initflags()
    see qh_readfeasible()
*/
void qh_setfeasible (int dim) {
  int tokcount= 0;
  char *s;
  coordT *coords, value;

  if (!(s= qh feasible_string)) {
    fprintf(qh ferr, "\
qhull input error: halfspace intersection needs a feasible point.\n\
Either prepend the input with 1 point or use 'Hn,n,n'.  See manual.\n");
    qh_errexit (qh_ERRinput, NULL, NULL);
  }
  if (!(qh feasible_point= (pointT*)malloc (dim* sizeof(coordT)))) {
    fprintf(qh ferr, "qhull error: insufficient memory for 'Hn,n,n'\n");
    qh_errexit(qh_ERRmem, NULL, NULL);
  }
  coords= qh feasible_point;
  while (*s) {
    value= qh_strtod (s, &s);
    if (++tokcount > dim) {
      fprintf (qh ferr, "qhull input warning: more coordinates for 'H%s' than dimension %d\n",
          qh feasible_string, dim);
      break;
    }
    *(coords++)= value;
    if (*s)
      s++;
  }
  while (++tokcount <= dim)    
    *(coords++)= 0.0;
} /* setfeasible */

/*-<a                             href="qh-io.htm#TOC"
  >-------------------------------</a><a name="skipfacet">-</a>
  
  qh_skipfacet( facet )
    returns 'True' if this facet is not to be printed 

  notes:
    based on the user provided slice thresholds and 'good' specifications
*/
boolT qh_skipfacet(facetT *facet) {
  facetT *neighbor, **neighborp;

  if (qh PRINTneighbors) {
    if (facet->good)
      return !qh PRINTgood;
    FOREACHneighbor_(facet) {
      if (neighbor->good)
	return False;
    }
    return True;
  }else if (qh PRINTgood)
    return !facet->good;
  else if (!facet->normal)
    return True;
  return (!qh_inthresholds (facet->normal, NULL));
} /* skipfacet */


// empty Funcs
void    dfacet( unsigned id) {};
void    dvertex( unsigned id) {};
pointT *qh_detvnorm (vertexT *vertex, vertexT *vertexA, setT *centers, realT *offsetp) {return NULL;};
setT   *qh_detvridge (vertexT *vertex) {return NULL;};
setT   *qh_detvridge3 (vertexT *atvertex, vertexT *vertex) {return NULL;};
int     qh_eachvoronoi (FILE *fp, printvridgeT printvridge, vertexT *atvertex, boolT visitall, qh_RIDGE innerouter, boolT inorder) {return 0;};
int     qh_eachvoronoi_all (FILE *fp, printvridgeT printvridge, boolT isupper, qh_RIDGE innerouter, boolT inorder) {return 0;};
void	qh_facet2point(facetT *facet, pointT **point0, pointT **point1, realT *mindist) {};
void    qh_geomplanes (facetT *facet, realT *outerplane, realT *innerplane) {};
void    qh_markkeep (facetT *facetlist) {};
setT   *qh_markvoronoi (facetT *facetlist, setT *facets, boolT printall, boolT *islowerp, int *numcentersp) {return NULL;};
void    qh_order_vertexneighbors(vertexT *vertex) {};
void	qh_printafacet(FILE *fp, int format, facetT *facet, boolT printall) {};
void    qh_printbegin (FILE *fp, int format, facetT *facetlist, setT *facets, boolT printall) {};
void 	qh_printcenter (FILE *fp, int format, char *string, facetT *facet) {};
void    qh_printcentrum (FILE *fp, facetT *facet, realT radius) {};
void    qh_printend (FILE *fp, int format, facetT *facetlist, setT *facets, boolT printall) {};
void    qh_printend4geom (FILE *fp, facetT *facet, int *num, boolT printall) {};
void    qh_printextremes (FILE *fp, facetT *facetlist, setT *facets, int printall) {};
void    qh_printextremes_2d (FILE *fp, facetT *facetlist, setT *facets, int printall) {};
void    qh_printextremes_d (FILE *fp, facetT *facetlist, setT *facets, int printall) {};
void	qh_printfacet2math(FILE *fp, facetT *facet, int notfirst) {};
void	qh_printfacet2geom(FILE *fp, facetT *facet, realT color[3]) {};
void    qh_printfacet2geom_points(FILE *fp, pointT *point1, pointT *point2,
								  facetT *facet, realT offset, realT color[3]) {};
void	qh_printfacet3math (FILE *fp, facetT *facet, int notfirst) {};
void	qh_printfacet3geom_nonsimplicial(FILE *fp, facetT *facet, realT color[3]) {};
void	qh_printfacet3geom_points(FILE *fp, setT *points, facetT *facet, realT offset, realT color[3]) {};
void	qh_printfacet3geom_simplicial(FILE *fp, facetT *facet, realT color[3]) {};
void	qh_printfacet3vertex(FILE *fp, facetT *facet, int format) {};
void	qh_printfacet4geom_nonsimplicial(FILE *fp, facetT *facet, realT color[3]) {};
void	qh_printfacet4geom_simplicial(FILE *fp, facetT *facet, realT color[3]) {};
void	qh_printfacetNvertex_nonsimplicial(FILE *fp, facetT *facet, int id, int format) {};
void	qh_printfacetNvertex_simplicial(FILE *fp, facetT *facet, int format) {};
void    qh_printfacetridges(FILE *fp, facetT *facet) {};
void	qh_printhelp_degenerate(FILE *fp) {};
void	qh_printhyperplaneintersection(FILE *fp, facetT *facet1, facetT *facet2,
									   setT *vertices, realT color[3]) {};
void	qh_printneighborhood (FILE *fp, int format, facetT *facetA, facetT *facetB, boolT printall) {};
void    qh_printline3geom (FILE *fp, pointT *pointA, pointT *pointB, realT color[3]) {};
void	qh_printpoint(FILE *fp, char *string, pointT *point) {};
void    qh_printpoint3 (FILE *fp, pointT *point) {};
void    qh_printpoints_out (FILE *fp, facetT *facetlist, setT *facets, int printall) {};
void    qh_printpointvect (FILE *fp, pointT *point, coordT *normal, pointT *center, realT radius, realT color[3]) {};
void    qh_printpointvect2 (FILE *fp, pointT *point, coordT *normal, pointT *center, realT radius) {};
void	qh_printridge(FILE *fp, ridgeT *ridge) {};
void    qh_printspheres(FILE *fp, setT *vertices, realT radius) {};
void    qh_printvdiagram (FILE *fp, int format, facetT *facetlist, setT *facets, boolT printall) {};
int     qh_printvdiagram2 (FILE *fp, printvridgeT printvridge, setT *vertices, qh_RIDGE innerouter, boolT inorder) {return 0;};
void    qh_printvneighbors (FILE *fp, facetT* facetlist, setT *facets, boolT printall) {};
void    qh_printvoronoi (FILE *fp, int format, facetT *facetlist, setT *facets, boolT printall) {};
void    qh_printvnorm (FILE *fp, vertexT *vertex, vertexT *vertexA, setT *centers, boolT unbounded) {};
void    qh_printvridge (FILE *fp, vertexT *vertex, vertexT *vertexA, setT *centers, boolT unbounded) {};
void	qh_produce_output(void) {};
void    qh_projectdim3 (pointT *source, pointT *destination) {};
int     qh_readfeasible (int dim, char *remainder) {return 0;};
coordT *qh_readpoints(int *numpoints, int *dimension, boolT *ismalloc) {return NULL;};

#if defined(macintosh)
	} // end namespace QHULL
#endif
