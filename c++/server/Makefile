Cxx=g++
linkDir= -L /usr/local/
thread= -lpthread
boostThread= -lboost_thread
name=Server

server:
	$(Cxx) *.cpp  $(linkDir) $(boostThread) $(thread) -o $(name)
clean:
	rm $(name)
