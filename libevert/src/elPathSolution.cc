/*************************************************************************
 *
 * This file is part of the EVERT Library / EVERTims program for room 
 * acoustics simulation.
 *
 * This program is free software; you can redistribute it and/or modify it under 
 * the terms of the GNU General Public License as published by the Free Software 
 * Foundation; either version 2 of the License, or any later version.
 *
 * THIS PROGRAM IS DISTRIBUTED IN THE HOPE THAT IT WILL BE USEFUL; BUT WITHOUT 
 * ANY WARRANTY; WITHIOUT EVEN THE IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS 
 * FOR A PARTICULAR PURPOSE. 
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, 
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
 * DEALINGS IN THE SOFTWARE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with 
 * this program; if not, see https://www.gnu.org/licenses/gpl-2.0.html or write 
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
 * MA 02110-1301, USA.
 *
 * Copyright
 *
 * (C) 2004-2005 Samuli Laine
 * Helsinki University of Technology
 *
 * (C) 2008-2017 Markus Noisternig
 * IRCAM-CNRS-UPMC UMR9912 STMS
 *
 ************************************************************************/
 
 
#include "elPathSolution.h"
#include "elBeam.h"
#include "elBSP.h"
#include "elListener.h"
#include "elRay.h"
#include "elRoom.h"
#include "elSource.h"

#ifdef __Darwin
    #include <OpenGL/gl.h>
#else
    #include <GL/gl.h>
#endif

#include <cstdio>
#define printf // Comment to add debug logs

using namespace EL;

//------------------------------------------------------------------------

// static const float EPS_SIMILAR_PATHS			= 0.00001f;
// static const float EPS_DEGENERATE_POLYGON_AREA	= 0.0001f;

static const float EPS_SIMILAR_PATHS = 1.0e-5;
static const float EPS_DEGENERATE_POLYGON_AREA = 1.0e-8;

//static const float EPS_SIMILAR_PATHS			= 0.00000000001f;
//static const float EPS_DEGENERATE_POLYGON_AREA	= 0.000000000001f;

static const int DISTANCE_SKIP_BUCKET_SIZE = 16;

//------------------------------------------------------------------------

struct PathSolution::SolutionNode
{
public:
    int m_parent;
    const Polygon* m_polygon;
};

//------------------------------------------------------------------------
void PathSolution::renderPath(const Path& path) const
{
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    
    float width = 2.f*(m_maximumOrder-path.m_order+1.f);
    float alpha = ((float)m_maximumOrder-path.m_order+4)/(m_maximumOrder+4);
    
    //	std::cerr << "Drawing path: width = " << width << ", alpha = " << alpha << std::endl;
    
    glLineWidth(width);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(1.f, 1.f, 0.f, alpha);
    
    glBegin(GL_LINE_STRIP);
    for( int i=0; i < (int)path.m_points.size(); i++ )
    {
        glVertex3fv(&path.m_points[i].x);
    }
    glEnd();
    
    glPointSize(width*2.f);
    glColor4f(1.f, 1.f, 1.f, alpha*2);
    glEnable(GL_POINT_SMOOTH);
    glDepthMask(GL_FALSE);
    glBegin(GL_POINTS);
    for( int i=0; i < (int)path.m_points.size(); i++ )
    {
        glVertex3fv(&path.m_points[i].x);
    }
    glEnd();
    
    glPopAttrib();
}
//------------------------------------------------------------------------

PathSolution::PathSolution(const Room& room, const Source& source, const Listener& listener, int maximumOrder, bool changed):
m_room (room),
m_source (source),
m_listener (listener),
m_maximumOrder (maximumOrder),
m_changed (changed)
{
    m_polygonCache.resize(maximumOrder);
    m_validateCache.resize(maximumOrder*2);
}

PathSolution::~PathSolution(void) {}

//------------------------------------------------------------------------

void PathSolution::clearCache(void)
{
    m_solutionNodes.clear();
    m_failPlanes.clear();
}

//------------------------------------------------------------------------

void PathSolution::solve(void)
{
    Vector3 source = m_source.getPosition();
    Vector3 target = m_listener.getPosition();
    
    clearCache();
    
    // Create an empty root node
    SolutionNode root;
    root.m_polygon = 0;
    root.m_parent  = -1;
    m_solutionNodes.push_back(root);
    
    // Do the recursive solving from scratch
    solveRecursive(source, target, Beam(), 0, 0);
    
    if( stop_signal )
    {
        printf ("Killed solution calculation\n");
        return;
    }
    
    //printf ("Calculated full solution\n");
    
    // Record the current source
    m_cachedSource = source;
    EL_ASSERT(m_solutionNodes.size() == m_failPlanes.size());
    
    // Initialize the buckets
    int numBuckets = (m_solutionNodes.size() + DISTANCE_SKIP_BUCKET_SIZE - 1) / DISTANCE_SKIP_BUCKET_SIZE;
    m_distanceSkipCache.resize(numBuckets);
    for( int i=0; i < numBuckets; i++ )
    {
        m_distanceSkipCache[i].set(0,0,0,0);
    }
}

