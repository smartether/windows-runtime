#pragma once

#include <vector>

#include "TypeDeclaration.h"
#include "MethodDeclaration.h"
#include "PropertyDeclaration.h"
#include "EventDeclaration.h"
#include "Wrappers/IteratorRange.h"

namespace NativeScript {
namespace Metadata {

    class InterfaceDeclaration;

    class BaseClassDeclaration : public TypeDeclaration {
    public:
        typedef TypeDeclaration Base;

        // TODO: Remove smart pointer from here
        using InterfaceIterator = std::vector<std::unique_ptr<const InterfaceDeclaration>>::const_iterator;

        using MethodIterator = std::vector<MethodDeclaration>::const_iterator;

        using PropertyIterator = std::vector<PropertyDeclaration>::const_iterator;

        using EventIterator = std::vector<EventDeclaration>::const_iterator;

        IteratorRange<InterfaceIterator> implementedInterfaces() const;

        IteratorRange<MethodIterator> methods() const;

        IteratorRange<PropertyIterator> properties() const;

        IteratorRange<EventIterator> events() const;

        std::vector<std::unique_ptr<const Declaration>> findMembersWithName(const wchar_t*) const;

        std::vector<MethodDeclaration> findMethodsWithName(const wchar_t*) const;

    protected:
        explicit BaseClassDeclaration(DeclarationKind, IMetaDataImport2*, mdTypeDef);

    private:
        std::vector<std::unique_ptr<const InterfaceDeclaration>> _implementedInterfaces;

        std::vector<MethodDeclaration> _methods;

        std::vector<PropertyDeclaration> _properties;

        std::vector<EventDeclaration> _events;
    };
}
}
