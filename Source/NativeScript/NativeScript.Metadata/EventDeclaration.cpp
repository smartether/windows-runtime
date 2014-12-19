#include "pch.h"
#include "EventDeclaration.h"

namespace NativeScript {
namespace Metadata {

using namespace std;

using namespace Microsoft::WRL::Wrappers;
using namespace Microsoft::WRL;

EventDeclaration::EventDeclaration(IMetaDataImport2* metadata, mdEvent token)
    : _metadata{metadata}
    , _token{token} {

    ASSERT(token != mdEventNil);
}

bool EventDeclaration::isExported() const {
    DWORD flags{0};
    ASSERT_SUCCESS(_metadata->GetEventProps(_token, nullptr, nullptr, 0, nullptr, &flags, nullptr, nullptr, nullptr, nullptr, nullptr, 0, nullptr));

    if (IsEvSpecialName(flags)) {
        return false;
    }

    return true;
}

wstring EventDeclaration::name() const {
    return fullName();
}

wstring EventDeclaration::fullName() const {
    identifier nameData;
    ULONG nameDataLength{0};

    ASSERT_SUCCESS(_metadata->GetEventProps(_token, nullptr, nameData.data(), nameData.size(), &nameDataLength, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, 0, nullptr));

    wstring name{nameData.data(), nameDataLength - 1};
    return name;
}

DelegateDeclaration EventDeclaration::type() const {
    mdToken delegateToken{mdTokenNil};

    ASSERT_SUCCESS(_metadata->GetEventProps(_token, nullptr, nullptr, 0, nullptr, nullptr, &delegateToken, nullptr, nullptr, nullptr, nullptr, 0, nullptr));

    switch (TypeFromToken(delegateToken)) {
        case mdtTypeDef: {
            return DelegateDeclaration(_metadata.Get(), delegateToken);
        }

        case mdtTypeRef: {
            identifier nameData;
            ASSERT_SUCCESS(_metadata->GetTypeRefProps(delegateToken, nullptr, nameData.data(), nameData.size(), nullptr));

            ComPtr<IMetaDataImport2> externalMetadata;
            mdTypeDef externalDelegateToken{mdTypeDefNil};
            ASSERT_SUCCESS(RoGetMetaDataFile(HStringReference(nameData.data()).Get(), nullptr, nullptr, externalMetadata.GetAddressOf(), &externalDelegateToken));

            return DelegateDeclaration(externalMetadata.Get(), externalDelegateToken);
        }

        case mdtTypeSpec: {
            // PCCOR_SIGNATURE signature{nullptr};
            // ULONG signatureSize{0};
            // ASSERT_SUCCESS(_metadata->GetTypeSpecFromToken(delegateToken, &signature, &signatureSize));
            //
            // ULONG type1{CorSigUncompressData(signature)};
            // ASSERT(type1 == ELEMENT_TYPE_GENERICINST);
            //
            // ULONG type2{CorSigUncompressData(signature)};
            // ASSERT(type2 == ELEMENT_TYPE_CLASS);
            //
            // mdToken openGenericDelegateToken{CorSigUncompressToken(signature)};
            // switch (TypeFromToken(openGenericDelegateToken)) {
            //     case mdtTypeDef:
            //         break;
            //
            //     case mdtTypeRef:
            //         break;
            //
            //     default:
            //         ASSERT_NOT_REACHED();
            // }
            //
            // ULONG numberOfGenericParameters{CorSigUncompressData(signature)};

            NOT_IMPLEMENTED();
        }

        default:
            ASSERT_NOT_REACHED();
    }
}

MethodDeclaration EventDeclaration::addMethod() const {
    mdMethodDef addMethodToken{mdMethodDefNil};
    ASSERT_SUCCESS(_metadata->GetEventProps(_token, nullptr, nullptr, 0, nullptr, nullptr, nullptr, &addMethodToken, nullptr, nullptr, nullptr, 0, nullptr));

    return MethodDeclaration{_metadata.Get(), addMethodToken};
}

MethodDeclaration EventDeclaration::removeMethod() const {
    mdMethodDef removeMethodToken{mdMethodDefNil};
    ASSERT_SUCCESS(_metadata->GetEventProps(_token, nullptr, nullptr, 0, nullptr, nullptr, nullptr, nullptr, &removeMethodToken, nullptr, nullptr, 0, nullptr));

    return MethodDeclaration{_metadata.Get(), removeMethodToken};
}

}
}
