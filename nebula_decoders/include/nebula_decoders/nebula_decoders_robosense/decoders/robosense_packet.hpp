#pragma once

#include "boost/endian/buffers.hpp"

#include <cstddef>
#include <cstdint>
#include <ctime>
#include <stdexcept>
#include <string>

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

  [[nodiscard]] uint16_t get_azimuth() const { return azimuth.value(); }
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

struct IpAddress
{
  boost::endian::big_uint8_buf_t first_octet;
  boost::endian::big_uint8_buf_t second_octet;
  boost::endian::big_uint8_buf_t third_octet;
  boost::endian::big_uint8_buf_t fourth_octet;

  [[nodiscard]] std::string to_string() const
  {
    return std::to_string(first_octet.value()) + "." + std::to_string(second_octet.value()) + "." +
           std::to_string(third_octet.value()) + "." + std::to_string(fourth_octet.value());
  }
};

struct MacAddress
{
  boost::endian::big_uint8_buf_t first_octet;
  boost::endian::big_uint8_buf_t second_octet;
  boost::endian::big_uint8_buf_t third_octet;
  boost::endian::big_uint8_buf_t fourth_octet;
  boost::endian::big_uint8_buf_t fifth_octet;
  boost::endian::big_uint8_buf_t sixth_octet;

  [[nodiscard]] std::string to_string() const
  {
    std::stringstream ss;
    ss << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(first_octet.value())
       << ":" << std::setw(2) << static_cast<int>(second_octet.value()) << ":" << std::setw(2)
       << static_cast<int>(third_octet.value()) << ":" << std::setw(2)
       << static_cast<int>(fourth_octet.value()) << ":" << std::setw(2)
       << static_cast<int>(fifth_octet.value()) << ":" << std::setw(2)
       << static_cast<int>(sixth_octet.value());
    return ss.str();
  }
};

struct Ethernet
{
  IpAddress lidar_ip;
  IpAddress dest_pc_ip;
  MacAddress mac_addr;
  boost::endian::big_uint16_buf_t lidar_out_msop_port;
  boost::endian::big_uint16_buf_t pc_dest_msop_port;
  boost::endian::big_uint16_buf_t lidar_out_difop_port;
  boost::endian::big_uint16_buf_t pc_dest_difop_port;
};

struct FovSetting
{
  boost::endian::big_uint16_buf_t fov_start;
  boost::endian::big_uint16_buf_t fov_end;
};

struct ChannelAngleCorrection
{
  boost::endian::big_uint8_buf_t sign;
  boost::endian::big_uint16_buf_t angle;
};

struct CorrectedVerticalAngle
{
  ChannelAngleCorrection angles[32];
};

struct CorrectedHorizontalAngle
{
  ChannelAngleCorrection angles[32];
};

struct SensorCalibration
{
  CorrectedVerticalAngle corrected_vertical_angle;
  CorrectedHorizontalAngle corrected_horizontal_angle;

  RobosenseCalibrationConfiguration getCalibration() const
  {
    RobosenseCalibrationConfiguration calibration;
    for (size_t i = 0; i < 32; ++i) {
      ChannelCorrection channel_correction;
      channel_correction.azimuth =
        static_cast<float>(corrected_horizontal_angle.angles[i].angle.value()) / 100.0f;
      channel_correction.elevation =
        static_cast<float>(corrected_vertical_angle.angles[i].angle.value()) / 100.0f;
      calibration.calibration.push_back(channel_correction);
    }
    return calibration;
  }
};

struct FirmwareVersion
{
  boost::endian::big_uint8_buf_t first_octet;
  boost::endian::big_uint8_buf_t second_octet;
  boost::endian::big_uint8_buf_t third_octet;
  boost::endian::big_uint8_buf_t fourth_octet;
  boost::endian::big_uint8_buf_t fifth_octet;

  [[nodiscard]] std::string to_string() const
  {
    std::stringstream ss;
    ss << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(first_octet.value())
       << std::setw(2) << static_cast<int>(second_octet.value()) << std::setw(2)
       << static_cast<int>(third_octet.value()) << std::setw(2)
       << static_cast<int>(fourth_octet.value()) << std::setw(2)
       << static_cast<int>(fifth_octet.value());
    return ss.str();
  }
};

struct SerialNumber
{
  boost::endian::big_uint8_buf_t first_octet;
  boost::endian::big_uint8_buf_t second_octet;
  boost::endian::big_uint8_buf_t third_octet;
  boost::endian::big_uint8_buf_t fourth_octet;
  boost::endian::big_uint8_buf_t fifth_octet;
  boost::endian::big_uint8_buf_t sixth_octet;

  [[nodiscard]] std::string to_string() const
  {
    std::stringstream ss;
    ss << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(first_octet.value())
       << std::setw(2) << static_cast<int>(second_octet.value()) << std::setw(2)
       << static_cast<int>(third_octet.value()) << std::setw(2)
       << static_cast<int>(fourth_octet.value()) << std::setw(2)
       << static_cast<int>(fifth_octet.value()) << std::setw(2)
       << static_cast<int>(sixth_octet.value());
    return ss.str();
  }
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
    case ReturnMode::SINGLE:
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