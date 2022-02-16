#pragma once

#define asserteq(a, b) {auto assert_a_val = (a); auto assert_b_val = (b); if((assert_a_val) == (assert_b_val)) { } else { std::cerr << "Assertion Failed in line " << __LINE__ << ": (" << #a " == " << #b << "), "<< assert_a_val << " != " << assert_b_val << std::endl; exit(1);}}

#define assert_greater_eq(a, b) {auto assert_a_val = (a); auto assert_b_val = (b); if((assert_a_val) > (assert_b_val)) { } else { std::cerr << "Assertion Failed in line " << __LINE__ << ": (" << #a " >= " << #b << "), "<< assert_a_val << " < " << assert_b_val << std::endl; exit(1);}}

#define assert_less(a, b) {auto assert_a_val = (a); auto assert_b_val = (b); if((assert_a_val) < (assert_b_val)) { } else { std::cerr << "Assertion Failed in line " << __LINE__ << ": (" << #a " < " << #b << "), "<< assert_a_val << " >= " << assert_b_val << std::endl; exit(1);}}
