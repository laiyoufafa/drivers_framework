/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "ast/ast_string_type.h"

namespace OHOS {
namespace HDI {
bool ASTStringType::IsStringType()
{
    return true;
}

String ASTStringType::ToString()
{
    return "String";
}

TypeKind ASTStringType::GetTypeKind()
{
    return TypeKind::TYPE_STRING;
}

String ASTStringType::EmitCType(TypeMode mode) const
{
    switch (mode) {
        case TypeMode::NO_MODE:
            return "char*";
        case TypeMode::PARAM_IN:
            return "const char*";
        case TypeMode::PARAM_OUT:
            return "char*";
        case TypeMode::LOCAL_VAR:
            return "char*";
        default:
            return "unknow type";
    }
}

String ASTStringType::EmitCppType(TypeMode mode) const
{
    switch (mode) {
        case TypeMode::NO_MODE:
            return "std::string";
        case TypeMode::PARAM_IN:
            return "const std::string&";
        case TypeMode::PARAM_OUT:
            return "std::string&";
        case TypeMode::LOCAL_VAR:
            return "std::string";
        default:
            return "unknow type";
    }
}

String ASTStringType::EmitJavaType(TypeMode mode, bool isInnerType) const
{
    return "String";
}

void ASTStringType::EmitCWriteVar(const String& parcelName, const String& name, const String& ecName,
    const String& gotoLabel, StringBuilder& sb, const String& prefix) const
{
    sb.Append(prefix).AppendFormat("if (!HdfSbufWriteString(%s, %s)) {\n",
        parcelName.string(), name.string());
    sb.Append(prefix + g_tab).AppendFormat(
        "HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n", name.string());
    sb.Append(prefix + g_tab).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", ecName.string());
    sb.Append(prefix + g_tab).AppendFormat("goto %s;\n", gotoLabel.string());
    sb.Append(prefix).Append("}\n");
}

void ASTStringType::EmitCProxyWriteOutVar(const String& parcelName, const String& name, const String& ecName,
    const String& gotoLabel, StringBuilder& sb, const String& prefix) const
{
    String lenName = String::Format("%sLen", name.string());
    sb.Append(prefix).AppendFormat("if (!HdfSbufWriteUint32(%s, %s)) {\n",
        parcelName.string(), lenName.string());
    sb.Append(prefix + g_tab).AppendFormat(
        "HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n", name.string());
    sb.Append(prefix + g_tab).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", ecName.string());
    sb.Append(prefix + g_tab).AppendFormat("goto %s;\n", gotoLabel.string());
    sb.Append(prefix).Append("}\n");
}

void ASTStringType::EmitCProxyReadVar(const String& parcelName, const String& name, bool isInnerType,
    const String& ecName, const String& gotoLabel, StringBuilder& sb, const String& prefix) const
{
    sb.Append(prefix).AppendFormat("const char *%s = HdfSbufReadString(%s);\n",
        name.string(), parcelName.string());
    sb.Append(prefix).AppendFormat("if (%s == NULL) {\n", name.string());
    sb.Append(prefix + g_tab).AppendFormat(
        "HDF_LOGE(\"%%{public}s: read %s failed!\", __func__);\n", name.string());
    sb.Append(prefix + g_tab).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", ecName.string());
    sb.Append(prefix + g_tab).AppendFormat("goto %s;\n", gotoLabel.string());
    sb.Append(prefix).Append("}\n");
}

void ASTStringType::EmitCStubReadVar(const String& parcelName, const String& name, const String& ecName,
    const String& gotoLabel, StringBuilder& sb, const String& prefix) const
{
    sb.Append(prefix).AppendFormat("const char *%s = HdfSbufReadString(%s);\n",
        name.string(), parcelName.string());
    sb.Append(prefix).AppendFormat("if (%s == NULL) {\n", name.string());
    sb.Append(prefix + g_tab).AppendFormat(
        "HDF_LOGE(\"%%{public}s: read %s failed!\", __func__);\n", name.string());
    sb.Append(prefix + g_tab).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", ecName.string());
    sb.Append(prefix + g_tab).AppendFormat("goto %s;\n", gotoLabel.string());
    sb.Append(prefix).Append("}\n");
}

void ASTStringType::EmitCStubReadOutVar(const String& parcelName, const String& name, const String& ecName,
    const String& gotoLabel, StringBuilder& sb, const String& prefix) const
{
    String lenName = String::Format("%sLen", name.string());
    sb.Append(prefix).AppendFormat("if (!HdfSbufReadUint32(%s, &%s)) {\n",
        parcelName.string(), lenName.string());
    sb.Append(prefix + g_tab).AppendFormat(
        "HDF_LOGE(\"%%{public}s: read %s size failed!\", __func__);\n", name.string());
    sb.Append(prefix + g_tab).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", ecName.string());
    sb.Append(prefix + g_tab).AppendFormat("goto %s;\n", gotoLabel.string());
    sb.Append(prefix).Append("}\n\n");

    sb.Append(prefix).AppendFormat("%s(%s, >, %s / sizeof(char), %s, HDF_ERR_INVALID_PARAM, %s);\n",
        CHECK_VALUE_RET_GOTO_MACRO, lenName.string(), MAX_BUFF_SIZE_MACRO, ecName.string(), gotoLabel.string());

    sb.Append(prefix).AppendFormat("if (%s > 0) {\n", lenName.string());
    sb.Append(prefix + g_tab).AppendFormat("%s = (%s)OsalMemCalloc(%s);\n", name.string(), EmitCType().string(),
        lenName.string());
    sb.Append(prefix + g_tab).AppendFormat("if (%s == NULL) {\n", name.string());
    sb.Append(prefix + g_tab + g_tab).AppendFormat("HDF_LOGE(\"%%{public}s: malloc %s failed\", __func__);\n",
        name.string());
    sb.Append(prefix + g_tab + g_tab).AppendFormat("%s = HDF_ERR_MALLOC_FAIL;\n", ecName.string());
    sb.Append(prefix + g_tab + g_tab).AppendFormat("goto %s;\n", gotoLabel.string());
    sb.Append(prefix + g_tab).Append("}\n");
    sb.Append(prefix).Append("}\n");
}

void ASTStringType::EmitCppWriteVar(const String& parcelName, const String& name, StringBuilder& sb,
    const String& prefix, unsigned int innerLevel) const
{
    sb.Append(prefix).AppendFormat("if (!%s.WriteString(%s)) {\n", parcelName.string(), name.string());
    sb.Append(prefix + g_tab).AppendFormat(
        "HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n", name.string());
    sb.Append(prefix + g_tab).Append("return HDF_ERR_INVALID_PARAM;\n");
    sb.Append(prefix).Append("}\n");
}

void ASTStringType::EmitCppReadVar(const String& parcelName, const String& name, StringBuilder& sb,
    const String& prefix, bool initVariable, unsigned int innerLevel) const
{
    if (initVariable) {
        sb.Append(prefix).AppendFormat("%s %s;\n", EmitCppType().string(), name.string());
    }

    sb.Append(prefix).AppendFormat("if (!%s.ReadString(%s)) {\n", parcelName.string(), name.string());
    sb.Append(prefix + g_tab).AppendFormat("HDF_LOGE(\"%%{public}s: read %s failed!\", __func__);\n", name.string());
    sb.Append(prefix + g_tab).Append("return HDF_ERR_INVALID_PARAM;\n");
    sb.Append(prefix).Append("}\n");
}

void ASTStringType::EmitCMarshalling(const String& name, StringBuilder& sb, const String& prefix) const
{
    sb.Append(prefix).AppendFormat("if (!HdfSbufWriteString(data, %s)) {\n", name.string());
    sb.Append(prefix + g_tab).AppendFormat(
        "HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n", name.string());
    sb.Append(prefix + g_tab).Append("return false;\n");
    sb.Append(prefix).Append("}\n");
}

void ASTStringType::EmitCUnMarshalling(const String& name, const String& gotoLabel, StringBuilder& sb,
    const String& prefix, std::vector<String>& freeObjStatements) const
{
    sb.Append(prefix).AppendFormat("const char *%s = HdfSbufReadString(data);\n", name.string());
    sb.Append(prefix).AppendFormat("if (%s == NULL) {\n", name.string());
    sb.Append(prefix + g_tab).AppendFormat(
        "HDF_LOGE(\"%%{public}s: read %s failed!\", __func__);\n", name.string());
    EmitFreeStatements(freeObjStatements, sb, prefix + g_tab);
    sb.Append(prefix + g_tab).AppendFormat("goto %s;\n", gotoLabel.string());
    sb.Append(prefix).Append("}\n");
}

void ASTStringType::EmitCppMarshalling(const String& parcelName, const String& name, StringBuilder& sb,
    const String& prefix, unsigned int innerLevel) const
{
    sb.Append(prefix).AppendFormat("if (!%s.WriteString(%s)) {\n", parcelName.string(), name.string());
    sb.Append(prefix + g_tab).AppendFormat(
        "HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n", name.string());
    sb.Append(prefix + g_tab).Append("return false;\n");
    sb.Append(prefix).Append("}\n");
}

void ASTStringType::EmitCppUnMarshalling(const String& parcelName, const String& name, StringBuilder& sb,
    const String& prefix, bool emitType, unsigned int innerLevel) const
{
    if (emitType) {
        sb.Append(prefix).AppendFormat("%s %s;\n", EmitCppType().string(), name.string());
    }
    sb.Append(prefix).AppendFormat("if (!%s.ReadString(%s)) {\n", parcelName.string(), name.string());
    sb.Append(prefix + g_tab).AppendFormat("HDF_LOGE(\"%%{public}s: read %s failed!\", __func__);\n", name.string());
    sb.Append(prefix + g_tab).Append("return false;\n");
    sb.Append(prefix).Append("}\n");
}

void ASTStringType::EmitMemoryRecycle(const String& name, bool isClient, bool ownership, StringBuilder& sb,
    const String& prefix) const
{
    sb.Append(prefix).AppendFormat("if (%s != NULL) {\n", name.string());
    sb.Append(prefix + g_tab).AppendFormat("OsalMemFree(%s);\n", name.string());
    sb.Append(prefix + g_tab).AppendFormat("%s = NULL;\n", name.string());
    sb.Append(prefix).Append("}\n");
}

void ASTStringType::EmitJavaWriteVar(const String& parcelName, const String& name, StringBuilder& sb,
    const String& prefix) const
{
    sb.Append(prefix).AppendFormat("%s.writeString(%s);\n", parcelName.string(), name.string());
}

void ASTStringType::EmitJavaReadVar(const String& parcelName, const String& name, StringBuilder& sb,
    const String& prefix) const
{
    sb.Append(prefix).AppendFormat("%s = %s.readString();\n", name.string(), parcelName.string());
}

void ASTStringType::EmitJavaReadInnerVar(const String& parcelName, const String& name, bool isInner,
    StringBuilder& sb, const String& prefix) const
{
    sb.Append(prefix).AppendFormat("%s %s = %s.readString();\n",
        EmitJavaType(TypeMode::NO_MODE).string(), name.string(), parcelName.string());
}
} // namespace HDI
} // namespace OHOS