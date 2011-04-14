/*=========================================================================

  Program:   Visualization Toolkit
  Module:    SQLQuery.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/*-------------------------------------------------------------------------
  Copyright 2008 Sandia Corporation.
  Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
  the U.S. Government retains certain rights in this software.
-------------------------------------------------------------------------*/
#include "SQLQuery.h"

#include "vtkObjectFactory.h"
#include "vtkSQLDatabase.h"
#include "vtkVariantArray.h"

#include "vtksys/SystemTools.hxx"


SQLQuery::SQLQuery()
{
  m_Database = NULL;
  m_Active = false;
}

SQLQuery::~SQLQuery()
{
}

vtkCxxSetObjectMacro(SQLQuery, Database, vtkSQLDatabase);

void SQLQuery::PrintSelf(ostream &os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "Query: " << (m_Query ? m_Query : "NULL") << endl;
  os << indent << "Database: " << (this->Database ? "" : "NULL") << endl;
  if (this->Database)
    {
    this->Database->PrintSelf(os, indent.GetNextIndent());
    }
}

std::string SQLQuery::EscapeString( const std::string& s, bool addSurroundingQuotes )
{
  std::string d;
  if ( addSurroundingQuotes )
    {
    d += '\'';
    }

  for ( std::string::iterator it = s.begin(); it != s.end(); ++ it )
    {
    if ( *it == '\'' )
      {
      d += '\''; // Single quotes are escaped by repeating them
      }
    d += *it;
    }

  if ( addSurroundingQuotes )
    {
    d += '\'';
    }
  return d;
}

std::string SQLQuery::EscapeString( const std::string& src, bool addSurroundingQuotes )
{
  std::string sstr( src );
  std::string dstr = this->EscapeString( sstr, addSurroundingQuotes );
  return dstr;
}

bool SQLQuery::BindParameter(int vtkNotUsed(index), unsigned char vtkNotUsed(value))
{
  itkErrorMacro(<<"This database driver does not support bound parameters.");
  return false;
}

bool SQLQuery::BindParameter(int vtkNotUsed(index), signed char vtkNotUsed(value))
{
  itkErrorMacro(<<"This database driver does not support bound parameters.");
  return false;
}


bool SQLQuery::BindParameter(int vtkNotUsed(index), unsigned short vtkNotUsed(value))
{
  itkErrorMacro(<<"This database driver does not support bound parameters.");
  return false;
}

bool SQLQuery::BindParameter(int vtkNotUsed(index), short vtkNotUsed(value))
{
  itkErrorMacro(<<"This database driver does not support bound parameters.");
  return false;
}

bool SQLQuery::BindParameter(int vtkNotUsed(index), unsigned int vtkNotUsed(value))
{
  itkErrorMacro(<<"This database driver does not support bound parameters.");
  return false;
}

bool SQLQuery::BindParameter(int vtkNotUsed(index), int vtkNotUsed(value))
{
  itkErrorMacro(<<"This database driver does not support bound parameters.");
  return false;
}

bool SQLQuery::BindParameter(int vtkNotUsed(index), unsigned long vtkNotUsed(value))
{
  itkErrorMacro(<<"This database driver does not support bound parameters.");
  return false;
}

bool SQLQuery::BindParameter(int vtkNotUsed(index), long vtkNotUsed(value))
{
  itkErrorMacro(<<"This database driver does not support bound parameters.");
  return false;
}

bool SQLQuery::BindParameter(int vtkNotUsed(index), vtkTypeUInt64 vtkNotUsed(value))
{
  itkErrorMacro(<<"This database driver does not support bound parameters.");
  return false;
}

bool SQLQuery::BindParameter(int vtkNotUsed(index), vtkTypeInt64 vtkNotUsed(value))
{
  itkErrorMacro(<<"This database driver does not support bound parameters.");
  return false;
}

bool SQLQuery::BindParameter(int vtkNotUsed(index), float vtkNotUsed(value))
{
  itkErrorMacro(<<"This database driver does not support bound parameters.");
  return false;
}

bool SQLQuery::BindParameter(int vtkNotUsed(index), double vtkNotUsed(value))
{
  itkErrorMacro(<<"This database driver does not support bound parameters.");
  return false;
}

bool SQLQuery::BindParameter(int vtkNotUsed(index), const char *vtkNotUsed(value))
{
  itkErrorMacro(<<"This database driver does not support bound parameters.");
  return false;
}

