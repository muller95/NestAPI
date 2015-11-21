PROG="/home/vadim/NestAPI/mtnest"
LIB="/home/vadim/NestAPI"
DRAWPOSITS="/home/vadim/drawposits"
ERRLOG="/home/vadim/merrlog"
WIDTH=800
HEIGHT=800
ITERS=3
FIFO="./cmdfifo"
#DATA="/home/vadim/frontend/approx.data"
DATA="/home/vadim/test.txt"
#DATA="/home/vadim/NestAPI/poly.in"
BACKEND="/home/vadim/frontend/backend.awk"

#LD_LIBRARY_PATH=/home/vadim/NestAPI
#export LD_LIBRARY_PATH
rm -rf $DRAWPOSITS
mkdir $DRAWPOSITS

for i in {1..1} 
do
#	$PROG $WIDTH $HEIGHT $ITERS $FIFO <$DATA | awk -f $BACKEND -v width=$WIDTH -v height=$HEIGHT -v outfile=./testo.svg
	$PROG $WIDTH $HEIGHT $ITERS $FIFO <$DATA >trfrms.data 
	cat trfrms.data
	awk -f $BACKEND -v width=$WIDTH -v height=$HEIGHT -v outfile=./testo.svg <trfrms.data
done

#$PROG <poly.in args222
