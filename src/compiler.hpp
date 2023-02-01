#pragma once

#include <algorithm>
#include <functional>
#include <iterator>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <tuple>
#include <utility>

#include <robin_hood.h>

#include "abstract_syntax_tree.hpp"
#include "model.hpp"
#include "expand.hpp"
#include "product.hpp"
#include "variables.hpp"

namespace pyqubo {
    // Compile.
  inline auto compile(const std::shared_ptr<const expression>& express, const std::shared_ptr<const expression>& strength) noexcept {
    auto variables = pyqubo::variables();

    const auto [polynomial, sub_hamiltonians, constraints] = expand()(express, &variables);
    
    std::cout << "polynomial:" << polynomial.to_string() << std::endl;

    // Remove all 0-valued elements. This can simplify the final formula by removing qubic terms.
    auto new_polynomial = pyqubo::polynomial();
    for(auto [key, val]: *(polynomial.get_terms())){
      //std::cout << "polynomial " << key.to_string() << ", " << val->to_string() << std::endl;
      if (val->expression_type() == pyqubo::expression_type::numeric_literal){
        const auto numval = std::static_pointer_cast<const pyqubo::numeric_literal>(val)->value();
        if (numval != 0.0){
          new_polynomial.insert({key,val});
        }
      }
    }
    auto new_poly = pyqubo::poly(&new_polynomial);

    std::cout << "new polynomial: " << new_poly.to_string() << std::endl;

    //const auto quadratic_polynomial = convert_to_quadratic(*(polynomial.get_terms()), strength, &variables);
    const auto quadratic_polynomial = convert_to_quadratic(*(new_poly.get_terms()), strength, &variables);
    
    // for(auto [key, val]: quadratic_polynomial){
    //   std::cout << "quadratic_polynomial " << key.to_string() << ", " << val->to_string() << std::endl;
    // }

    // for(auto [key, val]: sub_hamiltonians){
    //   std::cout << "sub_hamiltonians " << key << ", " << val.to_string() << std::endl;
    // }
    
    return model(quadratic_polynomial, sub_hamiltonians, constraints, variables);
  }
}