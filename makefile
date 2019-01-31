all:
	icpc -O0 -g -o driver ./driver.cpp -qopenmp -qopt-report=5 -xMIC-AVX512

clean:
	rm driver
