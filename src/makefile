DIR = dist
objects = $(DIR)/BallTree.o $(DIR)/BallTreeNode.o $(DIR)/Page.o $(DIR)/PagePool.o $(DIR)/test.o $(DIR)/Utility.o
cc = g++ -std=c++11

BallTree: $(objects)
	$(cc) -o bin/BallTree $(objects)

$(DIR)/BallTree.o: BallTree.h Utility.h Page.h PagePool.h BallTreeNode.h
	$(cc) -o $(DIR)/BallTree.o -c BallTree.cpp
$(DIR)/BallTreeNode.o: BallTreeNode.h Utility.h PagePool.h
	$(cc) -o $(DIR)/BallTreeNode.o -c BallTreeNode.cpp
$(DIR)/Page.o: Page.h
	$(cc) -o $(DIR)/Page.o -c Page.cpp
$(DIR)/PagePool.o: Page.h PagePool.h
	$(cc) -o $(DIR)/PagePool.o -c PagePool.cpp
$(DIR)/Utility.o: Utility.h
	$(cc) -o $(DIR)/Utility.o -c Utility.cpp
$(DIR)/test.o:
	$(cc) -o $(DIR)/test.o -c test.cpp
clean .PHONY:
	rm -f dist/* & rm -f bin/*
