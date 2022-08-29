#pragma once

// @generated by torchgen/gen.py from Operator.h

#include <tuple>
#include <vector>

// Forward declarations of any types needed in the operator signatures.
// We can't directly include these classes because it will cause circular include dependencies.
// This file is included by TensorBody.h, which defines the Tensor class.
#include <ATen/core/ATen_fwd.h>

namespace at {
namespace _ops {


struct TORCH_API _pad_enum {
  using schema = at::Tensor (const at::Tensor &, at::IntArrayRef, int64_t, c10::optional<double>);
  using ptr_schema = schema*;
  // See Note [static constexpr char* members for windows NVCC]
  STATIC_CONSTEXPR_STR_INL_EXCEPT_WIN_CUDA(name, "aten::_pad_enum")
  STATIC_CONSTEXPR_STR_INL_EXCEPT_WIN_CUDA(overload_name, "")
  STATIC_CONSTEXPR_STR_INL_EXCEPT_WIN_CUDA(schema_str, "_pad_enum(Tensor self, int[] pad, int mode, float? value=None) -> Tensor")
  static at::Tensor call(const at::Tensor & self, at::IntArrayRef pad, int64_t mode, c10::optional<double> value);
  static at::Tensor redispatch(c10::DispatchKeySet dispatchKeySet, const at::Tensor & self, at::IntArrayRef pad, int64_t mode, c10::optional<double> value);
};

}} // namespace at::_ops
