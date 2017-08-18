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
 
 
#include <string.h>
#include "elBSP.h"
#include "elBeam.h"
#include "elPolygon.h"
#include "elRay.h"

using namespace EL;

//------------------------------------------------------------------------

static const int g_maxPolygonsInLeaf = 4;

/*
static const float EPS_RAY_ENDS			= 1.f;
static const float EPS_BOUNDING_BOX		= 1.f;
static const float EPS_POLY_BOX_OVERLAP = 1.f;
static const float EPS_ISECT_POLYGON	= 1e-5f;
static const float EPS_DISTANCE			= 1e-5f;
*/

static const float EPS_RAY_ENDS = 1.0e-3f;
static const float EPS_BOUNDING_BOX = 1.0e-3f;
static const float EPS_POLY_BOX_OVERLAP = 1.0e-3f;
static const float EPS_ISECT_POLYGON = 1.0e-8f;
static const float EPS_DISTANCE = 1.0e-8f;

//------------------------------------------------------------------------

class BSP::TempNode
{
    
public:
    
    TempNode (void);
    ~TempNode (void);
    
    TempNode* m_children[2];
    int m_splitAxis;
    float m_splitPos;
    const Polygon** m_polygons;
    int m_numPolygons;
};

//------------------------------------------------------------------------

BSP::TempNode::TempNode(void):
m_splitAxis		(-1),
m_splitPos		(0.f),
m_polygons		(0),
m_numPolygons	(0)
{
    m_children[0] = m_children[1] = 0;
}

BSP::TempNode::~TempNode(void)
{
    delete	 m_children[0];
    delete	 m_children[1];
    delete[] m_polygons;
}

//------------------------------------------------------------------------
// Split plane finder
//------------------------------------------------------------------------

namespace {
    struct SortItem
    {
        float v;
        union
        {
            const Polygon* polygon;
            unsigned int iptr;
        };
    };
    bool operator<(const SortItem& a, const SortItem& b)
    {
        if( a.v < b.v ){ return true; }
        return (a.v==b.v && a.iptr<b.iptr);
    }
    
    SortItem* g_items;
    int g_totalPolys = 0;
    int g_maxDepth = 0;
    int g_numNodes = 0;
    int g_listSize = 0;
}

EL_FORCE_INLINE static void insertionSort(SortItem* items, int N)
{
    for( int i=0; i < N-1; i++ )
    {
        int k = -1;
        SortItem v0 = items[i];
        
        for( int j = i+1; j < N; j++ )
        {
            if( items[j] < v0 )
            {
                v0 = items[j];
                k  = j;
            }
        }
        
        if( k >= 0 ){ swap(items[i], items[k]); }
    }
}

EL_FORCE_INLINE static int median3(int low, int high)
{
    int l = low;
    int c = (unsigned int)(high + low)>>1;
    int h = high-2;
    
    SortItem lv = g_items[l];
    SortItem cv = g_items[c];
    SortItem hv = g_items[h];
    
    if( hv < lv ) { swap(l, h); swap(lv, hv); }
    if( cv < lv ) { swap(l, c); swap(lv, cv); }
    if( hv < cv ) { swap(c, h); swap(cv, hv); }
    
    return c;
}

static void quickSort(int low, int high)
{
    static const int SWITCHPOINT = 15;
    
    if( (high - low) <= SWITCHPOINT )
    {
        insertionSort( &g_items[low], high - low );
        return;
    }
    
    // Select pivot using median-3
    int	pivotIndex	= median3(low, high);
    swap( g_items[high-1], g_items[pivotIndex] );
    
    SortItem pivot = g_items[high-1];
    
    // Partition data
    int i = low  - 1;
    int j = high - 1;
    while( i < j )
    {
        do { i++; } while(g_items[i] < pivot);
        do { j--; } while(pivot < g_items[j]);
        swap(g_items[i], g_items[j]);
    }
    
    // Restore pivot
    SortItem tmp(g_items[j]);
    g_items[j] = g_items[i];
    g_items[i] = g_items[high-1];
    g_items[high-1] = tmp;
    
    // sort sub-partitions
    if( (i - low) > 1 ){ quickSort(low, i); }
    if( (high - (i+1)) > 1 ){ quickSort(i+1, high); }
}

