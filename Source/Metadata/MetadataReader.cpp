#include "Metadata-Prefix.h"
#include "MetadataReader.h"

const wchar_t* const WINDOWS_W{ L"Windows" };
const wchar_t* const SYSTEM_ENUM_W{ L"System.Enum" };
const wchar_t* const SYSTEM_VALUETYPE_W{ L"System.ValueType" };
const wchar_t* const SYSTEM_MULTICASTDELEGATE_W{ L"System.MulticastDelegate" };

namespace NativeScript {
namespace Metadata {

    using namespace std;
    using namespace Microsoft::WRL::Wrappers;
    using namespace Microsoft::WRL;

    // TODO: Create HCorenum wrapper

    // TODO: Use decl_iterator, specific_decl_iterator, filtered_decl_iterator

    shared_ptr<Declaration> MetadataReader::findByName(const wchar_t* fullName) const {
        HStringReference fullNameRef{ fullName };
        return findByName(fullNameRef.Get());
    }

    shared_ptr<Declaration> MetadataReader::findByName(HSTRING fullName) const {
        if (WindowsGetStringLen(fullName) == 0) {
            return make_shared<NamespaceDeclaration>(L"");
        }

        ComPtr<IMetaDataImport2> metadata;
        mdTypeDef token{ mdTokenNil };

        HRESULT getMetadataFileResult{ RoGetMetaDataFile(fullName, nullptr, nullptr, metadata.GetAddressOf(), &token) };

        if (FAILED(getMetadataFileResult)) {
            if (getMetadataFileResult == RO_E_METADATA_NAME_IS_NAMESPACE) {
                return make_shared<NamespaceDeclaration>(WindowsGetStringRawBuffer(fullName, nullptr));
            }

            return nullptr;
        }

        DWORD flags{ 0 };
        mdToken parentToken{ mdTokenNil };
        ASSERT_SUCCESS(metadata->GetTypeDefProps(token, nullptr, 0, nullptr, &flags, &parentToken));

        if (IsTdClass(flags)) {
            identifier parentName;

            switch (TypeFromToken(parentToken)) {
            case mdtTypeDef: {
                ASSERT_SUCCESS(metadata->GetTypeDefProps(parentToken, parentName.data(), parentName.size(), nullptr, nullptr, nullptr));
                break;
            }

            case mdtTypeRef: {
                ASSERT_SUCCESS(metadata->GetTypeRefProps(parentToken, nullptr, parentName.data(), parentName.size(), nullptr));
                break;
            }

            default:
                ASSERT_NOT_REACHED();
            }

            if (wcscmp(parentName.data(), SYSTEM_ENUM_W) == 0) {
                return make_shared<EnumDeclaration>(metadata.Get(), token);
            }

            if (wcscmp(parentName.data(), SYSTEM_VALUETYPE_W) == 0) {
                return make_shared<StructDeclaration>(metadata.Get(), token);
            }

            if (wcscmp(parentName.data(), SYSTEM_MULTICASTDELEGATE_W) == 0) {
                if (wcsstr(WindowsGetStringRawBuffer(fullName, nullptr), L"`")) {
                    return make_shared<GenericDelegateDeclaration>(metadata.Get(), token);
                } else {
                    return make_shared<DelegateDeclaration>(metadata.Get(), token);
                }
            }

            return make_shared<ClassDeclaration>(metadata.Get(), token);
        }

        if (IsTdInterface(flags)) {
            if (wcsstr(WindowsGetStringRawBuffer(fullName, nullptr), L"`")) {
                return make_shared<GenericInterfaceDeclaration>(metadata.Get(), token);
            } else {
                return make_shared<InterfaceDeclaration>(metadata.Get(), token);
            }
        }

        ASSERT_NOT_REACHED();
    }
}
}
