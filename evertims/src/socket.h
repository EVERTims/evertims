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
 

#ifndef __SOCKET_HPP
#define __SOCKET_HPP

// ********************************************************
// BEWARE: Never tested under WIN32 !!
// ********************************************************

// ********************************************************
// The includes
// ********************************************************
#ifndef _WIN32
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/errno.h>
#include <netdb.h>
#include <unistd.h>

#ifdef __Linux
#include <string.h>
#include <stdlib.h>
#endif

typedef int SOCKET;
#define  SOCKADDR_IN struct sockaddr_in

#else
#include <winsock2.h>
#include <Ws2tcpip.h>
WSADATA initialisation_win32; // Variable permettant de récupérer la structure d'information sur l'initialisation
WSADATA initialisation2_win32; // Variable permettant de récupérer la structure d'information sur l'initialisation
#endif


class Socket {
    
public:
    
    Socket(int port);
    Socket(char *hostAndPort);
    
    ~Socket();
    
    int read(char *buf);
    void write(int len, char *buf);
    
    
private:
    
    void openSocket();
    
    int m_socket_id;
    struct sockaddr_in m_sockaddr;
};

#endif
