/*
 * Copyright 2017, 2018 Simon Rasmussen (refactor)
 *
 * Copyright 2015, 2016 Thomas Timm Andersen (original version)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once
#include <vector>
#include "ur_rtde_driver/comm/bin_parser.h"
#include "ur_rtde_driver/comm/pipeline.h"
#include "ur_rtde_driver/comm/parser.h"
#include "ur_rtde_driver/primary/package_header.h"
#include "ur_rtde_driver/primary/robot_state.h"
#include "ur_rtde_driver/primary/robot_message.h"
#include "ur_rtde_driver/primary/robot_state/robot_mode_data.h"
#include "ur_rtde_driver/primary/robot_state/kinematics_info.h"
#include "ur_rtde_driver/primary/robot_state/master_board.h"
#include "ur_rtde_driver/primary/robot_message/version_message.h"

namespace ur_driver
{
namespace primary_interface
{
using namespace comm;
class PrimaryParser : comm::Parser<PackageHeader>
{
private:
  RobotState* state_from_type(robot_state_type type)
  {
    switch (type)
    {
      /*case robot_state_type::ROBOT_MODE_DATA:
        // SharedRobotModeData* rmd = new SharedRobotModeData();

        //return new rmd;
      case robot_state_type::MASTERBOARD_DATA:
        return new MBD;*/
      case robot_state_type::KINEMATICS_INFO:
        return new KinematicsInfo;
      default:
        return nullptr;
    }
  }

  RobotMessage* message_from_type(message_type type, uint64_t timestamp, uint8_t source)
  {
    switch (type)
    {
      /*case robot_state_type::ROBOT_MODE_DATA:
        // SharedRobotModeData* rmd = new SharedRobotModeData();

        //return new rmd;
      case robot_state_type::MASTERBOARD_DATA:
        return new MBD;*/
      case message_type::ROBOT_MESSAGE_VERSION:
        return new VersionMessage(timestamp, source);
      default:
        return nullptr;
    }
  }

public:
  bool parse(BinParser& bp, std::vector<std::unique_ptr<PrimaryPackage>>& results)
  {
    int32_t packet_size;
    robot_message_type type;
    bp.parse(packet_size);
    bp.parse(type);

    switch (type)
    {
      case robot_message_type::ROBOT_STATE:
      {
        while (!bp.empty())
        {
          if (!bp.checkSize(sizeof(uint32_t)))
          {
            LOG_ERROR("Failed to read sub-package length, there's likely a parsing error");
            return false;
          }
          uint32_t sub_size = bp.peek<uint32_t>();
          if (!bp.checkSize(static_cast<size_t>(sub_size)))
          {
            LOG_WARN("Invalid sub-package size of %" PRIu32 " received!", sub_size);
            return false;
          }

          // deconstruction of a sub parser will increment the position of the parent parser
          BinParser sbp(bp, sub_size);
          sbp.consume(sizeof(sub_size));
          robot_state_type type;
          sbp.parse(type);

          std::unique_ptr<PrimaryPackage> packet(state_from_type(type));

          if (packet == nullptr)
          {
            sbp.consume();

            // TODO: create robot state type here
            continue;
          }

          if (!packet->parseWith(sbp))
          {
            LOG_ERROR("Sub-package parsing of type %d failed!", static_cast<int>(type));
            return false;
          }

          results.push_back(std::move(packet));

          if (!sbp.empty())
          {
            LOG_ERROR("Sub-package of type %d was not parsed completely!", static_cast<int>(type));
            sbp.debug();
            return false;
          }
        }

        break;
      }

      case robot_message_type::ROBOT_MESSAGE:
      {
        uint64_t timestamp;
        uint8_t source;
        message_type message_type;

        bp.parse(timestamp);
        bp.parse(source);
        bp.parse(message_type);

        bool parsed = false;

        std::unique_ptr<PrimaryPackage> packet(message_from_type(message_type, timestamp, source));
        if (!packet->parseWith(bp))
        {
          LOG_ERROR("Package parsing of type %d failed!", static_cast<int>(message_type));
          return false;
        }

        results.push_back(std::move(packet));
        return parsed;
        break;
      }

      default:
      {
        LOG_WARN("Invalid state message type recieved: %u", static_cast<uint8_t>(type));
        bp.consume();
        return true;
      }
    }
    return true;
  }
};

}  // namespace primary_interface
}  // namespace ur_driver