bool SQLQuery::BindParameter(int vtkNotUsed(index), const char *vtkNotUsed(value), size_t vtkNotUsed(length))
{
  itkErrorMacro(<<"This database driver does not support bound parameters.");
  return false;
}


bool SQLQuery::BindParameter(int vtkNotUsed(index), const std::string &vtkNotUsed(value))
{
  itkErrorMacro(<<"This database driver does not support bound parameters.");
  return false;
}

bool SQLQuery::BindParameter(int vtkNotUsed(index), const void *vtkNotUsed(value), size_t vtkNotUsed(length))
{
  itkErrorMacro(<<"This database driver does not support bound parameters.");
  return false;
}

bool SQLQuery::ClearParameterBindings()
{
  itkErrorMacro(<<"This database driver does not support bound parameters.");
  return false;
}

bool SQLQuery::BindParameter(int index, vtkVariant data)
{
  if (!data.IsValid())
    {
    return true; // binding nulls is a no-op
    }

#define VTK_VARIANT_BIND_PARAMETER(Type,Function) \
  case Type: return this->BindParameter(index, data.Function())

  switch (data.GetType())
    {
    VTK_VARIANT_BIND_PARAMETER(VTK_STRING,ToString);
    VTK_VARIANT_BIND_PARAMETER(VTK_FLOAT,ToFloat);
    VTK_VARIANT_BIND_PARAMETER(VTK_DOUBLE,ToDouble);
    VTK_VARIANT_BIND_PARAMETER(VTK_CHAR,ToChar);
    VTK_VARIANT_BIND_PARAMETER(VTK_UNSIGNED_CHAR,ToUnsignedChar);
    VTK_VARIANT_BIND_PARAMETER(VTK_SIGNED_CHAR,ToSignedChar);
    VTK_VARIANT_BIND_PARAMETER(VTK_SHORT,ToShort);
    VTK_VARIANT_BIND_PARAMETER(VTK_UNSIGNED_SHORT,ToUnsignedShort);
    VTK_VARIANT_BIND_PARAMETER(VTK_INT,ToInt);
    VTK_VARIANT_BIND_PARAMETER(VTK_UNSIGNED_INT,ToUnsignedInt);
    VTK_VARIANT_BIND_PARAMETER(VTK_LONG,ToLong);
    VTK_VARIANT_BIND_PARAMETER(VTK_UNSIGNED_LONG,ToUnsignedLong);
#if defined(VTK_TYPE_USE___INT64)
    VTK_VARIANT_BIND_PARAMETER(VTK___INT64,To__Int64);
    VTK_VARIANT_BIND_PARAMETER(VTK_UNSIGNED___INT64,ToUnsigned__Int64);
#endif
#if defined(VTK_TYPE_USE_LONG_LONG)
    VTK_VARIANT_BIND_PARAMETER(VTK_LONG_LONG,ToLongLong);
    VTK_VARIANT_BIND_PARAMETER(VTK_UNSIGNED_LONG_LONG,ToUnsignedLongLong);
#endif
    case VTK_OBJECT:
      itkErrorMacro(<<"Variants of type VTK_OBJECT cannot be inserted into a database.");
      return false;
    default:
      itkErrorMacro(<<"Variants of type "
                    << data.GetType() << " are not currently supported by BindParameter.");
      return false;
    }
}

// ----------------------------------------------------------------------

bool SQLQuery::SetQuery(const std::string& queryString)
{
  // This is just vtkSetStringMacro from vtkSetGet.h
  if ( m_Query.empty() && queryString.empty() )
    {
    return true;
    }
  if ( !m_Query.empty() && !queryString.empty() &&
     ( !m_Query.compare( queryString ) == 0 ) )
    {
    return true; // query string isn't changing
    }
  if( !queryString.empty() )
    {
    size_t n = strlen(queryString) + 1; 
    char *cp1 =  new char[n]; 
    const char *cp2 = (queryString);
    m_Query = cp1; 
    do { *cp1++ = *cp2++; } while ( --n ); \
    }
   else
    {
    m_Query.clear();
    }
  this->Modified();
  return true;
}

// As above, this is a copy of vtkGetStringMacro from vtkGetSet.h.

std::string SQLQuery::GetQuery()
{
  vtkDebugMacro(<< this->GetClassName() 
                << " (" << this << "): returning Query of " 
                << (m_Query?m_Query:"(null)"));
  return this->m_Query;
} 

