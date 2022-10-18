#include <iostream>
#include <string>

#include "TestFunctions.hpp"
// #include "StaticVariantT.hpp"

JEJO_BEGIN

// ---------- SFINAE_HPP ---------


// ---------- TEMPLATE_FUNCTION_HPP ---------
#pragma region template_functions 

#pragma endregion

// ---------- TRAITS_HPP ---------
#pragma region traits

#pragma endregion 

// ---------- TEMPLATE_CLASSES_HPP ---------
#pragma region template_classes 

template<typename T> class Base{};
class A : public Base<A> { std::string s; std::string s1;};
class B: public Base<B>{};


void static_variant_test()
{
    //variant<A, B> v;
    //std::cout << sizeof(v) << " " << sizeof(A) << "\n";

    //v = 2.f;

    // std::cout << v.get<float>() << std::endl;

    //auto s = std::move(v);

    // std::cout << s.get<float>() << std::endl;

    //s = 999;

    //std::cout << s.get<float>() << std::endl;

    //s = v;

    //std::cout << s.get<std::string>() << std::endl;
}

#pragma endregion

JEJO_END

