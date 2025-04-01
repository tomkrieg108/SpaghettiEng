
#include "UUID.h"

namespace Utils
{
  static std::random_device s_device;
  static std::mt19937_64 s_rng(s_device());
  static std::uniform_int_distribution<uint64_t> s_dist;

  UUID::UUID() :
    m_uuid{ s_dist(s_rng) }
  {
  } 
}