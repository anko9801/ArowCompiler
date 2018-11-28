#include<iostream>
#include<vector>
#include<time.h>

int main(){
	int sum = 0;
	for(int j=0;j<20;j++){
		clock_t start = clock();
		std::vector<int> v;
		for(int i=0;i<10000000;i++){
			if (i >= 0){
				v.push_back(i);
			}
		}
		clock_t end = clock();
		std::cout << double(end - start) << std::endl;
		sum += end - start;
	}
	std::cout << sum / 20 * 100000 / CLOCKS_PER_SEC << std::endl << std::endl;

	sum = 0;
	for(int j=0;j<20;j++){
		clock_t start = clock();
		std::vector<int> v;
		for(int i=0;i<10000000;i++){
			v.push_back(i);
		}
		clock_t end = clock();
		std::cout << double(end - start) << std::endl;
		sum += end - start;
	}
	std::cout << sum / 20 * 100000 / CLOCKS_PER_SEC << std::endl << std::endl;

	sum = 0;
	int a = 0;
	for(int j=0;j<20;j++){
		clock_t start = clock();
		std::vector<int> v;
		for(int i=0;i<10000000;i++){
			if (i > a){
				v.push_back(i);
			}
		}
		clock_t end = clock();
		std::cout << double(end - start) << std::endl;
		sum += end - start;
	}
	std::cout << sum / 20  * 100000 / CLOCKS_PER_SEC << std::endl << std::endl;
	return 0;
}