static int getOptimalSplitPlane(const Polygon** polygons, int numPolygons, float& bestSplitPos, const AABB& aabb)
{
    int bestSplitAxis = -1;
    float bestCost = 0.f;
    
    for( int axis=0; axis < 3; axis++ )
    {
        static const int nextAxis[] = { 1, 2, 0, 1 };
        
        // construct item array
        SortItem* itemPtr = g_items;
        for( int i=0; i < numPolygons; i++ )
        {
            const Polygon* poly = polygons[i];
            float mn = (*poly)[0][axis];
            float mx = mn;
            for( int j=1; j < poly->numPoints(); j++ )
            {
                mn = min2(mn, (*poly)[j][axis]);
                mx = max2(mx, (*poly)[j][axis]);
            }
            
            itemPtr[0].v = mn;
            itemPtr[0].polygon = polygons[i];
            itemPtr[1].v = mx;
            itemPtr[1].iptr = itemPtr[0].iptr+1;
            itemPtr += 2;
        }
        
        // sort the array
        quickSort(0, numPolygons*2);
        
        // area terms
        int c1 = nextAxis[axis];
        int c2 = nextAxis[axis+1];
        float areaConst  = 2.f*(aabb.m_mx[c1]-aabb.m_mn[c1])*(aabb.m_mx[c2]-aabb.m_mn[c2]);
        float areaFactor = 2.f*((aabb.m_mx[c1]-aabb.m_mn[c1])+(aabb.m_mx[c2]-aabb.m_mn[c2]));
        float boundLeft  = aabb.m_mn[axis];
        float boundRight = aabb.m_mx[axis];
        
        // scan for lowest cost
        float bestAxisCost  = 0.f;
        float bestAxisSplit = 0.f;
        itemPtr = g_items;
        int leftPolys  = 0;
        int rightPolys = numPolygons;
        int bothPolys  = 0;
        
        for( int i=0; i < numPolygons*2; i++ )
        {
            const SortItem& it = *itemPtr++;
            
            if( !(it.iptr & 1) )
            {
                leftPolys++;
                bothPolys++;
            }
            
            if( it.v >= boundRight ){ break; }
            
            if( it.v > boundLeft )
            {
                float split   = it.v;
                float aLeft	  = areaConst + areaFactor * (split - boundLeft);
                float aRight  = areaConst + areaFactor * (boundRight - split);
                float cost	  = aLeft*leftPolys + aRight*rightPolys;
                if( cost < bestAxisCost || bestAxisCost == 0.f )
                {
                    bestAxisCost  = cost;
                    bestAxisSplit = split;
                }
            }
            if( it.iptr & 1 )
            {
                rightPolys--;
                bothPolys--;
            }
        }
        
        if( (bestAxisCost < bestCost || bestCost == 0.f) && bestAxisCost > 0.f )
        {
            bestCost	  = bestAxisCost;
            bestSplitPos  = bestAxisSplit;
            bestSplitAxis = axis;
        }
    }
    
    return bestSplitAxis;
}

//------------------------------------------------------------------------
// Globals
//------------------------------------------------------------------------

namespace
{
    struct RecursionEntry
    {
        uintptr_t* ptr;
        float dEnter;
        float dExit;
    };
    RecursionEntry*	g_recursionStack = 0;
    int g_recursionStackSize = 0;
    
    Vector3 g_intersectionPoint;
};

//------------------------------------------------------------------------
// Construction
//------------------------------------------------------------------------

