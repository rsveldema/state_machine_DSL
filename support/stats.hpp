#ifndef BASE_STATS__H_H__H
#define BASE_STATS__H_H__H

template<typename T, typename K>
bool contains(const T &l, const K &k)
{
  return std::find(l.begin(), l.end(), k) != l.end();
}

struct CheckerStatistics
{
  uint64_t max_step_count = 0;
  uint64_t steps = 0;

  uint64_t warnings = 0;
  std::vector<std::string> logged_warnings;

  void record_max_step_count(uint64_t count)
  {
    max_step_count = std::max(count, max_step_count);
  }
  
  void warn(const std::string &msg)
  {
    fprintf(stderr, "WARNING: %s\n", msg.c_str());
    
    if (! contains(logged_warnings, msg))
      {
	logged_warnings.push_back(msg);
      }
  }
};


#endif
