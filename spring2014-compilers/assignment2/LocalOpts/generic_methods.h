using namespace llvm;

namespace {
    /* Arbitrary precision constant values. To take care of both 32 bit and 64 bit numbers. */
    template <typename APType> APType getZeroOne(unsigned num_bits, int zeroOne);
    template <> APInt   getZeroOne(unsigned num_bits, int zeroOne) { return APInt(num_bits,zeroOne); }

    /* Compare value based on the type values to identify algebraic identities */
    /* generalize compare method based on type */
    template <typename ConstantType, typename APType> bool id_compare(ConstantType& cint, APType zero);
    template <> bool id_compare(ConstantInt& cint, APInt zero) { return cint.getValue().eq(zero); }

}