static BSP::TempNode* constructRecursive(const Polygon** polygons, int numPolygons,
                                         const AABB& aabb)
{
    // leaf?
    if (numPolygons <= g_maxPolygonsInLeaf)
    {
        g_totalPolys += numPolygons;
        BSP::TempNode* n = new BSP::TempNode;
        
        n->m_numPolygons = numPolygons;
        if( numPolygons )
        {
            n->m_polygons = new const Polygon*[numPolygons];
            memcpy(n->m_polygons, polygons, numPolygons*sizeof(const Polygon*));
        }
        return n;
    }
    
    // find split plane
    float splitPos;
    int axis = getOptimalSplitPlane(polygons, numPolygons, splitPos, aabb);
    if( axis < 0 )
    {
        g_totalPolys += numPolygons;
        BSP::TempNode* n = new BSP::TempNode;
        n->m_numPolygons = numPolygons;
        n->m_polygons	 = new const Polygon*[numPolygons];
        memcpy(n->m_polygons, polygons, numPolygons*sizeof(const Polygon*));
        return n;
    }
    
    // split
    BSP::TempNode* n = new BSP::TempNode;
    n->m_splitAxis = axis;
    n->m_splitPos = splitPos;
    n->m_numPolygons = numPolygons;
    
    // classify polygons
    for( int c=0; c < 2; c++ )
    {
        AABB aabb2 = aabb;
        
        if( c==0 ){ aabb2.m_mx[axis] = splitPos; }
        else{ aabb2.m_mn[axis] = splitPos; }
        
        AABB aabbTest = aabb2;
        
        aabbTest.m_mn -= EPS_POLY_BOX_OVERLAP * Vector3(1.f, 1.f, 1.f);
        aabbTest.m_mx += EPS_POLY_BOX_OVERLAP * Vector3(1.f, 1.f, 1.f);
        
        int childPolys = 0;
        for( int i=0; i < numPolygons; i++ )
        {
            Polygon poly = *polygons[i];	// copy
            AABB pbox = poly.getAABB();
            
            bool overlap = false;
            if( pbox.m_mn[axis] == splitPos && pbox.m_mx[axis] == splitPos )
            {
                overlap = (c==1); // on split plane, assign to right child
            }
            else
            {
                // determine split plane exactly
                for( int j=0; j < poly.numPoints(); j++ )
                {
                    float x = poly[j][axis];
                    if( c==0 && x < splitPos ){ overlap = true; }
                    if( c==1 && x > splitPos ){ overlap = true; }
                }
            }
            
            if( !overlap ){ continue; }
            
            if( poly.clip(aabbTest) != Polygon::CLIP_VANISHED )
            {
                if( i != childPolys )
                {
                    swap(polygons[i], polygons[childPolys]);
                }
                childPolys++;
            }
        }
        
        n->m_children[c] = constructRecursive(polygons, childPolys, aabb2);
    }
    
    return n;
}

//------------------------------------------------------------------------

static uintptr_t* convertRecursive(BSP::TempNode* node, uintptr_t* list)
{
    if( node->m_splitAxis < 0 )
    {
        // leaf node
        if( !node->m_numPolygons )
        {
            *list++ = 3;
            return list;
        }
        *list++ = (node->m_numPolygons << 2) | 3;
        
        for( int i=0; i < node->m_numPolygons; i++ )
        {
            *list++ = (uintptr_t)node->m_polygons[i];
        }
        return list;
    }
    
    // inside node
    uintptr_t* pRight = convertRecursive(node->m_children[0], list+2);
    list[0] = ((uintptr_t)pRight) + node->m_splitAxis;
    list[1] = *((uintptr_t*)&node->m_splitPos);
    return convertRecursive(node->m_children[1], pRight);
}

//------------------------------------------------------------------------

BSP::BSP(void):
m_hierarchy (0),
m_list (0)
{
    m_aabb.m_mn = m_aabb.m_mx = Vector3(0.f, 0.f, 0.f);
}

BSP::~BSP(void)
{
    delete m_hierarchy;
    delete[] m_list;
}

//------------------------------------------------------------------------

static void convertHierarchy(BSP::TempNode* root, uintptr_t* list)
{
    uintptr_t* end  = convertRecursive(root, list);
    //printf("list size: %d bytes (%.2f Mb)\n", (end-list)*4, (float)(end-list)*4.f/1024.f/1024.f);
}

//------------------------------------------------------------------------

