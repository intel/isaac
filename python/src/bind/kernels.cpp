/*
 * Copyright (c) 2015, PHILIPPE TILLET. All rights reserved.
 *
 * This file is part of ISAAC.
 *
 * ISAAC is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301  USA
 */

#include "isaac/jit/generation/elementwise_1d.h"
#include "isaac/jit/generation/elementwise_2d.h"
#include "isaac/jit/generation/reduce_1d.h"
#include "isaac/jit/generation/reduce_2d.h"
#include "isaac/jit/generation/gemm.h"

#include "common.hpp"
#include "kernels.h"


namespace tpt = isaac::templates;


namespace detail
{
  bp::list input_sizes(tpt::base & temp, sc::expression_tree const & tree)
  {
      std::vector<isaac::int_t> tmp = temp.input_sizes(tree);
      return tools::to_list(tmp.begin(), tmp.end());
  }
}

void export_templates()
{

  bp::object templates_module(bp::handle<>(bp::borrowed(PyImport_AddModule("isaac.templates"))));
  bp::scope().attr("templates") = templates_module;
  bp::scope template_scope = templates_module;

  //Base
  {
    #define __PROP(name) .def_readonly(#name, &tpt::base::name)
    bp::class_<tpt::base, std::shared_ptr<tpt::base>, boost::noncopyable>("base", bp::no_init)
            .def("lmem_usage", &tpt::base::lmem_usage)
            .def("registers_usage", &tpt::base::registers_usage)
            .def("is_invalid", &tpt::base::is_invalid)
            .def("input_sizes", &detail::input_sizes)
        ;
    #undef __PROP
  }

  bp::class_<tpt::parameterized_base, bp::bases<tpt::base>, boost::noncopyable>("parameterized_base", bp::no_init)
                                        .add_property("ls0", &tpt::parameterized_base::ls0)
                                        .add_property("ls1", &tpt::parameterized_base::ls1);

  bp::class_<tpt::external_base, bp::bases<tpt::base>, boost::noncopyable>("external_base", bp::no_init);

#define WRAP_BASE(name) bp::class_<tpt::name, bp::bases<tpt::parameterized_base>, boost::noncopyable>(#name, bp::no_init);

  #define WRAP_TEMPLATE(name, basename, ...) bp::class_<tpt::name, std::shared_ptr<tpt::name>, bp::bases<basename>>(#name, bp::init<__VA_ARGS__>())\
                                      ;
  WRAP_TEMPLATE(elementwise_1d, tpt::parameterized_base, uint, uint, uint)
  WRAP_TEMPLATE(elementwise_2d, tpt::parameterized_base, uint, uint, uint, uint, uint)
  WRAP_TEMPLATE(reduce_1d, tpt::parameterized_base, uint, uint, uint)
  WRAP_BASE(reduce_2d)
  WRAP_TEMPLATE(reduce_2d_rows, tpt::reduce_2d, uint, uint, uint, uint, uint)
  WRAP_TEMPLATE(reduce_2d_cols, tpt::reduce_2d, uint, uint, uint, uint, uint)
  WRAP_BASE(gemm)
  WRAP_TEMPLATE(gemm_nn, tpt::gemm, uint, uint, uint, uint, uint, uint, uint, uint, uint, uint)
  WRAP_TEMPLATE(gemm_tn, tpt::gemm, uint, uint, uint, uint, uint, uint, uint, uint, uint, uint)
  WRAP_TEMPLATE(gemm_nt, tpt::gemm, uint, uint, uint, uint, uint, uint, uint, uint, uint, uint)
  WRAP_TEMPLATE(gemm_tt, tpt::gemm, uint, uint, uint, uint, uint, uint, uint, uint, uint, uint)
  WRAP_TEMPLATE(cublas_gemm, tpt::external_base, char, char)
  WRAP_TEMPLATE(intelblas_gemm, tpt::external_base, char, char)


}
