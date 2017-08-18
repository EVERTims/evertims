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
 * (C) 2007 Lauri Savioja
 * Helsinki University of Technology
 *
 * (C) 2008-2017 Markus Noisternig
 * IRCAM-CNRS-UPMC UMR9912 STMS
 *
 ************************************************************************/
 

// DEPRECATED: RT60 is now estimated in Blender add-on

#ifndef _REVERB_ESTIM_H
#define _REVERB_ESTIM_H

#include <map>
#include <cstring>

#define MAX_BANDS 10

#include "elPathSolution.h"

class Response
{
    
public:
    
    Response(double samplingFrequency, double lengthInSecond);
    ~Response();
    
    void setItem(double time, double value);
    void addItem(double time, double value);
    
    void  SchroederIntegrate();
    float search           (double val = -0.00001);
    void  getMaxMin(double& maxValue, double& maxTime, double& minValue, double& minTime);
    
    void print(char* name);
    
    
private:
    
    double m_samplingFrequency;
    int m_length;
    
    double* m_signal;
};

class ReverbEstimator
{
    
public:
    
    ReverbEstimator (double samplingFrequency, EL::PathSolution *solution, double speedOfSound, double maxTime);
    ~ReverbEstimator ();
    
    void getMaxMin(int band, double& maxValue, double& maxTime, double& minValue, double& minTime);
    
    float getEstimateR60(int band, double startDecay, double endDecay);
    
    
private:
    
    Response* m_SchroederPlots[MAX_BANDS];
    
};

#endif
