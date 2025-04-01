#pragma once

namespace Utils
{
  class UUID
  {
  public:
    UUID();
    uint64_t GetUUID() const { return m_uuid; }
    operator uint64_t() const { return m_uuid; } //allows implicit conversion
    //explicit operator uint64_t() const { return m_uuid; } // need to use static_cast<uint64_t>(UUID())

  private:
    uint64_t m_uuid = 0;
  };
}

/*
 This enables UUID's to be used in unordered_map
 i.e. std::unordered_map<UUID, Component> etc
*/

namespace std {
	template <typename T> struct hash;

	//custom specialization of the std::hash 'functor' 
	template<>
	struct hash<Utils::UUID>
	{
		std::size_t operator()(const Utils::UUID& uuid) const
		{
			return (uint64_t)uuid;
		}
	};

}