/* Copyright 2019 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#include "tensorflow/lite/delegates/gpu/cl/kernels/elementwise.h"

#include <vector>

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "tensorflow/lite/delegates/gpu/cl/kernels/cl_test.h"
#include "tensorflow/lite/delegates/gpu/common/operations.h"
#include "tensorflow/lite/delegates/gpu/common/status.h"

using ::testing::FloatNear;
using ::testing::Pointwise;

namespace tflite {
namespace gpu {
namespace cl {
namespace {

TEST_F(OpenCLOperationTest, Abs) {
  TensorFloat32 src_tensor;
  src_tensor.shape = BHWC(1, 2, 1, 2);
  src_tensor.data = {half(0.0f), half(-1.0f), half(-0.05f), half(0.045f)};

  for (auto storage : env_.GetSupportedStorages()) {
    for (auto precision : env_.GetSupportedPrecisions()) {
      OperationDef op_def;
      op_def.precision = precision;
      auto data_type = DeduceDataTypeFromPrecision(precision);
      op_def.src_tensors.push_back({data_type, storage});
      op_def.dst_tensors.push_back({data_type, storage});
      TensorFloat32 dst_tensor;
      ElementwiseOneInput operation =
          CreateElementwiseOneInput(op_def, OperationType::ABS);
      ASSERT_OK(ExecuteGPUOperation(src_tensor, creation_context_, &operation,
                                    BHWC(1, 2, 1, 2), &dst_tensor));
      EXPECT_THAT(dst_tensor.data,
                  Pointwise(FloatNear(0.0f), {half(0.0f), half(1.0f),
                                              half(0.05f), half(0.045f)}));
    }
  }
}

TEST_F(OpenCLOperationTest, Cos) {
  TensorFloat32 src_tensor;
  src_tensor.shape = BHWC(1, 2, 1, 2);
  src_tensor.data = {0.0f, -1.0f, -0.05f, 0.045f};

  for (auto storage : env_.GetSupportedStorages()) {
    for (auto precision : env_.GetSupportedPrecisions()) {
      const float eps = precision == CalculationsPrecision::F32 ? 1e-6f : 1e-3f;
      OperationDef op_def;
      op_def.precision = precision;
      auto data_type = DeduceDataTypeFromPrecision(precision);
      op_def.src_tensors.push_back({data_type, storage});
      op_def.dst_tensors.push_back({data_type, storage});
      TensorFloat32 dst_tensor;
      ElementwiseOneInput operation =
          CreateElementwiseOneInput(op_def, OperationType::COS);
      ASSERT_OK(ExecuteGPUOperation(src_tensor, creation_context_, &operation,
                                    BHWC(1, 2, 1, 2), &dst_tensor));
      EXPECT_THAT(
          dst_tensor.data,
          Pointwise(FloatNear(eps), {std::cos(0.0f), std::cos(-1.0f),
                                     std::cos(-0.05f), std::cos(0.045f)}));
    }
  }
}

TEST_F(OpenCLOperationTest, HardSwish) {
  TensorFloat32 src_tensor;
  src_tensor.shape = BHWC(1, 1, 1, 7);
  src_tensor.data = {-4.5f, -3.0f, -1.5f, 0.0f, 1.5f, 3.0f, 4.5f};

  for (auto storage : env_.GetSupportedStorages()) {
    for (auto precision : env_.GetSupportedPrecisions()) {
      const float eps = precision == CalculationsPrecision::F32 ? 1e-5f : 1e-2f;
      OperationDef op_def;
      op_def.precision = precision;
      auto data_type = DeduceDataTypeFromPrecision(precision);
      op_def.src_tensors.push_back({data_type, storage});
      op_def.dst_tensors.push_back({data_type, storage});
      TensorFloat32 dst_tensor;
      ElementwiseOneInput operation =
          CreateElementwiseOneInput(op_def, OperationType::HARD_SWISH);
      ASSERT_OK(ExecuteGPUOperation(src_tensor, creation_context_, &operation,
                                    src_tensor.shape, &dst_tensor));
      EXPECT_THAT(
          dst_tensor.data,
          testing::Pointwise(testing::FloatNear(eps),
                             {0.0f, 0.0f, -0.375f, 0.0f, 1.125f, 3.f, 4.5f}));
    }
  }
}

TEST_F(OpenCLOperationTest, Log) {
  TensorFloat32 src_tensor;
  src_tensor.shape = BHWC(1, 2, 1, 2);
  src_tensor.data = {1.0f, 2.0f, 3.0f, 4.0f};

  for (auto storage : env_.GetSupportedStorages()) {
    for (auto precision : env_.GetSupportedPrecisions()) {
      const float eps = precision == CalculationsPrecision::F32 ? 1e-6f : 1e-3f;
      OperationDef op_def;
      op_def.precision = precision;
      auto data_type = DeduceDataTypeFromPrecision(precision);
      op_def.src_tensors.push_back({data_type, storage});
      op_def.dst_tensors.push_back({data_type, storage});
      TensorFloat32 dst_tensor;
      ElementwiseOneInput operation =
          CreateElementwiseOneInput(op_def, OperationType::LOG);
      ASSERT_OK(ExecuteGPUOperation(src_tensor, creation_context_, &operation,
                                    BHWC(1, 2, 1, 2), &dst_tensor));
      EXPECT_THAT(dst_tensor.data,
                  Pointwise(FloatNear(eps), {std::log(1.0f), std::log(2.0f),
                                             std::log(3.0f), std::log(4.0f)}));
    }
  }
}

TEST_F(OpenCLOperationTest, Rsqrt) {
  TensorFloat32 src_tensor;
  src_tensor.shape = BHWC(1, 2, 1, 2);
  src_tensor.data = {1.0f, 2.0f, 3.0f, 4.0f};

  for (auto storage : env_.GetSupportedStorages()) {
    for (auto precision : env_.GetSupportedPrecisions()) {
      const float eps = precision == CalculationsPrecision::F32 ? 1e-6f : 1e-3f;
      OperationDef op_def;
      op_def.precision = precision;
      auto data_type = DeduceDataTypeFromPrecision(precision);
      op_def.src_tensors.push_back({data_type, storage});
      op_def.dst_tensors.push_back({data_type, storage});
      TensorFloat32 dst_tensor;
      ElementwiseOneInput operation =
          CreateElementwiseOneInput(op_def, OperationType::RSQRT);
      ASSERT_OK(ExecuteGPUOperation(src_tensor, creation_context_, &operation,
                                    BHWC(1, 2, 1, 2), &dst_tensor));
      EXPECT_THAT(dst_tensor.data,
                  Pointwise(FloatNear(eps),
                            {1.0f / std::sqrt(1.0f), 1.0f / std::sqrt(2.0f),
                             1.0f / std::sqrt(3.0f), 1.0f / std::sqrt(4.0f)}));
    }
  }
}

TEST_F(OpenCLOperationTest, Sigmoid) {
  TensorFloat32 src_tensor;
  src_tensor.shape = BHWC(1, 2, 1, 2);
  src_tensor.data = {-std::log(1.0f), -std::log(2.0f), -std::log(3.0f),
                     -std::log(4.0f)};

  for (auto storage : env_.GetSupportedStorages()) {
    for (auto precision : env_.GetSupportedPrecisions()) {
      const float eps = precision == CalculationsPrecision::F32 ? 1e-6f : 1e-3f;
      OperationDef op_def;
      op_def.precision = precision;
      auto data_type = DeduceDataTypeFromPrecision(precision);
      op_def.src_tensors.push_back({data_type, storage});
      op_def.dst_tensors.push_back({data_type, storage});
      TensorFloat32 dst_tensor;
      ElementwiseOneInput operation =
          CreateElementwiseOneInput(op_def, OperationType::SIGMOID);
      ASSERT_OK(ExecuteGPUOperation(src_tensor, creation_context_, &operation,
                                    BHWC(1, 2, 1, 2), &dst_tensor));
      EXPECT_THAT(dst_tensor.data,
                  Pointwise(FloatNear(eps), {0.5f, 1.0f / 3.0f, 0.25f, 0.2f}));
    }
  }
}

TEST_F(OpenCLOperationTest, Sin) {
  TensorFloat32 src_tensor;
  src_tensor.shape = BHWC(1, 2, 1, 2);
  src_tensor.data = {0.0f, -1.0f, -0.05f, 0.045f};

  for (auto storage : env_.GetSupportedStorages()) {
    for (auto precision : env_.GetSupportedPrecisions()) {
      const float eps = precision == CalculationsPrecision::F32 ? 1e-6f : 1e-3f;
      OperationDef op_def;
      op_def.precision = precision;
      auto data_type = DeduceDataTypeFromPrecision(precision);
      op_def.src_tensors.push_back({data_type, storage});
      op_def.dst_tensors.push_back({data_type, storage});
      TensorFloat32 dst_tensor;
      ElementwiseOneInput operation =
          CreateElementwiseOneInput(op_def, OperationType::SIN);
      ASSERT_OK(ExecuteGPUOperation(src_tensor, creation_context_, &operation,
                                    BHWC(1, 2, 1, 2), &dst_tensor));
      EXPECT_THAT(
          dst_tensor.data,
          Pointwise(FloatNear(eps), {std::sin(0.0f), std::sin(-1.0f),
                                     std::sin(-0.05f), std::sin(0.045f)}));
    }
  }
}

TEST_F(OpenCLOperationTest, Sqrt) {
  TensorFloat32 src_tensor;
  src_tensor.shape = BHWC(1, 2, 1, 2);
  src_tensor.data = {1.0f, 2.0f, 3.0f, 4.0f};

  for (auto storage : env_.GetSupportedStorages()) {
    for (auto precision : env_.GetSupportedPrecisions()) {
      const float eps = precision == CalculationsPrecision::F32 ? 1e-6f : 1e-3f;
      OperationDef op_def;
      op_def.precision = precision;
      auto data_type = DeduceDataTypeFromPrecision(precision);
      op_def.src_tensors.push_back({data_type, storage});
      op_def.dst_tensors.push_back({data_type, storage});
      TensorFloat32 dst_tensor;
      ElementwiseOneInput operation =
          CreateElementwiseOneInput(op_def, OperationType::SQRT);
      ASSERT_OK(ExecuteGPUOperation(src_tensor, creation_context_, &operation,
                                    BHWC(1, 2, 1, 2), &dst_tensor));
      EXPECT_THAT(
          dst_tensor.data,
          Pointwise(FloatNear(eps), {std::sqrt(1.0f), std::sqrt(2.0f),
                                     std::sqrt(3.0f), std::sqrt(4.0f)}));
    }
  }
}

TEST_F(OpenCLOperationTest, Square) {
  TensorFloat32 src_tensor;
  src_tensor.shape = BHWC(1, 2, 1, 2);
  src_tensor.data = {1.0f, -2.0f, 3.0f, 4.0f};

  for (auto storage : env_.GetSupportedStorages()) {
    for (auto precision : env_.GetSupportedPrecisions()) {
      const float eps = precision == CalculationsPrecision::F32 ? 1e-6f : 1e-3f;
      OperationDef op_def;
      op_def.precision = precision;
      auto data_type = DeduceDataTypeFromPrecision(precision);
      op_def.src_tensors.push_back({data_type, storage});
      op_def.dst_tensors.push_back({data_type, storage});
      TensorFloat32 dst_tensor;
      ElementwiseOneInput operation =
          CreateElementwiseOneInput(op_def, OperationType::SQUARE);
      ASSERT_OK(ExecuteGPUOperation(src_tensor, creation_context_, &operation,
                                    BHWC(1, 2, 1, 2), &dst_tensor));
      EXPECT_THAT(dst_tensor.data,
                  Pointwise(FloatNear(eps), {1.0f, 4.0f, 9.0f, 16.0f}));
    }
  }
}

TEST_F(OpenCLOperationTest, Tanh) {
  TensorFloat32 src_tensor;
  src_tensor.shape = BHWC(1, 2, 1, 2);
  src_tensor.data = {1.0f, 2.0f, 3.0f, 4.0f};

  for (auto storage : env_.GetSupportedStorages()) {
    for (auto precision : env_.GetSupportedPrecisions()) {
      const float eps = precision == CalculationsPrecision::F32 ? 1e-6f : 1e-3f;
      OperationDef op_def;
      op_def.precision = precision;
      auto data_type = DeduceDataTypeFromPrecision(precision);
      op_def.src_tensors.push_back({data_type, storage});
      op_def.dst_tensors.push_back({data_type, storage});
      TensorFloat32 dst_tensor;
      ElementwiseOneInput operation =
          CreateElementwiseOneInput(op_def, OperationType::TANH);
      ASSERT_OK(ExecuteGPUOperation(src_tensor, creation_context_, &operation,
                                    BHWC(1, 2, 1, 2), &dst_tensor));
      EXPECT_THAT(
          dst_tensor.data,
          Pointwise(FloatNear(eps), {std::tanh(1.0f), std::tanh(2.0f),
                                     std::tanh(3.0f), std::tanh(4.0f)}));
    }
  }
}

TEST_F(OpenCLOperationTest, Sub) {
  TensorFloat32 src_tensor_0, src_tensor_1;
  src_tensor_0.shape = BHWC(1, 2, 1, 2);
  src_tensor_1.shape = BHWC(1, 2, 1, 2);
  src_tensor_0.data = {1.0f, 2.0f, 3.0f, 4.0f};
  src_tensor_1.data = {0.5f, 1.0f, 3.0f, 3.5f};

  for (auto storage : env_.GetSupportedStorages()) {
    for (auto precision : env_.GetSupportedPrecisions()) {
      const float eps = precision == CalculationsPrecision::F32 ? 1e-6f : 1e-3f;
      OperationDef op_def;
      op_def.precision = precision;
      auto data_type = DeduceDataTypeFromPrecision(precision);
      op_def.src_tensors.push_back({data_type, storage});
      op_def.src_tensors.push_back({data_type, storage});
      op_def.dst_tensors.push_back({data_type, storage});
      TensorFloat32 dst_tensor;
      ElementwiseTwoInput operation =
          CreateElementwiseTwoInput(op_def, OperationType::SUB);
      ASSERT_OK(ExecuteGPUOperation({src_tensor_0, src_tensor_1},
                                    creation_context_, &operation,
                                    BHWC(1, 2, 1, 2), &dst_tensor));
      EXPECT_THAT(dst_tensor.data,
                  Pointwise(FloatNear(eps), {0.5f, 1.0f, 0.0f, 0.5f}));
    }
  }
}

TEST_F(OpenCLOperationTest, SquaredDiff) {
  TensorFloat32 src_tensor_0, src_tensor_1;
  src_tensor_0.shape = BHWC(1, 2, 1, 2);
  src_tensor_1.shape = BHWC(1, 2, 1, 2);
  src_tensor_0.data = {1.0f, 2.0f, 3.0f, 4.0f};
  src_tensor_1.data = {0.5f, 1.0f, 3.0f, 3.5f};

  for (auto storage : env_.GetSupportedStorages()) {
    for (auto precision : env_.GetSupportedPrecisions()) {
      const float eps = precision == CalculationsPrecision::F32 ? 1e-6f : 1e-3f;
      OperationDef op_def;
      op_def.precision = precision;
      auto data_type = DeduceDataTypeFromPrecision(precision);
      op_def.src_tensors.push_back({data_type, storage});
      op_def.src_tensors.push_back({data_type, storage});
      op_def.dst_tensors.push_back({data_type, storage});
      TensorFloat32 dst_tensor;
      ElementwiseTwoInput operation =
          CreateElementwiseTwoInput(op_def, OperationType::SQUARED_DIFF);
      ASSERT_OK(ExecuteGPUOperation({src_tensor_0, src_tensor_1},
                                    creation_context_, &operation,
                                    BHWC(1, 2, 1, 2), &dst_tensor));
      EXPECT_THAT(dst_tensor.data,
                  Pointwise(FloatNear(eps), {0.25f, 1.0f, 0.0f, 0.25f}));
    }
  }
}

TEST_F(OpenCLOperationTest, Div) {
  TensorFloat32 src_tensor_0, src_tensor_1;
  src_tensor_0.shape = BHWC(1, 2, 1, 2);
  src_tensor_1.shape = BHWC(1, 2, 1, 2);
  src_tensor_0.data = {1.0f, 2.0f, 3.0f, 4.5f};
  src_tensor_1.data = {0.5f, 1.0f, 3.0f, 1.5f};

  for (auto storage : env_.GetSupportedStorages()) {
    for (auto precision : env_.GetSupportedPrecisions()) {
      const float eps = precision == CalculationsPrecision::F32 ? 1e-6f : 1e-2f;
      OperationDef op_def;
      op_def.precision = precision;
      auto data_type = DeduceDataTypeFromPrecision(precision);
      op_def.src_tensors.push_back({data_type, storage});
      op_def.src_tensors.push_back({data_type, storage});
      op_def.dst_tensors.push_back({data_type, storage});
      TensorFloat32 dst_tensor;
      ElementwiseTwoInput operation =
          CreateElementwiseTwoInput(op_def, OperationType::DIV);
      ASSERT_OK(ExecuteGPUOperation({src_tensor_0, src_tensor_1},
                                    creation_context_, &operation,
                                    BHWC(1, 2, 1, 2), &dst_tensor));
      EXPECT_THAT(dst_tensor.data,
                  Pointwise(FloatNear(eps), {2.0f, 2.0f, 1.0f, 3.0f}));
    }
  }
}

TEST_F(OpenCLOperationTest, Pow) {
  TensorFloat32 src_tensor_0, src_tensor_1;
  src_tensor_0.shape = BHWC(1, 2, 1, 2);
  src_tensor_1.shape = BHWC(1, 2, 1, 2);
  src_tensor_0.data = {6.0f, 7.0f, 4.0f, 2.0f};
  src_tensor_1.data = {0.0f, 1.0f, 2.0f, 3.0f};

  for (auto storage : env_.GetSupportedStorages()) {
    for (auto precision : env_.GetSupportedPrecisions()) {
      const float eps = precision == CalculationsPrecision::F32 ? 1e-6f : 1e-2f;
      OperationDef op_def;
      op_def.precision = precision;
      auto data_type = DeduceDataTypeFromPrecision(precision);
      op_def.src_tensors.push_back({data_type, storage});
      op_def.src_tensors.push_back({data_type, storage});
      op_def.dst_tensors.push_back({data_type, storage});
      TensorFloat32 dst_tensor;
      ElementwiseTwoInput operation =
          CreateElementwiseTwoInput(op_def, OperationType::POW);
      ASSERT_OK(ExecuteGPUOperation({src_tensor_0, src_tensor_1},
                                    creation_context_, &operation,
                                    BHWC(1, 2, 1, 2), &dst_tensor));
      EXPECT_THAT(dst_tensor.data,
                  Pointwise(FloatNear(eps), {1.0f, 7.0f, 16.0f, 8.0f}));
    }
  }
}

}  // namespace
}  // namespace cl
}  // namespace gpu
}  // namespace tflite
