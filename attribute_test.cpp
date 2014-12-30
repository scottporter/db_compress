#include "attribute.h"
#include "base.h"

#include <iostream>
#include <memory>

namespace db_compress {

void Test() {
    RegisterAttrValueCreator(0, new IntegerAttrValueCreator(), BASE_TYPE_INTEGER);
    AttrValue* attr = GetAttrValueCreator(0)->GetAttrValue(109);
    int val = static_cast<IntegerAttrValue*>(attr)->Value();
    if (val != 109) 
        std::cerr << "Attribute Unit Test Failed!\n";
    if (GetBaseType(0) != BASE_TYPE_INTEGER)
        std::cerr << "Attribute Unit Test Failed!\n";
}

}  // namespace db_compress

int main() {
    db_compress::Test();
}
