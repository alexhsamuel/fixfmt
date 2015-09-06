#pragma once

#include <functional>
#include <limits>
#include <memory>
#include <numeric>
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
   * Formats entry 'index' of the column into 'buf'.  
   *
   * 'index' must be at least zero and less than 'get_length()'.  'buf' must
   * accommodate 'get_width()' characters.  The formatted entry is not
   * NUL-terminated.
   */
  virtual void format(long index, char* buf) const = 0;

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

  virtual ~ColumnImpl() override {};

  virtual int get_width() const override { return format_.get_width(); }

  virtual long get_length() const override { return length_; }

  virtual void format(long const index, char* const buf) const override
  {
    format_.format(values_[index], buf);
  };


private:

  TYPE const* const values_;
  long const length_;
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

  virtual int get_width() const override { return str_.length(); }

  virtual long get_length() const override { return MAX_INDEX; }

  virtual void format(long const /* index */, char* const buf) const override
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

  Table() : width_(0), length_(MAX_INDEX) {}

  void add_column(unique_ptr<Column> col)
  {
    width_ += col->get_width();
    length_ = std::min(length_, col->get_length());
    columns_.push_back(std::move(col));
  }

  void add_string(string str)
  {
    add_column(unique_ptr<Column>(new StringColumn(std::move(str))));
  }

  virtual int get_width() const override { return width_; }

  virtual long get_length() const override { return length_; }

  virtual void format(long const index, char* const buf) const override
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
  int width_;
  long length_;

};


}  // namespace fixfmt

