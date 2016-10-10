# This is an example starting script for EVERTims

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
$DIR/../bin/ims -s 3858 -a 'listener_1/127.0.0.1:3860' -v 'listener_1/localhost:3862' -d 1 -D 2 -m $DIR/../resources/materials.dat -p 'listener_1/'