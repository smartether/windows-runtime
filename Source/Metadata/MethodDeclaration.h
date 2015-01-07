#pragma once

#include <vector>
#include <memory>

#include <cor.h>
#include <wrl.h>

#include "Declaration.h"
#include "ParameterDeclaration.h"
#include "IteratorRange.h"

namespace NativeScript {
namespace Metadata {

class InterfaceDeclaration;

class MethodDeclaration final : public Declaration {
public:
    typedef Declaration Base;

    using ParameterIterator = std::vector<const ParameterDeclaration>::const_iterator;

    explicit MethodDeclaration(IMetaDataImport2*, mdMethodDef);

    virtual bool isExported() const override;

    virtual std::wstring name() const override;

    virtual std::wstring fullName() const override;

    bool isInitializer() const;

    bool isStatic() const;

    bool isSealed() const;

    // TODO: Split the signature and add it's elements to the ParameterDeclarations
    PCCOR_SIGNATURE signature() const;

    IteratorRange<ParameterIterator> parameters() const;

    size_t numberOfParameters() const;

    std::wstring overloadName() const;

    bool isDefaultOverload() const;

private:
    friend std::unique_ptr<const InterfaceDeclaration> findDeclaringInterfaceForMethod(const MethodDeclaration&, size_t* outIndex);

    const Microsoft::WRL::ComPtr<IMetaDataImport2> _metadata;
    const mdMethodDef _token;

    std::vector<ParameterDeclaration> _parameters;
};

}
}
