/***********************************************************************
 *
 * This file is part of the EVERTims room acoustic modeling software.
 * It is released under the MIT License. You should have received a
 * copy of the MIT License along with EVERTims.  If not, see
 * http://www.opensource.org/licenses/mit-license.php
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * For details, see the LICENSE file
 *
 * The Reverberation estimation class for EVERTims
 *
 * (C) 2009 Lauri Savioja
 * Helsinki University of Technology
 *
 * (C) 2009-2015 Markus Noisternig
 * IRCAM-CNRS-UPMC UMR9912 STMS
 *
 ***********************************************************************/

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
