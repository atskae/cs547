#include <thread>
#include <iostream> 


void foo(){
	auto thread_id = std::this_thread::get_id();
	std::cout << "Thread id :" << thread_id << "\n";

}

int main(){
	std::thread first (foo);
	auto tid1 = first.get_id();
	std::thread second(foo);
	auto tid2 = second.get_id();

	std::cout << "Tid for first is: " << tid1 << '\n';

	std::cout << "Tid for second  is: " << tid2 << '\n';


	first.join();
	second.join();
	
       	tid1 = first.get_id();
 	tid2 = second.get_id();
	
	std::cout << "Tid for first is: " << tid1 << '\n';

	std::cout << "Tid for second  is: " << tid2 << '\n';

	

	return 0;
}