void BSP::constructHierarchy(const Polygon** polygons, int numPolygons)
{
    g_totalPolys = 0;
    EL_ASSERT(!m_hierarchy);
    EL_ASSERT(numPolygons > 0);
    
    // compute bounding box and construct sort item array
    g_items	= new SortItem[2*numPolygons];
    
    for( int i=0; i < 3; i++ )
    {
        m_aabb.m_mn[i] = m_aabb.m_mx[i] = (*polygons[0])[0][i];
    }
    
    for( int i=0; i < numPolygons; i++ )
    {
        const Polygon& poly = *polygons[i];
        for (int j=0; j < poly.numPoints(); j++)
            m_aabb.grow(poly[j]);
    }
    
    // enlarge the bounding box slightly so that polygons at the edge
    // of the scene don't get dropped away
    m_aabb.m_mn -= EPS_BOUNDING_BOX * Vector3(1.f, 1.f, 1.f);
    m_aabb.m_mx += EPS_BOUNDING_BOX * Vector3(1.f, 1.f, 1.f);
    
    // construct hierarchy
    m_hierarchy = constructRecursive(polygons, numPolygons, m_aabb);
    //printf("total polygons: %d\n", g_totalPolys);
    
    // count max depth
    g_numNodes = 0;
    g_listSize = 0;
    struct Hep{static int getDepth(BSP::TempNode* n)
        {
            g_numNodes++;
            if( n->m_splitAxis < 0 )
            {
                g_listSize += n->m_numPolygons+1;
                return 1;
            }
            g_listSize += 2;
            int d0 = getDepth(n->m_children[0]);
            int d1 = getDepth(n->m_children[1]);
            if( d0 > d1 ){ return d0+1; }
            return d1+1;
        }};
    
    g_maxDepth = Hep::getDepth(m_hierarchy);
    //printf("nodes: %d, max depth: %d\n", g_numNodes, g_maxDepth);
    
    // cleanup
    delete[] g_items;
    
    // convert
    m_list = new uintptr_t[g_listSize];
    convertHierarchy(m_hierarchy, m_list);
    
    // delete the temp hierarchy
    delete m_hierarchy;
    m_hierarchy = 0;
    
    // setup recursion stack
    if( g_maxDepth > g_recursionStackSize )
    {
        g_recursionStackSize = g_maxDepth;
        delete[] g_recursionStack;
        g_recursionStack = new RecursionEntry[g_maxDepth];
    }
}

//------------------------------------------------------------------------
// Ray cast helpers
//------------------------------------------------------------------------

namespace
{
	Vector3 g_orig;
	Vector3 g_dest;
	Vector3 g_dir;
	Vector3 g_invdir;
	unsigned int g_dirsgn[3];
	std::set<const Polygon*> g_foundPolygons;

	Vector3 g_beamMid;
	Vector3 g_beamDiag;
	const Beam* g_beamBeam;
	std::vector<const Polygon*>* g_beamResult;
};

EL_FORCE_INLINE static void setupRayCast(const Ray& ray)
{
	Vector3 ndir = EPS_RAY_ENDS * normalize(ray.m_b - ray.m_a);
	g_orig = ray.m_a + ndir;
	g_dest = ray.m_b - ndir;
	g_dir = g_dest - g_orig;

	g_invdir.set(1.f/g_dir.x, 1.f/g_dir.y, 1.f/g_dir.z);
	g_dirsgn[0] = *((unsigned int*)&g_invdir[0]) >> 31;
	g_dirsgn[1] = *((unsigned int*)&g_invdir[1]) >> 31;
	g_dirsgn[2] = *((unsigned int*)&g_invdir[2]) >> 31;
}

EL_FORCE_INLINE static float getSplitDistance(float splitPos, int axis)
{
	return (splitPos-g_orig[axis])*g_invdir[axis];
}

EL_FORCE_INLINE static void getEnterExitDistances(const AABB& aabb, float& dEnter, float& dExit)
{
    float x[2], y[2], z[2];
    
    // enter and exit distances
    x[0] = getSplitDistance(aabb.m_mn[0], 0);
    y[0] = getSplitDistance(aabb.m_mn[1], 1);
    z[0] = getSplitDistance(aabb.m_mn[2], 2);
    x[1] = getSplitDistance(aabb.m_mx[0], 0);
    y[1] = getSplitDistance(aabb.m_mx[1], 1);
    z[1] = getSplitDistance(aabb.m_mx[2], 2);
    
    int sx = g_dirsgn[0];
    int sy = g_dirsgn[1];
    int sz = g_dirsgn[2];
    
    // enter and exit
    float mn0 = x[sx];
    float mx0 = x[sx^1];
    float mn1 = y[sy];
    float mx1 = y[sy^1];
    float mn2 = z[sz];
    float mx2 = z[sz^1];
    
    // get maximum of enter distances and minimum of exit distances
    dEnter = mn0;
    if( mn1 > dEnter ){ dEnter = mn1; }
    if( mn2 > dEnter ){ dEnter = mn2; }
    
    dExit = mx0;
    if( mx1 < dExit ){ dExit = mx1; }
    if( mx2 < dExit ){ dExit = mx2; }
}