void PathSolution::update(void)
{
    //printf ("Solution Update\n");
    
    int numProc   = 0;
    int numTested = 0;
    
    Vector3 source = m_source.getPosition();
    Vector3 target = m_listener.getPosition();
    
    // Clear all paths
    m_paths.clear();
    
    // If we do not have any previous solution or the source has moved
    if( !m_solutionNodes.size() || m_cachedSource != source )
    {
        if( m_cachedSource != source )
        {
            printf ("Source changed! You should solve() instead of update()\n");
        }
        if( ! m_solutionNodes.size () )
        {
            printf ("No solution! You should solve() instead of update()\n");
        }
        return;
    }
    
    // Number of solution nodes
    int n = m_solutionNodes.size();
    
    // Number of buckets
    int nb = (m_solutionNodes.size() + DISTANCE_SKIP_BUCKET_SIZE - 1) / DISTANCE_SKIP_BUCKET_SIZE;
    
    // Take the first bucket skip sphere
    const Vector4* skipSphere = &m_distanceSkipCache[0];
    
    // Do for each bucket
    for( int b=0; b < nb; b++ )
    {
        // Test if the listener is inside the skip sphere
        // skip the rest of the tests if it is
        const Vector4& fc = *skipSphere++;
        Vector3 r = target - Vector3(fc.x, fc.y, fc.z);
        if( r.lengthSqr() < fc.w ){ continue; }
        
        // Initialize the skip sphere radius to zero
        float maxdot = 0.f;
        numProc++;
        
        // Go through the nodes in the bucket
        int imn = b * DISTANCE_SKIP_BUCKET_SIZE;
        int imx = imn + DISTANCE_SKIP_BUCKET_SIZE;
        if( imx > n ){ imx = n; }
        for( int i=imn; i < imx; i++ )
        {
            // Calculate the distance from the listener to the fail plane
            float d = dot(target, m_failPlanes[i]);
            
            // If the distance is positive or zero, the path is inside the
            // beam and must be validated for occlusion
            if( d >= 0.f )
            {
                validatePath(source, target, i, m_failPlanes[i]);
                numTested++;
            }
            
            // Record the maximum distance
            if( i == imn || d > maxdot ){ maxdot = d; }
        }
        
        // If all paths were on the wrong side of the fail planes, the skip sphere
        // can be set to be the distance to the nearest fail plane
        // Note: max (-x) = - min (x)
        if( maxdot < 0.f )
        {
            m_distanceSkipCache[b].set(target.x, target.y, target.z, maxdot*maxdot);
        }
    }
    
    m_pathFirstSet.clear();
    
    /*
     printf("paths: %d (proc %d = %.2f %%, tested %d, valid %d)\n",
     m_solutionNodes.size(), numProc * DISTANCE_SKIP_BUCKET_SIZE,
     (float)numProc/nb*100.f, numTested, m_paths.size());
     */
}

Vector4 PathSolution::getFailPlane(const Beam& beam, const Vector3& target)
{
    // Go through all the planes defining the beam
    // Find the plane whose distance to the listaner is smallest
    // The beams are convex so this gives the correct plane
    Vector4 failPlane(0.f, 0.f, 0.f, 1.f);
    
    if( beam.numPleqs() > 0 ){ failPlane = beam.getPleq(0); }
    
    for( int i=1; i < beam.numPleqs(); i++ )
    {
        if( dot(target, beam.getPleq(i)) < dot(target, failPlane) )
        {
            failPlane = beam.getPleq(i);
        }
    }
    
    return failPlane;
}

