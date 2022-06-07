// Copyright 2021 Xanadu Quantum Technologies Inc.

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
/**
 * @file
 */
#include "StateVecAdjDiff.hpp"

// explicit template instantiations
template void Pennylane::Algorithms::statevectorVJP<float>(
    std::vector<std::complex<float>> &jac, const JacobianData<float> &jd,
    const std::complex<float> *dy, size_t dy_size, bool apply_operations);

template void Pennylane::Algorithms::statevectorVJP<double>(
    std::vector<std::complex<double>> &jac, const JacobianData<double> &jd,
    const std::complex<double> *dy, size_t dy_size, bool apply_operations);