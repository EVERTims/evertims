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
 

// DEPRECATED: RT60 is now estimated in Blender add-on

#include <iostream>
#include <fstream>

#include "reverbEstimate.h"
#include "elPolygon.h"
#include "utils.h"

#define SPEED_OF_SOUND 340

Response::Response(double samplingFrequency, double lengthInSeconds):
m_samplingFrequency(samplingFrequency),
m_length((int)(lengthInSeconds * samplingFrequency))
{
    m_signal = new double[m_length];
    // DPQ: set original values to zero since used recursively afterwards
    for( int i = 0; i < m_length; i++){ m_signal[i] = 0.0; }
}

Response::~Response()
{
    delete m_signal;
}

void Response::setItem(double time, double value)
{
    int idx = (int) (time * m_samplingFrequency);
    if (idx < m_length){ m_signal[idx] = value; }
}

void Response::addItem(double time, double value)
{
    int idx = (int) (time * m_samplingFrequency);
    if (idx < m_length){ m_signal[idx] += value; }
}

void Response::SchroederIntegrate()
{
    int idx = m_length - 1;
    double sum = 0;
    
    while (idx)
    {
        sum += m_signal[idx];
        m_signal[idx--] = sum;
    }
    
    idx = m_length - 1;
    while (idx)
    {
        // DPQ: avoid adding -Inf values to m_signal when (guess here:) processing solution
        if (m_signal[idx] != 0)
        {
            m_signal[idx] = 10 * log10(m_signal[idx] / sum);
        }
        
        idx--;
    }
}

float Response::search (double val)
{
    int idx = 0;
    
    for (; idx < m_length; idx++){ if (m_signal[idx] <= val){ break; }}
    
    return (float)( (float)idx / m_samplingFrequency );
}

void Response::getMaxMin(double& maxValue, double &maxTime, double& minValue, double& minTime)
{
    int idx;
    
    for (idx = 1; idx < m_length; idx++)
    {
        if ( m_signal[idx] != m_signal[idx-1] ){ break; }
    }
    
    if (idx < m_length)
    {
        maxValue = m_signal[idx];
        maxTime  = (double)idx / m_samplingFrequency;
    }
    
    for (idx = m_length - 1; idx >= 0; idx--)
    {
        if ( m_signal[idx] != m_signal[idx+1] ){ break; }
    }
    
    if (idx >= 0)
    {
        minValue = m_signal[idx];
        minTime  = (double)idx / m_samplingFrequency;
    }
}

void Response::print(char *name)
{
    std::ofstream outs;
    
    outs.open (name);
    
    for (int i = 0; i < m_length; i++)
    {
        outs << m_signal[i] << std::endl;
    }
    
    outs.close();
}

ReverbEstimator::ReverbEstimator (double samplingFrequency, EL::PathSolution *solution, double speedOfSound, double maxTime)
{
    int idx = 0;
    
    for (int i = 0; i < MAX_BANDS; i++)
    {
        m_SchroederPlots[i] = new Response(samplingFrequency, maxTime);
    }
    
    double len;
    double time;
    float reflectance[MAX_BANDS];
    
    for (int i = 0; i < solution->numPaths(); i++)
    {
        const EL::PathSolution::Path& path = solution->getPath(i);
        
        // get path total duration
        len = solution->getLength( path );
        time = len / speedOfSound;
        
        // init per-band reflectance coefficients
        for (int k = 0; k < MAX_BANDS; k++){ reflectance[k] = 1.0; }
        
        // estimate path absorption based on encountered materials
        for (int j = 0; j < path.m_order; j++)
        {
            const EL::Polygon* p = path.m_polygons[j];
            const Material& m = p->getMaterial ();
            for (int k = 0; k < MAX_BANDS; k++) reflectance[k] *= ( 1 - m.absorption[k] );
        }
        
        // add estimated reflectance to schroeder plot (at path time)
        for (int k = 0; k < MAX_BANDS; k++)
        {
            m_SchroederPlots[k]->addItem(time, reflectance[k]);  // Should I divide this by len ?
        }
    }
    //  m_SchroederPlots[0]->print("energy.txt");
    
    for (int k = 0; k < MAX_BANDS; k++)
    {
        m_SchroederPlots[k]->SchroederIntegrate();
    }
    
    //  m_SchroederPlots[0]->print("schroeder.txt");
}


ReverbEstimator::~ReverbEstimator()
{
    for (int i = 0; i < MAX_BANDS; i++){ delete m_SchroederPlots[i]; }
}

float ReverbEstimator::getEstimateR60(int band, double startDecayAmpl, double endDecayAmpl)
{
    Response* r = m_SchroederPlots[band];
    
    COUT << "Starting to search the region: " << startDecayAmpl << " - " << endDecayAmpl << "\n";
    
    // convert from schroeder integrate values to originals
    float realStart = r->search();
    COUT << "realStart = " << realStart << "\n";
    float startTime = r->search(startDecayAmpl);
    COUT << "startTime = " << startTime << "\n";
    float endTime   = r->search(endDecayAmpl);
    COUT << "endTime = " << endTime << "\n";
    
    // if material is full absorber:
    if( realStart > endTime ) return 0.0;
    
    // get total decay time (based on start time + schroeder slope times -60dB)
    // float totalDecay = realStart + (float)(60 * (endTime-startTime)/(endDecayAmpl - startDecayAmpl));
    // DPQ: the minus seems to make more sense when looking at a Schroeder graph
    float totalDecay = realStart + (float)(- 60 * (endTime-startTime)/(endDecayAmpl - startDecayAmpl));
    
    return totalDecay;
}

void ReverbEstimator::getMaxMin(int band, double& maxValue, double& maxTime, double& minValue, double& minTime)
{
    Response* r = m_SchroederPlots[band];
    
    r->getMaxMin(maxValue, maxTime, minValue, minTime);
}