//------------------------------------------------------------------------
// Ray casts
//------------------------------------------------------------------------

EL_FORCE_INLINE static bool isectPolygonsAny(const Polygon** list, int numPolygons)
{
    Ray ray(g_orig, g_dest);
    while( numPolygons-- )
    {
        const Polygon* poly = *list++;
        if( ray.intersect(*poly) ){ return true; }
    }
    
    return false;
}

//------------------------------------------------------------------------

EL_FORCE_INLINE static bool rayCastListAny(uintptr_t* listOrig, float dEnterOrig, float dExitOrig)
{
    if( dEnterOrig < 0.f ){ dEnterOrig = 0.f; }
    if( dExitOrig  > 1.f ){ dExitOrig  = 1.f; }
    if( dEnterOrig > dExitOrig + EPS_DISTANCE ){ return 0; }

	RecursionEntry* stack = g_recursionStack;
	stack->ptr = listOrig;
	stack->dEnter = dEnterOrig;
	stack->dExit = dExitOrig;
	stack++;

	while( stack != g_recursionStack )
	{
		--stack;
		uintptr_t* list = stack->ptr;
		float dEnter = stack->dEnter;
		float dExit = stack->dExit;
		uintptr_t pRight = *list++;

label_skipstack:

		// leaf?
		if( (pRight & 3) == 3 )
		{
			int numPolygons = pRight>>2;
			if( isectPolygonsAny((const Polygon**)list, numPolygons) )
            {
				return true;
            }
			continue;
		}

		// recurse
		int a = pRight&3;
		float d = getSplitDistance(*((float*)list), a);

		uintptr_t* ch[2] = { list+1, (uintptr_t*)(pRight-a) };
        if( g_dirsgn[a] ){ swap(ch[1], ch[0]); }

		if( *ch[1] && d <= dExit+EPS_DISTANCE )
		{
			float newEnter = dEnter;
            if( d > newEnter ){ newEnter = d; }
			stack->ptr    = ch[1];
			stack->dEnter = newEnter;
			stack->dExit  = dExit;
			stack++;
		}

		pRight = *ch[0];
		if( pRight && d >= dEnter-EPS_DISTANCE )
		{
            if( d < dExit ){ dExit = d; }
			list = ch[0]+1;
			goto label_skipstack;
		}
	}

	return false;
}

bool BSP::rayCastAny(const Ray& ray) const
{
    setupRayCast(ray);
    float dEnter, dExit;
    getEnterExitDistances(m_aabb, dEnter, dExit);
    bool result = rayCastListAny(m_list, dEnter, dExit);
    
    return result;
}

//------------------------------------------------------------------------

EL_FORCE_INLINE static const Polygon* isectPolygons(const Polygon** list, int numPolygons, float dEnter, float dExit)
{
    const Polygon* res = 0;
    float thigh = dExit + EPS_ISECT_POLYGON;
    float tlow = dEnter - EPS_ISECT_POLYGON;
    Ray ray(g_orig, g_dest);
    
    while( numPolygons-- )
    {
        const Polygon* poly = *list++;
        
        if( ray.intersect(*poly) )
        {
            float t = -dot(g_orig, poly->getPleq()) / dot(g_dir, poly->getNormal());
            if( t < tlow || t > thigh ){ continue; }
            
            thigh = t;
            res   = poly;
            
            g_intersectionPoint = g_orig + t*g_dir;
        }
    }
    
    return res;
}

//------------------------------------------------------------------------

