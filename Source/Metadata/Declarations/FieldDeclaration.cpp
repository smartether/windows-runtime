#include "Metadata-Prefix.h"
#include "FieldDeclaration.h"

namespace NativeScript {
namespace Metadata {

    using namespace std;
    using namespace Microsoft::WRL;

    FieldDeclaration::FieldDeclaration(DeclarationKind kind, IMetaDataImport2* metadata, mdFieldDef token)
        : Base(kind)
        , _metadata{ metadata }
        , _token{ token } {

        ASSERT(metadata);
        ASSERT(TypeFromToken(token) == mdtFieldDef);
        ASSERT(token != mdFieldDefNil);
    }

    wstring FieldDeclaration::name() const {
        return fullName();
    }

    wstring FieldDeclaration::fullName() const {
        identifier nameData;
        ULONG nameLength{ 0 };

        ASSERT_SUCCESS(_metadata->GetFieldProps(_token, nullptr, nameData.data(), nameData.size(), &nameLength, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr));

        wstring result{ nameData.data(), nameLength - 1 };
        return result;
    }
}
}
