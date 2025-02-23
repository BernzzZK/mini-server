.PHONY: clean

redis.out:redis.o test.o
	@g++ redis.o test.o -o redis.out -lhiredis
%.o:%.cpp
	@g++ -c $< -o $@
clean : 
	@rm -rf *.out *.o
