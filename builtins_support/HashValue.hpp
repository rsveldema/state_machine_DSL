#ifndef HASH_VALUE_G___H
#define HASH_VALUE_G___H

class HashValue
{
private:
  uint64_t v;

public:
  HashValue()
  {
    v = 0;
  }

  HashValue(uint64_t _v)
  {
    v = _v;
  }

  void add(uint64_t _v)
  {
    v ^= _v;
  }

  void add(const HashValue & _v)
  {
    add(_v.v);
  }

  
  bool operator < (const HashValue &e) const { return v < e.v; }
  bool operator > (const HashValue &e) const { return v > e.v; }
  bool operator >= (const HashValue &e) const { return v >= e.v; }
  bool operator == (const HashValue &e) const { return v == e.v; }
  bool operator != (const HashValue &e) const { return v != e.v; }

  uint64_t get() const { return v; }
};


#endif
