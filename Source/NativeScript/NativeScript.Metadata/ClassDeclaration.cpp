#include "pch.h"
#include "ClassDeclaration.h"

namespace NativeScript {
namespace Metadata {

using namespace std;
using namespace Microsoft::WRL;

// TODO
namespace {

vector<InterfaceDeclaration> makeImplementedInterfacesDeclarations(IMetaDataImport2* metadata, mdTypeDef token) {
    HCORENUM enumerator{nullptr};
    ULONG count{0};
    array<mdInterfaceImpl, 1024> tokens;

    ASSERT_SUCCESS(metadata->EnumInterfaceImpls(&enumerator, token, tokens.data(), tokens.size(), &count));
    ASSERT(count < tokens.size() - 1);
    metadata->CloseEnum(enumerator);

    vector<InterfaceDeclaration> result;
    for (size_t i = 0; i < count; ++i) {
        mdToken interfaceToken{mdTokenNil};
        ASSERT_SUCCESS(metadata->GetInterfaceImplProps(tokens[i], nullptr, &interfaceToken));

        switch (TypeFromToken(interfaceToken)) {
            case mdtTypeDef: {
                result.emplace_back(metadata, interfaceToken);
                break;
            }

            case mdtTypeRef: {
                ComPtr<IMetaDataImport2> externalMetadata;
                mdTypeDef externalInterfaceToken{mdTypeDefNil};

                bool isResolved{resolveTypeRef(metadata, interfaceToken, externalMetadata.GetAddressOf(), &externalInterfaceToken)};
                ASSERT(isResolved);

                result.emplace_back(externalMetadata.Get(), externalInterfaceToken);
                break;
            }

            case mdtTypeSpec: {
                NOT_IMPLEMENTED();
            }

            default:
                ASSERT_NOT_REACHED();
        }
    }

    return result;
}

vector<MethodDeclaration> makeInitializerDeclarations(IMetaDataImport2* metadata, mdTypeDef token) {
    HCORENUM enumerator{nullptr};
    ULONG count{0};
    array<mdProperty, 1024> tokens;

    ASSERT_SUCCESS(metadata->EnumMethodsWithName(&enumerator, token, COR_CTOR_METHOD_NAME_W, tokens.data(), tokens.size(), &count));
    ASSERT(count < tokens.size() - 1);
    metadata->CloseEnum(enumerator);

    vector<MethodDeclaration> result;
    for (size_t i = 0; i < count; ++i) {
        result.emplace_back(metadata, tokens[i]);
    }

    return result;
}

}

ClassDeclaration::ClassDeclaration(IMetaDataImport2* metadata, mdTypeDef token)
    : Base(metadata, token)
    , _implementedInterfaces(makeImplementedInterfacesDeclarations(metadata, token))
    , _initializers(makeInitializerDeclarations(metadata, token)) {

}

wstring ClassDeclaration::baseFullName() const {
    identifier parentName;
    ULONG parentNameLength;

    mdToken parentToken{mdTokenNil};
    ASSERT_SUCCESS(_metadata->GetTypeDefProps(_token, nullptr, 0, nullptr, nullptr, &parentToken));

    switch (TypeFromToken(parentToken)) {
        case mdtTypeDef: {
            ASSERT_SUCCESS(_metadata->GetTypeDefProps(parentToken, parentName.data(), parentName.size(), &parentNameLength, nullptr, nullptr));
            break;
        }

        case mdtTypeRef: {
            ASSERT_SUCCESS(_metadata->GetTypeRefProps(parentToken, nullptr, parentName.data(), parentName.size(), &parentNameLength));
            break;
        }

        default:
            ASSERT_NOT_REACHED();
    }

    wstring result{parentName.data(), parentNameLength - 1};
    return result;
}

// TODO: Abstract class
ClassType ClassDeclaration::classType() {
    HRESULT isComposable{_metadata->GetCustomAttributeByName(_token, COMPOSABLE_ATTRIBUTE_W, nullptr, nullptr)};
    ASSERT_SUCCESS(isComposable);
    if (isComposable == S_OK) {
        return ClassType::Subclassable;
    }

    HRESULT isInstantiable{_metadata->GetCustomAttributeByName(_token, ACTIVATABLE_ATTRIBUTE_W, nullptr, nullptr)};
    ASSERT_SUCCESS(isInstantiable);
    if (isInstantiable == S_OK) {
        return ClassType::Instantiable;
    }

    return ClassType::Uninstantiable;
}

IteratorRange<ClassDeclaration::InterfaceIterator> ClassDeclaration::implementedInterfaces() const {
    return IteratorRange<InterfaceIterator>(_implementedInterfaces.begin(), _implementedInterfaces.end());
}

IteratorRange<ClassDeclaration::MethodIterator> ClassDeclaration::initializers() const {
    return IteratorRange<MethodIterator>(_initializers.begin(), _initializers.end());
}

}
}
