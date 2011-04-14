/*=========================================================================

  Program:   Visualization Toolkit
  Module:    SQLQuery.h

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
// .NAME SQLQuery - executes an sql query and retrieves results
//
// .SECTION Description
// The abstract superclass of SQL query classes.  Instances of subclasses
// of SQLQuery are created using the GetQueryInstance() function in
// vtkSQLDatabase.  To implement a query connection for a new database
// type, subclass both vtkSQLDatabase and SQLQuery, and implement the
// required functions.  For the query class, this involves the following:
//
// Execute() - Execute the query on the database.  No results need to be
//             retrieved at this point, unless you are performing caching.
//
// GetNumberOfFields() - After Execute() is performed, returns the number
//                       of fields in the query results.
//
// GetFieldName() - The name of the field at an index.
//
// GetFieldType() - The data type of the field at an index.
//
// NextRow() - Advances the query results by one row, and returns whether
//             there are more rows left in the query.
//
// DataValue() - Extract a single data value from the current row.
//
// Begin/Rollback/CommitTransaction() - These methods are optional but
// recommended if the database supports transactions.
//
// .SECTION Thanks
// Thanks to Andrew Wilson from Sandia National Laboratories for his work
// on the database classes.
//
// .SECTION See Also
// vtkSQLDatabase

#ifndef __SQLQuery_h
#define __SQLQuery_h

#include "vtkRowQuery.h"
#include "std::string.h" // for EscapeString()

class vtkSQLDatabase;
class vtkVariant;
class vtkVariantArray;

namespace itk
{
class SQLQuery : public vtkRowQuery
{
public:
  vtkTypeMacro(SQLQuery, vtkRowQuery);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // The query string to be executed.  Since some databases will
  // process the query string as soon as it's set, this method returns
  // a boolean to indicate success or failure.
  virtual bool SetQuery(const std::string& query);
  virtual std::string GetQuery();

  // Description:
  // Return true if the query is active (i.e. execution was successful
  // and results are ready to be fetched).  Returns false on error or
  // inactive query.
  bool IsActive() const { return this->Active; }

  // Description:
  // Execute the query.  This must be performed
  // before any field name or data access functions
  // are used.
  virtual bool Execute() = 0;

  // Description:
  // Begin, commit, or roll back a transaction.  If the underlying
  // database does not support transactions these calls will do
  // nothing.
  virtual bool BeginTransaction() { return true; }
  virtual bool CommitTransaction() { return true; }
  virtual bool RollbackTransaction() { return true; }

  // Description:
  // Return the database associated with the query.
  vtkGetObjectMacro(Database, SQLDatabase);

//BTX
  // Description:
  // Bind a parameter to a placeholder in a query.  A full discussion
  // of this feature is beyond the scope of this header file, but in
  // short, here's how it works:
  //
  // Instead of saying "SELECT foo FROM mytable WHERE myfield = 12345"
  // you can say "SELECT foo FROM mytable WHERE myfield = ?".  The ?
  // character is a placeholder for a parameter that must then be
  // bound.  Call BindParameter(0, 12345) to bind the integer value
  // 12345 to that field.  Placeholders are indexed starting at 0.
  //
  // You are responsible for making sure that the types match when you
  // call BindParameter.  You don't have to get it precisely correct:
  // in general, the SQL driver is smart enough to do things like cast
  // a short to a long or a float to a double.
  //
  // Bound parameters were introduced in ANSI SQL 92.  Please see that
  // standard for more information.
  //
  // Most of these methods are excluded from wrapping because the Java
  // wrapper treats all integer types from char up through 64-bit long
  // as 'int'.  This is incorrect behavior but what else am I going to
  // do?
  //
  // Finally, the default implementation for BindParameter(int,
  // vtkVariant) dispatches to one of the more type-specific versions.  It
  // should be OK to use in database drivers without modification.

  virtual bool BindParameter(int index, unsigned char value);
  virtual bool BindParameter(int index, unsigned short value);
  virtual bool BindParameter(int index, unsigned int value);
  virtual bool BindParameter(int index, unsigned long value);
  // The C and C++ standards leave it up to each compiler to decide
  // whether chars are signed or unsigned by default.  All the other
  // types are signed unless otherwise specified.
  virtual bool BindParameter(int index, signed char value);
  virtual bool BindParameter(int index, short value);
//ETX
  virtual bool BindParameter(int index, int value);
//BTX
  virtual bool BindParameter(int index, long value);
  virtual bool BindParameter(int index, Identifier value);
  virtual bool BindParameter(int index, Identifier value);
//ETX
  virtual bool BindParameter(int index, float value);
  virtual bool BindParameter(int index, double value);
  // Description:
  // Bind a string value -- string must be null-terminated
  virtual bool BindParameter(int index, const char *stringValue);
  // Description:
  // Bind a string value by specifying an array and a size
  virtual bool BindParameter(int index, const char *stringValue, size_t length);
//BTX
  virtual bool BindParameter(int index, const std::string &string);
//ETX
  virtual bool BindParameter(int index, vtkVariant var);
  // Description:
  // Bind a blob value.  Not all databases support blobs as a data
  // type.  Check vtkSQLDatabase::IsSupported(VTK_SQL_FEATURE_BLOB) to
  // make sure.
  virtual bool BindParameter(int index, const void *data, size_t length);
  // Description:
  // Reset all parameter bindings to NULL.
  virtual bool ClearParameterBindings();

//BTX
  // Description:
  // Escape a string for inclusion into an SQL query.
  // If \a addSurroundingQuotes is true, then quotation marks appropriate to the
  // backend database will be added to enclose the escaped string. This argument
  // defaults to true.
  //
  // A default, simple-minded implementation is provided for
  // database backends that do not provde a way to escape
  // strings for use inside queries.
  virtual std::string EscapeString( std::string s, bool addSurroundingQuotes = true );
//ETX

  // Description:
  // Escape a string for inclusion into an SQL query.
  // This method exists to provide a wrappable version of
  // the method that takes and returns std::string objects.
  // You are responsible for calling delete [] on the
  // character array returned by this method.
  // This method simply calls the std::string variant and thus
  // need not be re-implemented by subclasses.
  std::string EscapeString( const std::string& src, bool addSurroundingQuotes );

protected:
  SQLQuery();
  ~SQLQuery();

  // Description:
  // Set the database associated with the query.
  // This is only to be called by the corresponding
  // database class on creation of the query in
  // GetQueryInstance().
  void SetDatabase(SQLDatabase* db);

  std::string m_Query;
  SQLDatabase::Pointer m_Database;
  bool m_Active;

private:
  SQLQuery(const SQLQuery &); // Not implemented.
  void operator=(const SQLQuery &); // Not implemented.
};
}

#endif // __SQLQuery_h
