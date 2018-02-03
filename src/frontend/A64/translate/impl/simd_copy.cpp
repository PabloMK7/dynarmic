/* This file is part of the dynarmic project.
 * Copyright (c) 2018 MerryMage
 * This software may be used and distributed according to the terms of the GNU
 * General Public License version 2 or any later version.
 */

#include "common/bit_util.h"
#include "frontend/A64/translate/impl/impl.h"

namespace Dynarmic::A64 {

bool TranslatorVisitor::DUP_gen(bool Q, Imm<5> imm5, Reg Rn, Vec Vd) {
    const size_t size = Common::LowestSetBit(imm5.ZeroExtend());
    if (size > 3) return UnallocatedEncoding();
    if (size == 3 && !Q) return ReservedValue();
    const size_t esize = 8 << size;
    const size_t datasize = Q ? 128 : 64;

    const IR::UAny element = X(esize, Rn);

    const IR::U128 result = [&]{
        switch (esize) {
        case 8:
            return Q ? ir.VectorBroadcast8(element) : ir.VectorLowerBroadcast8(element);
        case 16:
            return Q ? ir.VectorBroadcast16(element) : ir.VectorLowerBroadcast16(element);
        case 32:
            return Q ? ir.VectorBroadcast32(element) : ir.VectorLowerBroadcast32(element);
        default:
            return ir.VectorBroadcast64(element);
        }
    }();

    V(datasize, Vd, result);

    return true;
}

bool TranslatorVisitor::SMOV(bool Q, Imm<5> imm5, Vec Vn, Reg Rd) {
    const size_t size = Common::LowestSetBit(imm5.ZeroExtend());
    if (size == 2 && !Q) return UnallocatedEncoding();
    if (size > 2) return UnallocatedEncoding();

    const size_t idxdsize = imm5.Bit<4>() ? 128 : 64;
    const size_t index = imm5.ZeroExtend<size_t>() >> (size + 1);
    const size_t esize = 8 << size;
    const size_t datasize = Q ? 64 : 32;

    const IR::U128 operand = V(idxdsize, Vn);

    const IR::UAny elem = ir.VectorGetElement(esize, operand, index);
    X(datasize, Rd, SignExtend(elem, datasize));

    return true;
}

bool TranslatorVisitor::UMOV(bool Q, Imm<5> imm5, Vec Vn, Reg Rd) {
    const size_t size = Common::LowestSetBit(imm5.ZeroExtend());
    if (size < 3 && Q) return UnallocatedEncoding();
    if (size == 3 && !Q) return UnallocatedEncoding();
    if (size > 3) return UnallocatedEncoding();

    const size_t idxdsize = imm5.Bit<4>() ? 128 : 64;
    const size_t index = imm5.ZeroExtend<size_t>() >> (size + 1);
    const size_t esize = 8 << size;
    const size_t datasize = Q ? 64 : 32;

    const IR::U128 operand = V(idxdsize, Vn);

    const IR::UAny elem = ir.VectorGetElement(esize, operand, index);
    X(datasize, Rd, ZeroExtend(elem, datasize));

    return true;
}

bool TranslatorVisitor::INS_gen(Imm<5> imm5, Reg Rn, Vec Vd) {
    const size_t size = Common::LowestSetBit(imm5.ZeroExtend());
    if (size > 3) return UnallocatedEncoding();

    const size_t index = imm5.ZeroExtend<size_t>() >> (size + 1);
    const size_t esize = 8 << size;
    const size_t datasize = 128;

    const IR::UAny element = X(esize, Rn);
    const IR::U128 result = ir.VectorSetElement(esize, V(datasize, Vd), index, element);
    V(datasize, Vd, result);

    return true;
}

bool TranslatorVisitor::INS_elt(Imm<5> imm5, Imm<4> imm4, Vec Vn, Vec Vd) {
    const size_t size = Common::LowestSetBit(imm5.ZeroExtend());
    if (size > 3) return UnallocatedEncoding();

    const size_t dst_index = imm5.ZeroExtend<size_t>() >> (size + 1);
    const size_t src_index = imm4.ZeroExtend<size_t>() >> size;
    const size_t idxdsize = imm4.Bit<3>() ? 128 : 64;
    const size_t esize = 8 << size;

    const IR::U128 operand = V(idxdsize, Vn);
    const IR::UAny elem = ir.VectorGetElement(esize, operand, src_index);
    const IR::U128 result = ir.VectorSetElement(esize, V(128, Vd), dst_index, elem);
    V(128, Vd, result);

    return true;
}

} // namespace Dynarmic::A64
