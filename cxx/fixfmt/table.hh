#pragma once

#include <functional>
#include <limits>
#include <memory>
#include <numeric>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

//------------------------------------------------------------------------------

namespace fixfmt {

using std::string;
using std::unique_ptr;

static constexpr auto MAX_INDEX = std::numeric_limits<long>::max();

class Column
{
public:

  virtual ~Column() = default;

  /**
   * Returns the formatted width of the column.
   */
  virtual int get_width() const = 0;

  /**
   * Returns the number of entries in the column.
   */
  virtual long get_length() const = 0;

  /**
   * Formats entry 'index'.
   */
  virtual string operator()(long index) const = 0;

};


template<typename TYPE, typename FMT>
class ColumnImpl
  : public Column
{
public:

  ColumnImpl(TYPE const* values, long length, FMT format)
  : values_(values),
    length_(length),
    format_(std::move(format))
  {
  }

  virtual ~ColumnImpl() override {}

  virtual int get_width() const override { return format_.get_width(); }

  virtual long get_length() const override { return length_; }

  virtual string operator()(long const index) const override
  {
    return format_(values_[index]);
  }

  FMT const& get_format() const { return format_; }

private:

  TYPE const* const values_;
  long const length_;
  FMT const format_;

};


/**
 * A column with one degree of indirection through an integral index column.
 *
 * For categorical columns.
 */
template<typename IDXTYPE, typename TYPE, typename FMT>
class IndexedColumn
  : public Column
{
public:

  IndexedColumn(
    IDXTYPE const* const index, 
    long const index_length, 
    ColumnImpl<TYPE, FMT> column)
    : index_(index),
      index_length_(index_length),
      column_(std::move(column))
  {
  }

  virtual ~IndexedColumn() override {}

  virtual int get_width() const override 
  { 
    return column_.get_format().get_width(); 
  }

  virtual long get_length() const override { return index_length_; }

  virtual string operator()(long const index) const override
  {
    return column_(index_[index]);
  }

private:

  IDXTYPE const* const index_;
  long const index_length_;
  ColumnImpl<TYPE, FMT> const column_;

};


class StringColumn
  : public Column
{
public:

  StringColumn(string str)
  : str_(std::move(str))
  {
  }

  virtual int get_width() const override { return string_length(str_); }

  virtual long get_length() const override { return MAX_INDEX; }

  virtual string operator()(long const /* index */) const override
    { return str_; }

private:

  string str_;

};


//------------------------------------------------------------------------------

class Table
  : public Column
{
public:

  Table() : width_(0), length_(MAX_INDEX) {}

  void 
  add_column(
    unique_ptr<Column> col)
  {
    width_ += col->get_width();
    length_ = std::min(length_, col->get_length());
    columns_.push_back(std::move(col));
  }

  void 
  add_string(
    string str)
  { 
    add_column(unique_ptr<Column>(new StringColumn(std::move(str)))); 
  }

  virtual int get_width() const override { return width_; }
  virtual long get_length() const override { return length_; }

  virtual string 
  operator()(
    long const index) 
    const override
  {
    std::stringstream result;
    for (auto i = columns_.begin(); i != columns_.end(); ++i)
      result << (**i)(index);
    return result.str();
  }

private:

  std::vector<unique_ptr<Column>> columns_;
  int width_;
  long length_;

};


}  // namespace fixfmt

