//#pragma once
//
//#include <type_traits>
//#include <iostream>
//#include <iomanip>
//
//#define GENERATE_HAS_MEMBER(member)                                               \
//                                                                                  \
//template < class T >                                                              \
//class HasMember_##member                                                          \
//{                                                                                 \
//private:                                                                          \
//    using Yes = char[2];                                                          \
//    using  No = char[1];                                                          \
//                                                                                  \
//    struct Fallback { int member; };                                              \
//    struct Derived : T, Fallback { };                                             \
//                                                                                  \
//    template < class U >                                                          \
//    static No& test ( decltype(U::member)* );                                     \
//    template < typename U >                                                       \
//    static Yes& test ( U* );                                                      \
//                                                                                  \
//public:                                                                           \
//    static constexpr bool RESULT = sizeof(test<Derived>(nullptr)) == sizeof(Yes); \
//};                                                                                \
//                                                                                  \
//template < class T >                                                              \
//struct has_member_##member                                                        \
//: public std::integral_constant<bool, HasMember_##member<T>::RESULT>              \
//{ };                                                                              \
//
//GENERATE_HAS_MEMBER(text)
//GENERATE_HAS_MEMBER(wrapLength)
//GENERATE_HAS_MEMBER(r)
//GENERATE_HAS_MEMBER(g)
//GENERATE_HAS_MEMBER(b)