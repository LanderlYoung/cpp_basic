/*
 * <pre>
 * Author: taylorcyang@tencent.com
 * Date:   2020-01-26
 * Time:   16:53
 * Life with Passion, Code with Creativity.
 * </pre>
 */

#include <iostream>
#include <typeinfo>
#include <utility>
#include <vector>
#include <gtest/gtest.h>
#include <boost/type_index.hpp>

using namespace std;
using boost::typeindex::type_id;
using boost::typeindex::type_id_with_cvr;

TEST(BOOST, TypeIndex) {
    vector<int> v;
    auto it = v.cbegin();

    cout << "*** Using typeid\n";
    cout << typeid(const int).name() << endl;
    cout << typeid(v).name() << endl;
    cout << typeid(it).name() << endl;
    auto name = type_id<decltype(v)>().pretty_name();
    cout << "*** Using type_id\n";
    cout << type_id<const int>() << endl;
    cout << type_id<unique_ptr<vector<int>>>() << endl;
    cout << type_id<shared_ptr<vector<int>>>() << endl;
    cout << type_id<decltype(v)>() << endl;
    cout << type_id<decltype(it)>() << endl;

    cout << "*** Using type_id_with_cvr\n";
    cout << type_id_with_cvr<const int>() << endl;
    cout << type_id_with_cvr<decltype((v))>() << endl;
    cout << type_id_with_cvr<decltype(move((v)))>() << endl;
    cout << type_id_with_cvr<decltype((it))>() << endl;
}
