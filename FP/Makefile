CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra
TARGET = flower_exchange
SRCS = main.cpp CSVReader.cpp CSVWriter.cpp Exchange.cpp ExecutionReport.cpp Order.cpp OrderBook.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) $(OBJS)

.PHONY: all run clean
