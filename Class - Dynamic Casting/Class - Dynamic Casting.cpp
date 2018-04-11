#include <vector>
#include <memory>
#include <iostream>

class X 
{
public:
	int id;
	virtual ~X() = default;
};

class A : public X
{
public:
	void run() { std::cout << "Rnning A Class....\n"; }
};

class B : public X
{
public:
	int lala = 10;
};

int main() 
{
	std::vector<std::unique_ptr<X>> types;

	types.emplace_back(std::make_unique<A>());
	types.emplace_back(std::make_unique<B>());

	int var = 0;
	for (int i = 0; i < 2; ++i) 
	{
		// for raw pointers
		// if (auto ta = dynamic_cast<A *>(types[i]))
		if (auto ta = dynamic_cast<A *>(types[i].get())) 
			ta->run();
		
		if (auto tb = dynamic_cast<B *>(types[i].get())) 
			var = tb->lala;
	}
	std::cout << var << "\n";
	std::cin.get();
}
