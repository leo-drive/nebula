#pragma once

#include "boost/endian/buffers.hpp"

#include <cstddef>
#include <cstdint>
#include <ctime>
#include <stdexcept>

namespace nebula
{
namespace drivers
{
namespace robosense_packet
{

#pragma pack(push, 1)

struct Unit
{
  boost::endian::big_uint16_buf_t distance;
  boost::endian::big_uint8_buf_t reflectivity;
};

template <typename UnitT, size_t UnitN>
struct Block
{
  boost::endian::big_uint16_buf_t flag;
  boost::endian::big_uint16_buf_t azimuth;
  UnitT units[UnitN];
  typedef UnitT unit_t;

  uint16_t get_azimuth() const { return azimuth.value(); }
};

template <typename BlockT, size_t BlockN>
struct Body
{
  typedef BlockT block_t;
  BlockT blocks[BlockN];
};

/// @brief Base struct for all Robosense packets. This struct is not allowed to have any non-static
/// members, otherwise memory layout is not guaranteed for the derived structs.
/// @tparam nBlocks The number of blocks in the packet
/// @tparam nChannels The number of channels per block
/// @tparam maxReturns The maximum number of returns, e.g. 2 for dual return
/// @tparam degreeSubdivisions The resolution of the azimuth angle in the packet, e.g. 100 if packet
/// azimuth is given in 1/100th of a degree
template <size_t nBlocks, size_t nChannels, size_t maxReturns, size_t degreeSubdivisions>
struct PacketBase
{
  static constexpr size_t N_BLOCKS = nBlocks;
  static constexpr size_t N_CHANNELS = nChannels;
  static constexpr size_t MAX_RETURNS = maxReturns;
  static constexpr size_t DEGREE_SUBDIVISIONS = degreeSubdivisions;
};
#pragma pack(pop)

/// @brief Get the number of returns for a given return mode
/// @param return_mode The return mode
/// @return The number of returns
size_t get_n_returns(ReturnMode return_mode)
{
  switch (return_mode) {
    case ReturnMode::DUAL:
      return 2;
    case ReturnMode::STRONGEST:
      return 1;
    case ReturnMode::LAST:
      return 1;
    case ReturnMode::FIRST:
      return 1;
    default:
      throw std::runtime_error("Unknown return mode");
  }
}

/// @brief Get timestamp from packet in nanoseconds
/// @tparam PacketT The packet type
/// @param packet The packet to get the timestamp from
/// @return The timestamp in nanoseconds
template <typename PacketT>
uint64_t get_timestamp_ns(const PacketT & packet)
{
  return packet.header.timestamp.get_time_in_ns();
}

/// @brief Get the distance unit of the given packet type in meters. Distance values in the packet,
/// multiplied by this value, yield the distance in meters.
/// @tparam PacketT The packet type
/// @param packet The packet to get the distance unit from
/// @return The distance unit in meters
template <typename PacketT>
double get_dis_unit(const PacketT & packet)
{
  // Packets define distance unit in millimeters, convert to meters here
  const uint8_t range_resolution = packet.header.range_resolution.value();
  if (range_resolution == 0) {
    return 0.0025;
  } else if (range_resolution == 1) {
    return 0.0050;
  }
  // Should throw an error here?
  // throw std::runtime_error("Unknown range resolution");
}

}  // namespace robosense_packet
}  // namespace drivers
}  // namespace nebula