void PathSolution::validatePath(const Vector3& source,
                                const Vector3& target,
                                int nodeIndex,
                                Vector4& failPlane)
{
    // Collect polygons in the path from this node to the root
    int order = 0;
    while( nodeIndex )
    {
        m_polygonCache[order++] = m_solutionNodes[nodeIndex].m_polygon;
        nodeIndex = m_solutionNodes[nodeIndex].m_parent;
    }
    
    // Reconstruct image source for this level
    Vector3 imgSource = source;
    for( int i=order-1; i >= 0; i-- )
    {
        imgSource = mirror(imgSource, m_polygonCache[i]->getPleq());
    }
    
    // Test for polygon miss and failed reflection
    // Record miss type and order
    Vector3 s = imgSource;
    Vector3 t = target;
    
    bool missed = false;
    int missOrder = -1;
    const Polygon* missPoly = 0;
    Ray missRay(Vector3(0,0,0), Vector3(0,0,0));
    bool missSide = false;
    
    for( int i=0; i < order; i++ )
    {
        const Polygon* poly = m_polygonCache[i];
        const Vector4& pleq = poly->getPleq();
        Ray ray(s, t);
        
        // On the same side of the polygon?
        if( dot(s, pleq) * dot(t, pleq) > 0.f )
        {
            missed	  = true;
            missSide  = true;
            missOrder = i;
            missPoly  = poly;
            missRay	  = ray;
            break;
        }
        
        // Not intersecting the polygon?
        if( !ray.intersectExt(*poly) )
        {
            missed	  = true;
            missSide  = false;
            missOrder = i;
            missPoly  = poly;
            missRay   = ray;
            break;
        }
        
        // Next image source
        Vector3 isect = intersect(ray, pleq);
        s = mirror(s, pleq);
        t = isect;
        
        // Record intersection points and images sources
        m_validateCache[i*2] = isect;
        m_validateCache[i*2+1] = s;
    }
    
    // Path missed a polygon?
    if( missed )
    {
        Vector4 missPlane(0.f, 0.f, 0.f, 0.f);
        if( missSide )
        {
            // The target and the image source were on the same side of
            // the polygon so reconstruct the fail plane from its plane
            // equation; make sure it is facing away
            missPlane = missPoly->getPleq();
            if( dot(missRay.m_a, missPlane) > 0.f ){ missPlane = -missPlane; }
        }
        else
        {
            // The ray from the target to the image source did not hit the
            // polygon so reconstruct the fail plane from the missed polygon
            // edge and the image source; the nearest plane to the target is
            // the correct fail plane
            Beam beam(missRay.m_a, *missPoly);
            missPlane = beam.getPleq(1);
            for (int i=2; i < beam.numPleqs(); i++)
            {
                if( dot(missRay.m_b, beam.getPleq(i)) < dot(missRay.m_b, missPlane) )
                {
                    missPlane = beam.getPleq(i);
                }
            }
        }
        
        // Mirror the fail plane according to the remaining polygons in the path
        for( int i=missOrder-1; i >= 0; i-- )
        {
            missPlane = mirror(missPlane, m_polygonCache[i]->getPleq());
        }
        
        // Because of numerical inaccuracies, we may end up on wrong side
        if( dot(target, missPlane) > 0.f )
        {
            // Reconstruct beam for robust fail plane construction
            Beam beam;
            imgSource = source;
            for( int i=order-1; i >= 0; i-- )
            {
                Polygon poly = *m_polygonCache[i];
                poly.clip(beam);
                
                imgSource = mirror(imgSource, poly.getPleq());
                beam = Beam(imgSource, poly);
            }
            
            // Update the fail plane
            missPlane = getFailPlane(beam, target);
        }
        
        // Done, normalize to be sure
        failPlane = normalize(missPlane);
        return;
    }
    
    // Test for occlusion
    // Go through the path segments with a fast ray tracer
    t = target;
    for( int i=0; i < order; i++ )
    {
        Vector3 isect = m_validateCache[i*2];
        if( m_room.getBSP().rayCastAny(Ray(isect, t)) ){ return; }
        t = isect;
    }
    if( m_room.getBSP().rayCastAny(Ray(source, t)) ){ return; }
    
    // Validated, add to results
    Path path;
    path.m_order = order;
    path.m_points.resize(order+2);
    path.m_polygons.resize(order);
    
    t = target;
    for( int i=0; i < order; i++ )
    {
        path.m_points[order-i+1] = t;
        path.m_polygons[order-i-1] = m_polygonCache[i];
        
        t = m_validateCache[i*2];
    }
    
    path.m_points[0] = source;
    path.m_points[1] = t;
    
    // Finally remove similar paths to dodge bad geometry
    
    float fval = dot(path.m_points[1], Vector3(1.f, 1.f, 1.f));
    float fmin = fval - 2.f*EPS_SIMILAR_PATHS;
    float fmax = fval + 2.f*EPS_SIMILAR_PATHS;
    
    for( std::multimap<float, int>::const_iterator it = m_pathFirstSet.upper_bound(fmin); it != m_pathFirstSet.end() && it->first <= fmax; ++it )
    {
        int i = it->second;
        
        const Path& p = m_paths[i];
        if (p.m_order != order){ continue; }
        
        bool safe = false;
        for( int j=1; j < (int)p.m_points.size()-1; j++ )
        {
            if( (p.m_points[j] - path.m_points[j]).lengthSqr() > EPS_SIMILAR_PATHS * EPS_SIMILAR_PATHS )
            {
                safe = true;
                break;
            }
        }
        if( !safe ){ return; }
    }
    
    m_pathFirstSet.insert(std::pair<float,int>(fval, m_paths.size()));
    m_paths.push_back(path);
}

