sample: sample.cpp
	g++ -std=c++11 sample.cpp -o sampleProgram && ./sampleProgram

clean:
	rm -f sampleProgram