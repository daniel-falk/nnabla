// Copyright 2021 Sony Corporation.
// Copyright 2021 Sony Group Corporation.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef NBLA_FUNCTION_CUMSUM_HPP
#define NBLA_FUNCTION_CUMSUM_HPP

#include <nbla/cpu.hpp>
#include <nbla/function.hpp>
#include <nbla/function_registry.hpp>

namespace nbla {

NBLA_REGISTER_FUNCTION_HEADER(CumSum, int, bool, bool);

/**
    Performs cumulative summation of a given array along a specific axis.
    Given x = [x1, x2, x3]
    y = [x1, x1+x2, x1+x2+x3] if not exclusive and not reverse
    y = [0, x1, x1+x2] if exclusive and not reverse
    y = [x1+x2+x3, x2+x3, x3] if not exclusive and reverse
    y = [x2+x3, x3, 0] if exclusive and reverse

Inputs:
- x: N-D array
- (param) axis: int
- (param) exclusive: bool
- (param) reverse: bool

Outputs:
- y: N-D array

\ingroup FunctionImplGrp
 */

template <typename T> class CumSum : public BaseFunction<int, bool, bool> {
protected:
  int axis_;
  bool exclusive_;
  bool reverse_;
  Size_t size_, size0_, size1_, size2_;

public:
  CumSum(const Context &ctx, int axis, bool exclusive, bool reverse)
      : BaseFunction(ctx, axis, exclusive, reverse), axis_(axis),
        exclusive_(exclusive), reverse_(reverse) {}
  virtual ~CumSum() {}
  virtual shared_ptr<Function> copy() const {
    return create_CumSum(ctx_, axis_, exclusive_, reverse_);
  }
  virtual int min_inputs() { return 1; }
  virtual int min_outputs() { return 1; }
  virtual vector<dtypes> in_types() { return vector<dtypes>{get_dtype<T>()}; }
  virtual vector<dtypes> out_types() { return vector<dtypes>{get_dtype<T>()}; }
  virtual vector<string> allowed_array_classes() {
    return SingletonManager::get<Cpu>()->array_classes();
  }
  virtual string name() { return "CumSum"; }
  // TODO: This must be overridden if any of input grad depends on a output
  // data. See doc in function.hpp.
  // virtual bool grad_depends_output_data(int i, int o) const {
  // }
  // TODO: If any of data/grad storage is inplaced with any of output, you must
  // override some of these. See doc in function.hpp.
  // virtual int inplace_data(int i) const {
  // }
  // virtual int inplace_data_with(int i) const {
  // }
  // TODO: If you want to avoid clearing input buffers in any case, define this
  // function returning true.
  // virtual bool prohibit_clear_input_buffers() const {
  //   return true;
  // }
  // TODO: If you want to avoid zero-ing gradient of inputs even when accum is
  // true, uncomment the following function definition.
  // virtual bool prohibit_zero_input_grad() const {
  //   return true;
  // }
  virtual bool grad_depends_output_data(int i, int o) const { return false; }

protected:
  NBLA_API virtual void setup_impl(const Variables &inputs,
                                   const Variables &outputs);
  NBLA_API virtual void forward_impl(const Variables &inputs,
                                     const Variables &outputs);
  NBLA_API virtual void backward_impl(const Variables &inputs,
                                      const Variables &outputs,
                                      const vector<bool> &propagate_down,
                                      const vector<bool> &accum);
  virtual bool grad_depends_input_data_impl(int i, int j) const {
    return false;
  }
};
}
#endif
