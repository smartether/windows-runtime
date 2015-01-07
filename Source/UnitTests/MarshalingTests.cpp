#include "pch.h"

namespace NativeScript {
namespace UnitTests {

TEST_CLASS(MarshalingTests) {
public:
    TEST_METHOD(InOutParameter) {
        MetadataReader metadataReader;

        const wchar_t* name{L"NativeScript.TestFixtures.MarshalingClass"};
        shared_ptr<ClassDeclaration> declaration{static_pointer_cast<ClassDeclaration>(metadataReader.findByName(name))};
        MethodDeclaration method{declaration->findMethodsWithName(L"InOutParameterMethod")[0]};
        IteratorRange<MethodDeclaration::ParameterIterator> parameters{method.parameters()};

        MethodDeclaration::ParameterIterator it{parameters.begin()};
        const ParameterDeclaration& inParameter{*it};
        Assert::IsTrue(inParameter.name() == L"inParameter");
        Assert::IsTrue(inParameter.fullName() == L"inParameter");
        Assert::IsTrue(inParameter.isOut() == false);

        ++it;
        const ParameterDeclaration& outParameter{*it};
        Assert::IsTrue(outParameter.name() == L"outParameter");
        Assert::IsTrue(outParameter.fullName() == L"outParameter");
        Assert::IsTrue(outParameter.isOut() == true);

        ++it;
        Assert::IsTrue(it == parameters.end());
    }
};

}
}