EL_FORCE_INLINE static const Polygon* rayCastList(uintptr_t* listOrig, float dEnterOrig, float dExitOrig)
{
    if( dEnterOrig < 0.f ){ dEnterOrig = 0.f; }
    if( dExitOrig  > 1.f ){ dExitOrig  = 1.f; }
    if( dEnterOrig > dExitOrig+EPS_DISTANCE ){ return 0; }
    
    RecursionEntry* stack = g_recursionStack;
    stack->ptr = listOrig;
    stack->dEnter = dEnterOrig;
    stack->dExit = dExitOrig;
    stack++;
    
    while( stack != g_recursionStack )
    {
        --stack;
        uintptr_t* list = stack->ptr;
        float dEnter = stack->dEnter;
        float dExit = stack->dExit;
        uintptr_t pRight = *list++;
        
    label_skipstack:
        
        // leaf?
        if( (pRight & 3) == 3 )
        {
            int numPolygons = pRight>>2;
            const Polygon* poly = isectPolygons((const Polygon**)list, numPolygons, dEnter, dExit);
            if( poly ){ return poly; }
            continue;
        }
        
        // recurse
        int a = pRight&3;
        float d = getSplitDistance(*((float*)list), a);
        
        uintptr_t* ch[2] = { list+1, (uintptr_t*)(pRight-a) };
        if( g_dirsgn[a] ){ swap(ch[1], ch[0]); }
        
        if( *ch[1] && d <= dExit+EPS_DISTANCE )
        {
            float newEnter = dEnter;
            if (d > newEnter) newEnter = d;
            stack->ptr = ch[1];
            stack->dEnter = newEnter;
            stack->dExit = dExit;
            stack++;
        }
        
        pRight = *ch[0];
        if( pRight && d >= dEnter-EPS_DISTANCE )
        {
            if( d < dExit ){ dExit = d; }
            list = ch[0]+1;
            goto label_skipstack;
        }
    }
    
    return 0;
}

const Polygon* BSP::rayCast(const Ray& ray) const
{
    setupRayCast(ray);
    float dEnter, dExit;
    getEnterExitDistances(m_aabb, dEnter, dExit);
    const Polygon* result = rayCastList(m_list, dEnter, dExit);
    
    return result;
}

//------------------------------------------------------------------------
// Beam intersection
//------------------------------------------------------------------------

EL_FORCE_INLINE static bool intersectAABBFrustum(const Vector3& m,
												 const Vector3& d,
												 const Vector4* p,
												 int numPlanes)
{
	for( int i=0; i < numPlanes; i++ )
	{
		float NP = d.x*fabsf(p->x)+d.y*fabsf(p->y)+d.z*fabsf(p->z);
		float MP = m.x*p->x+m.y*p->y+m.z*p->z+p->w;
        if( (MP+NP) < 0.0f ){ return false; }
		p++;
	}
	return true;
}

static void beamCastRecursive(uintptr_t* list)
{
    uintptr_t pRight = *list++;
    
    if( g_beamBeam->numPleqs() && !intersectAABBFrustum(g_beamMid, g_beamDiag, &g_beamBeam->getPleq(0), g_beamBeam->numPleqs()) )
    {
        return;
    }
    
    // leaf?
    if( (pRight & 3) == 3 )
    {
        uintptr_t numTriangles = pRight>>2;
        for( int i=0; i < numTriangles; i++ )
        {
            const Polygon* poly = (const Polygon*)(*list++);
            if( g_foundPolygons.find(poly) != g_foundPolygons.end() )
            {
                continue;
            }
            
            g_beamResult->push_back(poly);
            g_foundPolygons.insert(poly);
        }
        return;
    }
    
    // recurse
    unsigned int axis = pRight & 3;
    float splitPos = *((float*)list);
    
    uintptr_t* ch[2] = { list+1, (uintptr_t*)(pRight-axis) };
    
    float om = g_beamMid[axis];
    float od = g_beamDiag[axis];
    
    g_beamMid[axis]  = .5f*(om-od + splitPos);
    g_beamDiag[axis] = splitPos - g_beamMid[axis];
    beamCastRecursive(ch[0]);
    
    g_beamMid[axis]  = .5f*(om+od + splitPos);
    g_beamDiag[axis] = g_beamMid[axis] - splitPos;
    beamCastRecursive(ch[1]);
    
    g_beamMid[axis]  = om;
    g_beamDiag[axis] = od;
}

void BSP::beamCast(const Beam& beam, std::vector<const Polygon*>& result) const
{
    g_beamMid = .5f*(m_aabb.m_mn + m_aabb.m_mx);
    g_beamDiag = .5f*(m_aabb.m_mx - m_aabb.m_mn);
    g_beamBeam = &beam;
    g_beamResult = &result;
    
    g_foundPolygons.clear();
    beamCastRecursive(m_list);
}

//------------------------------------------------------------------------
// Utility wrappers
//------------------------------------------------------------------------

Vector3 BSP::getIntersectionPoint(void)
{
	return g_intersectionPoint;
}

//------------------------------------------------------------------------