void PathSolution::solveRecursive(const Vector3& source,
                                  const Vector3& target,
                                  const Beam& beam,
                                  int order,
                                  int parentIndex)
{
    /*
     if (order == 0)
     {
     printf ("solveRecursive () IN\n");
     }
     */
    // Forced stop
    if( stop_signal )
    {
        printf ("Stop signal catch 1\n");
        return;
    }
    
    // Start with the optimal fail plane
    m_failPlanes.push_back(getFailPlane(beam, target));
    
    // Recursion max depth reached?
    if( order >= m_maximumOrder ){ return; }
    
    // Find the polygons intersecting the beam
    std::vector<const Polygon*> polygons;
    m_room.getBSP().beamCast(beam, polygons);
    
    // For each polygon in the beam
    for( int i=(int)polygons.size()-1; i >= 0; i-- )
    {
        if( stop_signal )
        {
            printf ("Stop signal catch 2\n");
            break;
        }
        
        const Polygon* orig = polygons[i];
        // Construct image source
        Vector3 imgSource = mirror(source, orig->getPleq());
        
        // Not root?
        if( parentIndex > 0 )
        {
            // Test for cases where the parent polygon is the same as the
            // current polygon or the image sources match
            const Polygon* ppoly = m_solutionNodes[parentIndex].m_polygon;
            if( orig == ppoly ){ continue; }
            
            Vector3 testSource = mirror(imgSource, ppoly->getPleq());
            if( (source-testSource).length() < EPS_SIMILAR_PATHS ){ continue; }
        }
        
        // Do not count polygons with vanishingly small intersections
        Polygon poly = *orig;
        if( poly.clip(beam) == Polygon::CLIP_VANISHED ){ continue; }
        
        // Do not count degenerated polygons
        if( poly.getArea() < EPS_DEGENERATE_POLYGON_AREA ){ continue; }
        
        // Create a new beam from the images source and the polygon
        Beam b(imgSource, poly);
        
        // Create a new solution node
        SolutionNode node;
        node.m_polygon = orig;
        node.m_parent  = parentIndex;
        m_solutionNodes.push_back(node);
        
        // Solve recursively the child beam
        solveRecursive(imgSource, target, b, order+1, m_solutionNodes.size()-1);
        
        /*
         if (order==0) {
         printf("building beam tree.. %.2f %% (%.2f Mb)\r",
         100.f-(float)i/(float)polygons.size()*100.f,
         m_solutionNodes.size() * sizeof(SolutionNode) /
         1048576.0);
         }
         */
    }
    /*
     if (order==0)
     printf("\n");
     */
    /*
     if (order == 0)
     {
     printf ("solveRecursive () OUT\n");
     }
     */
}


float PathSolution::getLength(const Path& path)
{
    float len = 0;
    
    for( int j=0; j < path.m_points.size()-1; j++ )
    {
        const EL::Vector3& p0 = path.m_points[j];
        const EL::Vector3& p1 = path.m_points[j+1];
        
        len += (p1-p0).length();
    }
    return len;
}


void PathSolution::print (int minOrder, int maxOrder, int maxAmount)
{
    int i, j, k;
    float reflectance[10];
    
    int pathCount = 0;
    
    for( j = 0; j < numPaths (); j++ )
    {
        Path path = getPath (j);
        if( pathCount >= maxAmount ){ break; }
        if( (path.m_order < minOrder) || (path.m_order > maxOrder) ){ continue; }
        pathCount++;
        
        printf ("%ld (", path.m_points.size () - 2 );
        
        for( k = 0; k < 10; k++ ){ reflectance[k] = 1.0; }
        for( i = 0; i < path.m_points.size () - 1; i++ )
        {
            printf ("[%.5f %.5f %.5f]-", path.m_points[i].x, path.m_points[i].y, path.m_points[i].z);
            
            if( i > 0 )
            {
                const Polygon* p = path.m_polygons[i-1];
                const Material& m = p->getMaterial ();
                for( k = 0; k < 10; k++ ){ reflectance[k] *= (1 - m.absorption[k]); }
            }
        }
        printf ("[%.2f %.2f %.2f])",
                path.m_points[i].x,
                path.m_points[i].y,
                path.m_points[i].z);
        
        /*
         if ( i > 1 )
         {
         printf ("[");
         for (k = 0; k < 9 ;k++)
         printf("%.2f,", 1-reflectance[k]);
         printf ("%.2f]", 1-reflectance[9]);
         }
         */
        printf ("\n");
    }
}
