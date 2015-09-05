#pragma once

#include <functional>
#include <memory>
#include <numeric>
#include <string>
#include <utility>
#include <vector>

//------------------------------------------------------------------------------

namespace fixfmt {

using std::string;
using std::unique_ptr;

class Column
{
public:

  virtual ~Column() = default;
  virtual size_t get_width() const = 0;
  virtual void format(size_t index, char* buf) const = 0;

};


template<typename TYPE, typename FMT>
class ColumnImpl
  : public Column
{
public:

  ColumnImpl(TYPE const* values, FMT format)
  : values_(values),
    format_(std::move(format))
  {
  }

  virtual ~ColumnImpl() override {};

  virtual size_t get_width() const { return format_.get_width(); }

  virtual void format(size_t const index, char* const buf) const
  {
    format_.format(values_[index], buf);
  };


private:

  TYPE const* const values_;
  FMT const format_;

};


class StringColumn
  : public Column
{
public:

  StringColumn(string str)
  : str_(std::move(str))
  {
  }

  virtual size_t get_width() const { return str_.length(); }

  virtual void format(size_t const /* index */, char* const buf) const
  {
    str_.copy(buf, str_.length());
  }

private:

  string str_;

};


class Table
  : public Column
{
public:

  Table() : width_(0) {}

  void add_string(string str)
  {
    add_column(unique_ptr<Column>(new StringColumn(std::move(str))));
  }

  void add_column(unique_ptr<Column> col)
  {
    width_ += col->get_width();
    columns_.push_back(std::move(col));
  }

  virtual size_t get_width() const { return width_; }

  virtual void format(size_t const index, char* const buf) const
  {
    char* p = buf;
    for (auto i = columns_.begin(); i != columns_.end(); ++i) {
      Column const& col = **i;
      col.format(index, p);
      p += col.get_width();
    }
  }

private:

  std::vector<unique_ptr<Column>> columns_;
  size_t width_;

};


}  // namespace fixfmt

