// Copyright 2012 Cloudera Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "runtime/types.h"
#include <ostream>
#include <sstream>

#include "gen-cpp/TCLIService_constants.h"

using namespace std;
using namespace apache::hive::service::cli::thrift;

namespace impala {

PrimitiveType ThriftToType(TPrimitiveType::type ttype) {
  switch (ttype) {
    case TPrimitiveType::INVALID_TYPE: return INVALID_TYPE;
    case TPrimitiveType::NULL_TYPE: return TYPE_NULL;
    case TPrimitiveType::BOOLEAN: return TYPE_BOOLEAN;
    case TPrimitiveType::TINYINT: return TYPE_TINYINT;
    case TPrimitiveType::SMALLINT: return TYPE_SMALLINT;
    case TPrimitiveType::INT: return TYPE_INT;
    case TPrimitiveType::BIGINT: return TYPE_BIGINT;
    case TPrimitiveType::FLOAT: return TYPE_FLOAT;
    case TPrimitiveType::DOUBLE: return TYPE_DOUBLE;
    case TPrimitiveType::DATE: return TYPE_DATE;
    case TPrimitiveType::DATETIME: return TYPE_DATETIME;
    case TPrimitiveType::TIMESTAMP: return TYPE_TIMESTAMP;
    case TPrimitiveType::STRING: return TYPE_STRING;
    case TPrimitiveType::VARCHAR: return TYPE_VARCHAR;
    case TPrimitiveType::BINARY: return TYPE_BINARY;
    case TPrimitiveType::DECIMAL: return TYPE_DECIMAL;
    case TPrimitiveType::CHAR: return TYPE_CHAR;
    default: return INVALID_TYPE;
  }
}

TPrimitiveType::type ToThrift(PrimitiveType ptype) {
  switch (ptype) {
    case INVALID_TYPE: return TPrimitiveType::INVALID_TYPE;
    case TYPE_NULL: return TPrimitiveType::NULL_TYPE;
    case TYPE_BOOLEAN: return TPrimitiveType::BOOLEAN;
    case TYPE_TINYINT: return TPrimitiveType::TINYINT;
    case TYPE_SMALLINT: return TPrimitiveType::SMALLINT;
    case TYPE_INT: return TPrimitiveType::INT;
    case TYPE_BIGINT: return TPrimitiveType::BIGINT;
    case TYPE_FLOAT: return TPrimitiveType::FLOAT;
    case TYPE_DOUBLE: return TPrimitiveType::DOUBLE;
    case TYPE_DATE: return TPrimitiveType::DATE;
    case TYPE_DATETIME: return TPrimitiveType::DATETIME;
    case TYPE_TIMESTAMP: return TPrimitiveType::TIMESTAMP;
    case TYPE_STRING: return TPrimitiveType::STRING;
    case TYPE_VARCHAR: return TPrimitiveType::VARCHAR;
    case TYPE_BINARY: return TPrimitiveType::BINARY;
    case TYPE_DECIMAL: return TPrimitiveType::DECIMAL;
    case TYPE_CHAR: return TPrimitiveType::CHAR;
    default: return TPrimitiveType::INVALID_TYPE;
  }
}

string TypeToString(PrimitiveType t) {
  switch (t) {
    case INVALID_TYPE: return "INVALID";
    case TYPE_NULL: return "NULL";
    case TYPE_BOOLEAN: return "BOOLEAN";
    case TYPE_TINYINT: return "TINYINT";
    case TYPE_SMALLINT: return "SMALLINT";
    case TYPE_INT: return "INT";
    case TYPE_BIGINT: return "BIGINT";
    case TYPE_FLOAT: return "FLOAT";
    case TYPE_DOUBLE: return "DOUBLE";
    case TYPE_DATE: return "DATE";
    case TYPE_DATETIME: return "DATETIME";
    case TYPE_TIMESTAMP: return "TIMESTAMP";
    case TYPE_STRING: return "STRING";
    case TYPE_VARCHAR: return "VARCHAR";
    case TYPE_BINARY: return "BINARY";
    case TYPE_DECIMAL: return "DECIMAL";
    case TYPE_CHAR: return "CHAR";
  };
  return "";
}

string TypeToOdbcString(PrimitiveType t) {
  // ODBC driver requires types in lower case
  switch (t) {
    case INVALID_TYPE: return "invalid";
    case TYPE_NULL: return "null";
    case TYPE_BOOLEAN: return "boolean";
    case TYPE_TINYINT: return "tinyint";
    case TYPE_SMALLINT: return "smallint";
    case TYPE_INT: return "int";
    case TYPE_BIGINT: return "bigint";
    case TYPE_FLOAT: return "float";
    case TYPE_DOUBLE: return "double";
    case TYPE_DATE: return "date";
    case TYPE_DATETIME: return "datetime";
    case TYPE_TIMESTAMP: return "timestamp";
    case TYPE_STRING: return "string";
    case TYPE_VARCHAR: return "string";
    case TYPE_BINARY: return "binary";
    case TYPE_DECIMAL: return "decimal";
    case TYPE_CHAR: return "char";
  };
  return "unknown";
}

TTypeEntry ColumnType::ToHs2Type() const {
  TPrimitiveTypeEntry type_entry;
  switch (type) {
    // Map NULL_TYPE to BOOLEAN, otherwise Hive's JDBC driver won't
    // work for queries like "SELECT NULL" (IMPALA-914).
    case TYPE_NULL:
      type_entry.__set_type(TTypeId::BOOLEAN_TYPE);
      break;
    case TYPE_BOOLEAN:
      type_entry.__set_type(TTypeId::BOOLEAN_TYPE);
      break;
    case TYPE_TINYINT:
      type_entry.__set_type(TTypeId::TINYINT_TYPE);
      break;
    case TYPE_SMALLINT:
      type_entry.__set_type(TTypeId::SMALLINT_TYPE);
      break;
    case TYPE_INT:
      type_entry.__set_type(TTypeId::INT_TYPE);
      break;
    case TYPE_BIGINT:
      type_entry.__set_type(TTypeId::BIGINT_TYPE);
      break;
    case TYPE_FLOAT:
      type_entry.__set_type(TTypeId::FLOAT_TYPE);
      break;
    case TYPE_DOUBLE:
      type_entry.__set_type(TTypeId::DOUBLE_TYPE);
      break;
    case TYPE_TIMESTAMP:
      type_entry.__set_type(TTypeId::TIMESTAMP_TYPE);
      break;
    case TYPE_STRING:
      type_entry.__set_type(TTypeId::STRING_TYPE);
      break;
    case TYPE_BINARY:
      type_entry.__set_type(TTypeId::BINARY_TYPE);
      break;
    case TYPE_DECIMAL: {
      TTypeQualifierValue tprecision;
      tprecision.__set_i32Value(precision);
      TTypeQualifierValue tscale;
      tscale.__set_i32Value(scale);

      TTypeQualifiers type_quals;
      type_quals.qualifiers[g_TCLIService_constants.PRECISION] = tprecision;
      type_quals.qualifiers[g_TCLIService_constants.SCALE] = tscale;
      type_entry.__set_typeQualifiers(type_quals);
      type_entry.__set_type(TTypeId::DECIMAL_TYPE);
      break;
    }
    case TYPE_CHAR:
    case TYPE_VARCHAR: {
      TTypeQualifierValue tmax_len;
      tmax_len.__set_i32Value(len);

      TTypeQualifiers type_quals;
      type_quals.qualifiers[g_TCLIService_constants.CHARACTER_MAXIMUM_LENGTH] = tmax_len;
      type_entry.__set_typeQualifiers(type_quals);
      type_entry.__set_type(
          (type == TYPE_CHAR) ? TTypeId::CHAR_TYPE : TTypeId::VARCHAR_TYPE);
      break;
    }
    default:
      // HiveServer2 does not have a type for invalid, date and datetime.
      DCHECK(false) << "bad TypeToTValueType() type: " << DebugString();
      type_entry.__set_type(TTypeId::STRING_TYPE);
  };

  TTypeEntry result;
  result.__set_primitiveEntry(type_entry);
  return result;
}

string ColumnType::DebugString() const {
  stringstream ss;
  switch (type) {
    case TYPE_CHAR:
      ss << "CHAR(" << len << ")";
      return ss.str();
    case TYPE_DECIMAL:
      ss << "DECIMAL(" << precision << ", " << scale << ")";
      return ss.str();
    default:
      return TypeToString(type);
  }
}

ostream& operator<<(ostream& os, const ColumnType& type) {
  os << type.DebugString();
  return os;
}

}
