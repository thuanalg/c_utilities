LIBNAME=libmycode
SERVICENAME=mycode.service
SRC1= main.c
SRC2= mycode.c
LIBS= -lsqlite3 -lpthread -lrt

all:
	gcc -g -o ${SERVICENAME} ${SRC1} ${LIBS}
	gcc -shared -o ${LIBNAME}.so -fPIC ${SRC2} ${LIBS}
clean:
	rm ${SERVICENAME} ${LIBNAME}.so
install:
	
