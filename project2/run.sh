DE=""
E_ARG=55	#usage error

# Usage info
show_help()
{
	echo "Usage: [-d] [-H] [-r repeat] <size|file> <threads>
		-d			Display result matrix.
		-r repeat		Repeat execution.
		-H			Help

		For Example: ./lu 3000 15 -d
		"
		exit ${E_ARG}
}

while getopts "dHr:" OPT
do
case $OPT in
d)
D_MODE="-d"	
;;
H) 
show_help	;;
r)
re="${OPTARG}"	;;
\?)
echo "Invalid option: -$OPTARG. Use -H flag for help."
exit ${E_ARG}
;;
esac																													done

if [ "${re}" != "" ]
then
echo "re 값은:${re}"
fi

#OPTIND shift
shift $(( $OPTIND - 1))

	arg1=$1			#matrix size : n
	arg2=$2			#thread num

	MATRIX_SIZE=${arg1}
	THREADS_NUM=${arg2:-16} 

	if [ $# = 0 ]
	then
	echo "Invalid argument: Use -H flag for help."
	exit $E_ARG
	fi

	echo "matrix size::${MATRIX_SIZE}"
	echo "thread num::${THREADS_NUM}"

#make use Makefil
	make

	while [ ${re:-1} -ge 1 ]; do
	./test ${MATRIX_SIZE} ${THREADS_NUM} ${D_MODE}
re=$(( ${re}-1 ))
	done

