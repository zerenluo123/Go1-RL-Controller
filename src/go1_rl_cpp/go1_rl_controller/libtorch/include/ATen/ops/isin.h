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



#include <ATen/ops/isin_ops.h>

namespace at {


// aten::isin.Tensor_Tensor_out(Tensor elements, Tensor test_elements, *, bool assume_unique=False, bool invert=False, Tensor(a!) out) -> Tensor(a!)
TORCH_API inline at::Tensor & isin_out(at::Tensor & out, const at::Tensor & elements, const at::Tensor & test_elements, bool assume_unique=false, bool invert=false) {
    return at::_ops::isin_Tensor_Tensor_out::call(elements, test_elements, assume_unique, invert, out);
}

// aten::isin.Tensor_Tensor_out(Tensor elements, Tensor test_elements, *, bool assume_unique=False, bool invert=False, Tensor(a!) out) -> Tensor(a!)
TORCH_API inline at::Tensor & isin_outf(const at::Tensor & elements, const at::Tensor & test_elements, bool assume_unique, bool invert, at::Tensor & out) {
    return at::_ops::isin_Tensor_Tensor_out::call(elements, test_elements, assume_unique, invert, out);
}

// aten::isin.Tensor_Tensor(Tensor elements, Tensor test_elements, *, bool assume_unique=False, bool invert=False) -> Tensor
TORCH_API inline at::Tensor isin(const at::Tensor & elements, const at::Tensor & test_elements, bool assume_unique=false, bool invert=false) {
    return at::_ops::isin_Tensor_Tensor::call(elements, test_elements, assume_unique, invert);
}

// aten::isin.Tensor_Scalar_out(Tensor elements, Scalar test_element, *, bool assume_unique=False, bool invert=False, Tensor(a!) out) -> Tensor(a!)
TORCH_API inline at::Tensor & isin_out(at::Tensor & out, const at::Tensor & elements, const at::Scalar & test_element, bool assume_unique=false, bool invert=false) {
    return at::_ops::isin_Tensor_Scalar_out::call(elements, test_element, assume_unique, invert, out);
}

// aten::isin.Tensor_Scalar_out(Tensor elements, Scalar test_element, *, bool assume_unique=False, bool invert=False, Tensor(a!) out) -> Tensor(a!)
TORCH_API inline at::Tensor & isin_outf(const at::Tensor & elements, const at::Scalar & test_element, bool assume_unique, bool invert, at::Tensor & out) {
    return at::_ops::isin_Tensor_Scalar_out::call(elements, test_element, assume_unique, invert, out);
}

// aten::isin.Tensor_Scalar(Tensor elements, Scalar test_element, *, bool assume_unique=False, bool invert=False) -> Tensor
TORCH_API inline at::Tensor isin(const at::Tensor & elements, const at::Scalar & test_element, bool assume_unique=false, bool invert=false) {
    return at::_ops::isin_Tensor_Scalar::call(elements, test_element, assume_unique, invert);
}

// aten::isin.Scalar_Tensor_out(Scalar element, Tensor test_elements, *, bool assume_unique=False, bool invert=False, Tensor(a!) out) -> Tensor(a!)
TORCH_API inline at::Tensor & isin_out(at::Tensor & out, const at::Scalar & element, const at::Tensor & test_elements, bool assume_unique=false, bool invert=false) {
    return at::_ops::isin_Scalar_Tensor_out::call(element, test_elements, assume_unique, invert, out);
}

// aten::isin.Scalar_Tensor_out(Scalar element, Tensor test_elements, *, bool assume_unique=False, bool invert=False, Tensor(a!) out) -> Tensor(a!)
TORCH_API inline at::Tensor & isin_outf(const at::Scalar & element, const at::Tensor & test_elements, bool assume_unique, bool invert, at::Tensor & out) {
    return at::_ops::isin_Scalar_Tensor_out::call(element, test_elements, assume_unique, invert, out);
}

// aten::isin.Scalar_Tensor(Scalar element, Tensor test_elements, *, bool assume_unique=False, bool invert=False) -> Tensor
TORCH_API inline at::Tensor isin(const at::Scalar & element, const at::Tensor & test_elements, bool assume_unique=false, bool invert=false) {
    return at::_ops::isin_Scalar_Tensor::call(element, test_elements, assume_unique, invert);
}

}
