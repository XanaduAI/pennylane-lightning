#include "Constant.hpp"
#include "ConstantUtil.hpp"
#include "KernelMap.hpp"
#include "Util.hpp"

#include <catch2/catch.hpp>

using namespace Pennylane;
using namespace Pennylane::KernelMap;

TEST_CASE("Test PriorityDispatchSet", "[PriorityDispatchSet]") {
    using Catch::Matchers::Contains;

    auto pds = PriorityDispatchSet();
    pds.emplace(10u, Util::IntegerInterval<size_t>(10, 20),
                Gates::KernelType::PI);

    SECTION("Test conflict") {
        /* If two elements has the same priority but integer intervals overlap,
         * they conflict. */
        REQUIRE(pds.conflict(10u, Util::IntegerInterval<size_t>(19, 23)));
    }

    SECTION("Get Kernel") {
        REQUIRE(pds.getKernel(15) == Gates::KernelType::PI);
        REQUIRE_THROWS_WITH(pds.getKernel(30),
                            Contains("Cannot find a kernel"));
    }
}

TEST_CASE("Test default kernels for gates are well defined", "[KernelMap]") {
    auto &instance = OperationKernelMap<Gates::GateOperation>::getInstance();
    Util::for_each_enum<Threading, CPUMemoryModel>(
        [&instance](Threading threading, CPUMemoryModel memory_model) {
            for (size_t num_qubits = 1; num_qubits < 27; num_qubits++) {
                REQUIRE_NOTHROW(
                    instance.getKernelMap(num_qubits, threading, memory_model));
            }
        });
}

TEST_CASE("Test default kernels for generators are well defined",
          "[KernelMap]") {
    auto &instance =
        OperationKernelMap<Gates::GeneratorOperation>::getInstance();
    Util::for_each_enum<Threading, CPUMemoryModel>(
        [&instance](Threading threading, CPUMemoryModel memory_model) {
            for (size_t num_qubits = 1; num_qubits < 27; num_qubits++) {
                REQUIRE_NOTHROW(
                    instance.getKernelMap(num_qubits, threading, memory_model));
            }
        });
}

TEST_CASE("Test default kernels for matrix operation are well defined",
          "[KernelMap]") {
    auto &instance = OperationKernelMap<Gates::MatrixOperation>::getInstance();
    Util::for_each_enum<Threading, CPUMemoryModel>(
        [&instance](Threading threading, CPUMemoryModel memory_model) {
            for (size_t num_qubits = 1; num_qubits < 27; num_qubits++) {
                REQUIRE_NOTHROW(
                    instance.getKernelMap(num_qubits, threading, memory_model));
            }
        });
}

TEST_CASE("Test unallowed kernel", "[KernelMap]") {
    using Gates::GateOperation;
    using Gates::KernelType;
    auto &instance = OperationKernelMap<Gates::GateOperation>::getInstance();
    REQUIRE_THROWS(instance.assignKernelForOp(
        GateOperation::PauliX, Threading::SingleThread,
        CPUMemoryModel::Unaligned, 0, Util::full_domain<size_t>(),
        KernelType::None));
}

TEST_CASE("Test several limiting cases of default kernels", "[KernelMap]") {
    auto &instance = OperationKernelMap<Gates::GateOperation>::getInstance();
    SECTION("Single thread, large number of qubits") {
        // For large N, single thread calls "LM" for all single- and two-qubit
        // gates. For three-qubit gates, we use PI.
        auto gate_map = instance.getKernelMap(24, Threading::SingleThread,
                                              CPUMemoryModel::Unaligned);
        Util::for_each_enum<Gates::GateOperation>(
            [&gate_map](Gates::GateOperation gate_op) {
                INFO(Util::lookup(Gates::Constant::gate_names, gate_op));
                if (gate_op == Gates::GateOperation::MultiRZ) {
                    REQUIRE(gate_map[gate_op] == Gates::KernelType::LM);
                } else if (Util::lookup(Gates::Constant::gate_wires, gate_op) !=
                           3) {
                    REQUIRE(gate_map[gate_op] == Gates::KernelType::LM);
                } else {
                    REQUIRE(gate_map[gate_op] == Gates::KernelType::PI);
                }
            });
    }
    SECTION("Single thread, N = 14") {
        // For large N = 14, IsingXX with "PI" is slightly faster
        auto gate_map = instance.getKernelMap(14, Threading::SingleThread,
                                              CPUMemoryModel::Unaligned);
        REQUIRE(gate_map[Gates::GateOperation::IsingXX] ==
                Gates::KernelType::PI);
    }
}

TEST_CASE("Test assignKernelForOp", "[KernelMap]") {
    using Gates::GateOperation;
    using Gates::KernelType;
    auto &instance = OperationKernelMap<Gates::GateOperation>::getInstance();
    SECTION("Test priority works") {
        auto original_kernel = instance.getKernelMap(
            24, Threading::SingleThread,
            CPUMemoryModel::Unaligned)[GateOperation::PauliX];

        instance.assignKernelForOp(GateOperation::PauliX,
                                   Threading::SingleThread,
                                   CPUMemoryModel::Unaligned, 100,
                                   Util::full_domain<size_t>(), KernelType::PI);

        REQUIRE(instance.getKernelMap(
                    24, Threading::SingleThread,
                    CPUMemoryModel::Unaligned)[GateOperation::PauliX] ==
                KernelType::PI);

        instance.removeKernelForOp(GateOperation::PauliX,
                                   Threading::SingleThread,
                                   CPUMemoryModel::Unaligned, 100);
        REQUIRE(instance.getKernelMap(
                    24, Threading::SingleThread,
                    CPUMemoryModel::Unaligned)[GateOperation::PauliX] ==
                original_kernel);
    }
}
