#pragma once

// @generated by torchgen/gen.py from Function.h

#include <ATen/Context.h>
#include <ATen/DeviceGuard.h>
#include <ATen/TensorUtils.h>
#include <ATen/TracerMode.h>
#include <ATen/core/Generator.h>
#include <ATen/core/Reduction.h>
#include <ATen/core/Tensor.h>
#include <c10/core/Scalar.h>
#include <c10/core/Storage.h>
#include <c10/core/TensorOptions.h>
#include <c10/util/Deprecated.h>
#include <c10/util/Optional.h>



#include <ATen/ops/linalg_svdvals_ops.h>

namespace at {


// aten::linalg_svdvals(Tensor A) -> Tensor
TORCH_API inline at::Tensor linalg_svdvals(const at::Tensor & A) {
    return at::_ops::linalg_svdvals::call(A);
}

// aten::linalg_svdvals.out(Tensor A, *, Tensor(a!) out) -> Tensor(a!)
TORCH_API inline at::Tensor & linalg_svdvals_out(at::Tensor & out, const at::Tensor & A) {
    return at::_ops::linalg_svdvals_out::call(A, out);
}

// aten::linalg_svdvals.out(Tensor A, *, Tensor(a!) out) -> Tensor(a!)
TORCH_API inline at::Tensor & linalg_svdvals_outf(const at::Tensor & A, at::Tensor & out) {
    return at::_ops::linalg_svdvals_out::call(A, out);
}

}
